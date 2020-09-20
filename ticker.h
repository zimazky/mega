#ifndef ticker_h
#define ticker_h

#include <Arduino.h>

char* yyyymmdd(char* datestring, uint32_t unixtime);
char* hhmmss(char* datestring, uint32_t unixtime);
char* hex(char* datestring, uint32_t unixtime);

class ticker {
public:
  uint32_t _t = 0;
  uint32_t _t5s = 0;
  bool _is_sync = false; 
  bool _is_overflow = false;
  int8_t _syncf = 0;  // состояние синхронизации в текущем пятисекундном цикле 
                      // (0-синхронизация не требуется, 1-синхронизация прошла успешно, -1 ошибка)
public:
  uint32_t lastsynctime = 0;
  int32_t  lastsyncdelta = 0; // знаковый
  uint32_t lastsyncinterval = 5000;
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
