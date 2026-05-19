// Code capteurs dans https://github.com/opentx/opentx/blob/2.3/radio/src/telemetry/flysky_ibus.cpp
// et https://github.com/betaflight/betaflight/blob/master/src/main/telemetry/ibus_shared.h

#pragma once

#define    IBUS_SENSOR_TYPE_NONE              0x00
#define    IBUS_SENSOR_TYPE_TEMPERATURE       0x01 // Temperature (in 0.1 degrees, where 0=-40'C)
#define    IBUS_SENSOR_TYPE_RPM_FLYSKY        0x02 // RPM
#define    IBUS_SENSOR_TYPE_EXTERNAL_VOLTAGE  0x03 // External voltage (in 0.01)
#define    IBUS_SENSOR_TYPE_CELL              0x04 // Avg Cell voltage
#define    IBUS_SENSOR_TYPE_BAT_CURR          0x05 // battery current A * 100
#define    IBUS_SENSOR_TYPE_FUEL              0x06 // remaining battery percentage / mah drawn otherwise or fuel level no unit!
#define     IBUS_SENSOR_TYPE_RPM              0x07 // throttle value / battery capacity
#define     IBUS_SENSOR_TYPE_CMP_HEAD         0x08 //Heading  0..360 deg, 0=north 2bytes
#define     IBUS_SENSOR_TYPE_CLIMB_RATE       0x09 //2 bytes m/s *100
#define     IBUS_SENSOR_TYPE_COG              0x0a //2 bytes  Course over ground(NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. unknown max uint
#define     IBUS_SENSOR_TYPE_GPS_STATUS       0x0b //2 bytes
#define     IBUS_SENSOR_TYPE_ACC_X            0x0c //2 bytes m/s *100 signed
#define     IBUS_SENSOR_TYPE_ACC_Y            0x0d //2 bytes m/s *100 signed
#define     IBUS_SENSOR_TYPE_ACC_Z            0x0e //2 bytes m/s *100 signed
#define     IBUS_SENSOR_TYPE_ROLL             0x0f //2 bytes deg *100 signed
#define     IBUS_SENSOR_TYPE_PITCH            0x10 //2 bytes deg *100 signed
#define     IBUS_SENSOR_TYPE_YAW              0x11 //2 bytes deg *100 signed
#define     IBUS_SENSOR_TYPE_VERTICAL_SPEED   0x12 //2 bytes m/s *100
#define     IBUS_SENSOR_TYPE_GROUND_SPEED     0x13 //2 bytes m/s *100 different unit than build-in sensor
#define     IBUS_SENSOR_TYPE_GPS_DIST         0x14 //2 bytes dist from home m unsigned
#define     IBUS_SENSOR_TYPE_ARMED            0x15 //2 bytes
#define     IBUS_SENSOR_TYPE_FLIGHT_MODE      0x16 //2 bytes
#define     IBUS_SENSOR_TYPE_PRES             0x41 // Pressure (Pa)
#define     IBUS_SENSOR_TYPE_ODO1             0x7c // Odometer1
#define     IBUS_SENSOR_TYPE_ODO2             0x7d // Odometer2
#define     IBUS_SENSOR_TYPE_SPE              0x7e // Speed 2bytes km/h

#define     IBUS_SENSOR_TYPE_GPS_LAT          0x80 //4bytes signed WGS84 in degrees * 1E7
#define     IBUS_SENSOR_TYPE_GPS_LON          0x81 //4bytes signed WGS84 in degrees * 1E7
#define     IBUS_SENSOR_TYPE_GPS_ALT          0x82 //4bytes signed!!! GPS alt m*100
#define     IBUS_SENSOR_TYPE_ALT              0x83 //4bytes signed!!! Alt m*100
#define     IBUS_SENSOR_TYPE_ALT_MAX          0x84 //4bytes signed MaxAlt m*100

#define     IBUS_SENSOR_TYPE_ALT_FLYSKY       0xf9 // Altitude 2 bytes signed in m

#include <TinyGPS++.h>
void iBusInit();
void iBusSetValue(TinyGPSPlus gps);
void iBusLoop();
void iBusRestart();
void iBusStop();

void batterySensInit();
void batterySensProcess();

void batterySensCalibrate0();
void batterySensCalibrate1();

int32_t getBatterycentiVolts();
int32_t getBatteryCurrent();
int32_t getBatteryCurrentSensormV();