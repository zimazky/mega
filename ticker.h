#ifndef ticker_h
#define ticker_h

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
class ticker {
public:
  uint32_t _t = 0;
  uint32_t _t0 = 0; // сохраненный предыдущий счетчик
  bool _is_sync = false; 
  bool _is_overflow = false;
  int8_t _syncf = 0;  // состояние синхронизации в текущем пятисекундном цикле 
                      // (0-синхронизация не требуется, 1-синхронизация прошла успешно, -1 ошибка)
public:
  uint32_t lastsynctime = 0;
  int32_t  lastsyncdelta = 0; // знаковый
  uint32_t lastsyncinterval = 5000;
  uint32_t increment = 4958; // 5 секунд, скорректировано с учетом рассинхронизации (42 миллмсекунды за 5 секунд)
  uint32_t unixtime = 0;
  uint32_t starttime = 0;
  
  void begin();
  void handler5s( void (*f)() );
  void sync();
  void print(Print* s);     // Вывод данных в поток
};

#endif
//
// END OF FILE
//
