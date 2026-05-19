#include <Arduino.h>
#include "lpf.h"
#include "beacon.h"
#if defined(FEATURE_IBUS)
#include "telemetry.h"
#pragma message "compil fs_telemetrie.h !."
#include <IBusBM.h>
#include <TinyGPS++.h>
#if defined(ESP32)
#if defined(CONFIG_IDF_TARGET_ESP32C3)
#include "driver/adc.h"
HardwareSerial iBusSerial(0);  // UART 0 sur ESP32C3 ( pour iBus  1 wire / half duplex)
#pragma message "Utilisation de HardwareSerial 0 ESP32C3 pour iBus  !."
#else
HardwareSerial iBusSerial(2);  // UART 2 sur ESP32 ( pour iBus  1 wire / half duplex)
#pragma message "Utilisation de HardwareSerial 2 ESP32 pour iBus !."
#endif
#else
#error "Il faut utiliser une carte ESP32 ou ESP32C3 pour la tememetrie iBus !."
#endif
IBusBM iBus;

uint8_t gps_lat;
uint8_t gps_lon;
uint8_t gps_alt;
uint8_t gps_speed;
uint8_t gps_spe;
uint8_t gps_status;
uint8_t gps_head;
uint8_t gps_second;

#if defined(CONFIG_IDF_TARGET_ESP32C3)
extern struct pref preferences;
extern BeaconWebServer server;

uint8_t batt_cells_ibusid;
uint8_t batt_current_busid;
uint8_t batt_percentage_busid;

enum BatteryType {
  BATTERY_3S,
  BATTERY_4S,
  BATTERY_UNKNOWN
};

static struct {
    LPF_U32_slow voltage;
    LPF_U32_slow current;
    int16_t currentOffset;
    int8_t batteryType;
} batteryData;

void adcProcess() {
  static bool initialized = false;
  static uint32_t numberOfReadings = 0;
  
  batteryData.voltage = analogReadMilliVolts(ADC1_CHANNEL_0);
  batteryData.current = analogReadMilliVolts(ADC1_CHANNEL_1);
  
  if(!initialized && numberOfReadings > 5000) {
    // Get filtered sensor values to set initial offsets for current measurement
    batteryData.currentOffset = batteryData.current();
    initialized = true;
  } else {
    numberOfReadings++;
  }
}

void identifyBatteryType() {
  // Simple heuristic based on voltage to determine battery type
  int32_t voltage = (float)analogReadMilliVolts(ADC1_CHANNEL_0) * preferences.adcCalib[0] * 100.;
  if (voltage > 1400) { // >14V likely a 4S LiPo
    batteryData.batteryType = BATTERY_4S;
  } else if (voltage > 1080) {
    batteryData.batteryType = BATTERY_3S;
  } else {
    batteryData.batteryType = BATTERY_UNKNOWN;
  }
}

uint8_t calcBatteryPercentage()
{
  int32_t voltage = getBatterycentiVolts();
  if (batteryData.batteryType == BATTERY_4S) {
    // 4S LiPo: 16.8V full, 13.08V empty
    if (voltage > 16500) {
      return 100;
    } else if (voltage > 1644) {
      return 90;
    } else if (voltage > 1608) {
      return 80;
    } else if (voltage > 1580) {
      return 70;
    } else if (voltage > 1548) {
      return 60;
    } else if (voltage > 1536) {
      return 50;
    } else if (voltage > 1520) {
      return 40;
    } else if (voltage > 1508) {
      return 30;
    } else if (voltage > 1492) {
      return 20;
    } else if (voltage > 1460) {
      return 10;
    } else {
      return 0;
    }
  } else if (batteryData.batteryType == BATTERY_3S) {
    // 3S LiPo: 12.6V full, 9.81V empty
      if (voltage > 1255) {
      return 100;
    } else if (voltage > 1233) {
      return 90;
    } else if (voltage > 1206) {
      return 80;
    } else if (voltage > 1185) {
      return 70;
    } else if (voltage > 1161) {
      return 60;
    } else if (voltage > 1152) {
      return 50;
    } else if (voltage > 1140) {
      return 40;
    } else if (voltage > 1131) {
      return 30;
    } else if (voltage > 1119) {
      return 20;
    } else if (voltage > 1100) {
      return 10;
    } else {
      return 0;
    }
  } else {
    return 0; // Unknown battery type, cannot estimate percentage
  }
}

