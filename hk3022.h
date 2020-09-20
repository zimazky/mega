//
//    FILE: hk3022.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class HK3022 definition
//

#ifndef hk3022_h
#define hk3022_h

#include <Arduino.h>

class hk3022 {
public:  
  int16_t pressure;        // Текущее давление в системе, бар*1000
  
  hk3022( uint8_t pin );
  void read();                // Чтение датчика
  void print(Stream* s);      // Вывод данных в поток
  void println(Stream* s);    // Вывод данных в поток
  void logdiff(Stream* s, uint32_t unixtime, bool f);  // Вывод логов в поток.
    
private:
  uint8_t _pin;
// переменные для вывода разностных логов  
  uint32_t _ut;
  int16_t _p = 0;
};

#endif
//
// END OF FILE
//
