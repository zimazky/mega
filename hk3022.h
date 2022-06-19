//
//    FILE: hk3022.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class HK3022 definition
//

#ifndef hk3022_h
#define hk3022_h

#include <Arduino.h>
#include "button.h"

class hk3022 {
public:  
  int16_t pressure;             // Напряжение с датчика, 1023 соответствует 5В
  
  int8_t mode = 1;              // Режим работы (0 - выключен, 1 - включено слежение до запуска насоса, 2 - включено слежение после запуска насоса, 4 - сухой ход)
  bool poweron = 0;             // Подача энергии на насос (0 - нет, 1 - включено)
  uint32_t begindrymode = 0;    // Время включения состояния сухого хода unixtime
  uint32_t beginpumpon = 0;     // Время включения насоса unixtime

  uint16_t hilimit = 593;        // Верхний предел отключения насоса (верхнее давление системы). По умолчанию 3,0 бар
  uint16_t lolimit = 380;        // Нижний предел включения насоса (нижнее давление системы). По умолчанию 1,7 бар
  uint16_t drylimit = 300;       // Предел отключения насоса по сухому ходу. По умолчанию 1,2 бар
  
  uint16_t pumpinittime = 40;    // Время инициализации насоса в секундах (для достижения давления выше предела сухого хода). По умолчанию 40 секунд
  uint16_t pumprunlimit = 300;   // Предел времени непрерывной работы насоса в секундах (исключение длительной работы на сухом ходу). По умолчанию 5 минут
  uint16_t retryinterval = 1200; // Интервал времени для повторения запуска насоса в секундах (при сухом колодце). По умолчанию 20 минут 
  
  hk3022(uint8_t pin, uint8_t pw_pin); // Конструктор без кнопки управления
  void handler(uint32_t unixtime);     // Обработчик на каждом цикле
  int16_t getpressure(uint32_t unixtime); // Функция давления для тестирования
  void handler5s(uint32_t unixtime); // Обработчик каждые 5 сек
  void print(Stream* s);      // Вывод данных в поток
  void println(Stream* s);    // Вывод данных в поток с переводом строки
  void writeconf(Stream* s);  // Запись конфигурации в поток
  void logdiff(Stream* s, uint32_t unixtime, bool f);  // Вывод логов в поток
    
private:
  uint8_t _pin;
  button* _btn = 0; // кнопка запуска системы (null - без кнопочного управления)
  uint8_t _pw_pin;  // pin управления подачей питания к насосу
// переменные для вывода разностных логов  
  uint32_t _ut;     // предыдущее значение времени
  int16_t _p = 0;   // предыдущее значение давления
  bool _pw = 0;     // предыдущее значение подачи мощности
  int8_t _m = 0;    // предыдущее значение режима работы
  uint16_t _hl = 0;
  uint16_t _ll = 0;
  uint16_t _dl = 0;
  uint16_t _pit = 0;
  uint16_t _prl = 0;
  uint16_t _ri = 0;
};

#endif
//
// END OF FILE
//
