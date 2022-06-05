//
//    FILE: irrigate.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class auto-irrigation definition
//

#ifndef irrigate_h
#define irrigate_h

#include <Arduino.h>
#include "button.h"

class irrigate {
public:  
  
  int8_t  mode = 0;           // Режим работы (0 - выключен, 1 - включен)
  bool    poweron = 0;        // Подача энергии на клапан (0 - нет, 1 - включено)
  
  irrigate( uint8_t _pw_pin ); // Конструктор без кнопки управления
  void handler();             // Обработчик на каждом цикле
  void handler5s(uint32_t unixtime); // Обработчик каждые 5 сек
  void print(Stream* s);      // Вывод данных в поток
  void println(Stream* s);    // Вывод данных в поток с переводом строки
  void writeconf(Stream* s);  // Запись конфигурации в поток
  void logdiff(Stream* s, uint32_t unixtime, bool f);  // Вывод логов в поток
    
private:
  button* _btn = 0; // кнопка запуска системы (null - без кнопочного управления)
  uint8_t _pw_pin;  // pin управления подачей питания к клапану
// переменные для вывода разностных логов  
  uint32_t _ut;
};

#endif
//
// END OF FILE
//
