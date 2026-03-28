/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
float distanceSimple(float lat1, float lon1, float lat2, float lon2);
void setSerialGpsSpeed(uint32_t baud);
void setGpsRate(uint32_t rate);
void limitNmeaSentences();
void fs_initGPS(uint32_t baud, uint32_t rate) ;
void setGpsSpeed(uint32_t baud);
void addChecksum(char *buff);
void sendPacket(byte * packet, byte len);
void crc_ublox( byte *msg, uint32_t size);
void echoGPS(int duree) ;
