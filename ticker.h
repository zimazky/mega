#ifndef ticker_h
#define ticker_h

#include <Arduino.h>

class ticker {
public:
  uint32_t _t = 0;
  uint32_t _t0 = 0; // сохраненный предыдущий счетчик
  bool _is_sync = false; 
  //bool _is_overflow = false;
  int8_t _syncf = 0;  // состояние синхронизации в текущем пятисекундном цикле 
                      // (0-синхронизация не требуется, 1-синхронизация прошла успешно, -1 ошибка)
public:
  uint32_t lastsynctime = 0;  // последнее время успешной синхронизации
  int32_t  lastsyncdelta = 0; // знаковое, расхождение в сек во время последней синхронизации
  uint32_t lastsyncinterval = 5000;
  uint32_t increment = 4958;  // 5 секунд, скорректировано с учетом рассинхронизации (42 миллмсекунды за 5 секунд)
  uint32_t unixtime = 0;
  uint32_t starttime = 0;
  uint16_t _lc = 0;           // текущий счетчик циклов в интервале 5 сек
  uint16_t loopcounter = 0;   // счетчик циклов за последний интервал 5 сек
  
  void begin();
  void handler5s( void (*f)() );
  void sync();
  void print(Print* s);     // Вывод данных в поток
};

#endif
//
// END OF FILE
//