void calibrateSensor(uint8_t channel, float actualValue, int32_t offset = 0) {
  int32_t rawValue = 0;
  
  // Using a switch is slightly cleaner for channel selection
  switch(channel) {
    case 0: rawValue = batteryData.voltage(); break;
    case 1: rawValue = batteryData.current(); break;
    default: return;
  }

  rawValue -= offset; // Apply offset if needed

  float calibrationFactor = actualValue / (float)rawValue;
  preferences.adcCalib[channel] = calibrationFactor;
  savePreferences();
  
  server.send(200, "text/plain", "Calibration Radio done");
}

int32_t getBatterycentiVolts() {
  return (int32_t)((float)batteryData.voltage() * preferences.adcCalib[0] * 100.); // Return in 0.01V
}

int32_t getBatteryCurrent() {
  int32_t rawCurrent = batteryData.current() - batteryData.currentOffset;
  int32_t current =  (float)rawCurrent * preferences.adcCalib[1]; // Return in A*100
  return current > 10 ? current : 0; // Clamp to 0 if negative
}

int32_t getBatteryCurrentSensormV() {
  return batteryData.current();
}

#endif // ESP32C3

void batterySensInit() {
#ifdef CONFIG_IDF_TARGET_ESP32C3
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); // 0-3.6V range
#endif

  batt_cells_ibusid  = iBus.addSensor(IBUS_SENSOR_TYPE_CELL, 2);  // Cells voltage in cV (2 bytes)
  batt_current_busid = iBus.addSensor(IBUS_SENSOR_TYPE_BAT_CURR, 2);  // Battery current in A*100 (2 bytes)
  batt_percentage_busid = iBus.addSensor(IBUS_SENSOR_TYPE_FUEL, 2);  // Battery percentage (2 bytes)

  // Set Hall effect sensor offsets to current readings (assuming no current flow during calibration)
  batteryData.currentOffset = analogReadMilliVolts(ADC1_CHANNEL_1);

  identifyBatteryType();
}

void batterySensProcess() {
#ifdef CONFIG_IDF_TARGET_ESP32C3
  adcProcess();
#endif
}

void batterySensCalibrate0(){
  calibrateSensor(0, 10);
}

void batterySensCalibrate1(){
  calibrateSensor(1, 500, batteryData.currentOffset); // Assuming 2A load for current calibration
}

void iBusInit() {
#ifdef iBusUseTimer
  iBus.begin(iBusSerial, 1, iBus_RX, iBus_TX);  // timer 0 des problèmes avec EEPROM, wifi etc ... timer 1 idem ...
#else
  iBus.begin(iBusSerial, IBUSBM_NOTIMER, iBus_RX, iBus_TX);
#endif
  gps_lat = iBus.addSensor(IBUS_SENSOR_TYPE_GPS_LAT, 4);
  gps_lon = iBus.addSensor(IBUS_SENSOR_TYPE_GPS_LON, 4);
  gps_alt = iBus.addSensor(IBUS_SENSOR_TYPE_GPS_ALT, 4);
  gps_head = iBus.addSensor(IBUS_SENSOR_TYPE_CMP_HEAD, 2);
  gps_speed = iBus.addSensor(IBUS_SENSOR_TYPE_GROUND_SPEED, 2);
}

void iBusSetValue(TinyGPSPlus gps) {
  iBus.setSensorMeasurement(gps_lat, gps.location.lat() * 1E7);  //4bytes signed WGS84 in degrees * 1E7
  iBus.setSensorMeasurement(gps_lon, gps.location.lng() * 1E7);  // 4bytes signed WGS84 in degrees * 1E7
  iBus.setSensorMeasurement(gps_alt, gps.altitude.value());      // en cm  . 4bytes signed!!! GPS alt m*100.
  iBus.setSensorMeasurement(gps_head, gps.course.deg());         // en d°
  iBus.setSensorMeasurement(gps_speed, gps.speed.mps() * 100);   // en m/s *100

  iBus.setSensorMeasurement(batt_cells_ibusid, getBatterycentiVolts());  // Cells voltage in cV (2 bytes)
  iBus.setSensorMeasurement(batt_current_busid, getBatteryCurrent());  // Battery current in A*100 (2 bytes)
  iBus.setSensorMeasurement(batt_percentage_busid, calcBatteryPercentage());  // Battery percentage (2 bytes)
}

void iBusRestart() {
  iBusSerial.begin(115200, SERIAL_8N1, iBus_RX, iBus_TX);
}

void iBusStop() {
  iBusSerial.end();
}

void iBusLoop() {
  iBus.loop();
}

#endif  // #if defined(FEATURE_IBUS)
