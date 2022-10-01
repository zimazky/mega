//
//    FILE: irrigate.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class auto-irrigation definition
//

#ifndef irrigate_h
#define irrigate_h

#include <Arduino.h>

union program {
  program(uint16_t A);
  program(uint8_t A, uint8_t B);
  uint16_t all;
  uint8_t a[2];
};

class irrigate {
public:  
  // Параметры ручного запуска
  uint8_t Mduration = 60;     // Продолжительность полива в минутах (от 1 до 255 минут)
  uint32_t beginmanual = 0;   // Время включения ручного полива unixtime

  // Для начала будем использовать две автоматические программы A и B. В дальнейшем можно добавить программы C, D
  // Параметры программы A,B
  program start = program(48,255);  // Время начала запуска в десятиминутках (0-143, 255 - выключен) (48- 8ч)
  program duration = program(60,60);// Продолжительность полива в минутах (от 1 до 255 минут)
  program days = program(255,255);  // Селектор дней полива:
                              //   >127 - маска дней недели
                              //    128 - программа выключена (все дни недели отключены)
                              //    255 - ежедневно (все дни недели включены)
                              // 66-127 - не используется
                              //     64 - четные дни
                              //     65 - нечетные дни
                              //    <64 - (резерв) интервальный режим
  
  uint8_t poweron = 0;        // Признаки срабатывания программы и/или запуска полива 
                              // (00DCBAMP - маска по программам A, B, C, D или запущен полив вручную M, P - бит подачи сигнала на откытие клапана)
  
  irrigate( uint8_t _pw_pin ); // Конструктор без кнопки управления
  void handler5s(uint32_t unixtime, bool is_hydrosystem_ready); // Обработчик каждые 5 сек
  void print(Stream* s);      // Вывод данных в поток
  void writeconf(Stream* s);  // Запись конфигурации в поток
  void logdiff(Stream* s, uint32_t unixtime, bool f);  // Вывод логов в поток
    
private:
  uint8_t _pw_pin;  // pin управления подачей питания к клапану
// переменные для вывода разностных логов  
  uint32_t _ut;
  uint8_t _p = 0;
  uint8_t _md = 0;
  program _as = program(0);
  program _ad = program(0);
  program _ads = program(0);
};

#endif
//
// END OF FILE
//
