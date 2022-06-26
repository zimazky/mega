//
//    FILE: zone.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class zone definition
//

#ifndef zone_h
#define zone_h

#include <Arduino.h>
#include "dht.h"
#include "button.h"

class zone {
public:
  char    id;                 // Идентификатор
  int16_t target_temperature; // Целевая температура, С
  int16_t temperature;        // Текущая температура, С
  int16_t humidity;           // Влажность, %
  int16_t delta;              // Интервал поддержания температуры (гистерезис)
  bool    mode = 0;           // Режим работы (0 - выключен, 1 - включено поддержание целевой температуры)
  bool    poweron = 0;        // Подача энергии (0 - нет, 1 - включено)
  bool    is_tc = false;      // Признак выхода на заданную температуру
  int8_t  dht_status = 0;     // Состояние датчика температуры
  
  zone(uint8_t dht_pin, char i); // Конструктор зоны без управления
  zone(uint8_t dht_pin, uint8_t led_pin, uint8_t btn_pin, uint8_t pw_pin, uint16_t tc, uint16_t dt, char i);
  void handler();             // Обработчик на каждом цикле
  void handler5s();           // Обработчик каждые 5 сек
  void print(Stream* s);      // Вывод данных в поток
  void writeconf(Stream* s);  // Запись конфигурации в поток.
  void logdiff(Stream* s, uint32_t unixtime, bool f);  // Вывод логов в поток.

private:
  button* _btn = 0; // используем также как определение типа датчика (null - без управления)
  dht _dht;
  uint8_t _dht_pin;
  uint8_t _led_pin;
  uint8_t _pw_pin;
// переменные для вывода разностных логов  
  uint32_t _ut;
  int16_t _t = 0; //предыдущая температура
  int16_t _h = 0; //предыдущая влажность
  int16_t _tc = 0;
  int16_t _dt = 0;
  bool _m = 0;
  bool _p = 0;
  int8_t _s = 0;
};

#endif
//
// END OF FILE
//
