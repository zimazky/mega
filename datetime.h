//
//    FILE: datetime.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Date and Time utils
//

#ifndef datetime_h
#define datetime_h

#include <Arduino.h>

char* yyyymmdd(char* datestring, uint32_t unixtime);
char* hhmmss(char* datestring, uint32_t unixtime);
char* hex(char* datestring, uint32_t unixtime);
uint32_t time(uint32_t unixtime);
int8_t weekday(uint32_t unixtime);
int8_t monthday(uint32_t unixtime);

/*
class datetime {
public:
  uint8_t day;
  uint8_t month;
  uint8_t weekday;
  uint16_t year;
  uint32_t time;
  datetime(unixtime);
}
*/


#endif
//
// END OF FILE
//
