//
//    FILE: irrigate.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class auto-irrigation definition
//

#ifndef irrigate_h
#define irrigate_h

#include <Arduino.h>

class irrigate {
public:  
  // Для начала будем использовать одну программу A. В дальнейшем можно добавить программы B, C, D
  // Параметры программы A
  uint8_t Astart = 255;       // Время начала запуска в десятиминутках (1-143, 255 - выключен)
  uint8_t Aduration = 60;     // Продолжительность полива в минутах (от 1 до 255 минут)
  uint8_t Adays = 255;        // Селектор дней полива:
                              //   >127 - маска дней недели
                              //    128 - программа выключена (все дни недели отключены)
                              //    255 - ежедневно (все дни недели включены)
                              // 66-127 - не используется
                              //     64 - четные дни
                              //     65 - нечетные дни
                              //    <64 - (резерв) интервальный режим
  
  uint8_t poweron = 0;        // Признаки срабатывания программы (0000DCBA - маска по программам A, B, C, D)
  
  irrigate( uint8_t _pw_pin ); // Конструктор без кнопки управления
  void handler5s(uint32_t unixtime, int8_t is_hydrosystem_ready); // Обработчик каждые 5 сек
  void print(Stream* s);      // Вывод данных в поток
  void println(Stream* s);    // Вывод данных в поток с переводом строки
  void writeconf(Stream* s);  // Запись конфигурации в поток
  void logdiff(Stream* s, uint32_t unixtime, bool f);  // Вывод логов в поток
    
private:
  uint8_t _pw_pin;  // pin управления подачей питания к клапану
// переменные для вывода разностных логов  
  uint32_t _ut;
  int16_t _p = 0;
};

#endif
//
// END OF FILE
//
