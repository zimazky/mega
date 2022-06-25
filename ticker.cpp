#include "ticker.h"
#include <ethudp.h>

uint32_t ntpUnixTime (EthernetUDP &udp);

#define timeServer {192,168,2,1}  
//#define timeServer {88,147,254,230}  //ntp4.stratum2.ru

EthernetUDP udp;

void ticker::begin() {
  _t = millis();
  _t0 = _t;
  sync();         // попытка синхронизировать часы в самом начале
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Обработчик вызывает функцию f() каждые 5 секунд
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ticker::handler5s( void (*f)() ) {
  _lc++; 
  _t = millis();
  //if(_t < _t0) _is_overflow = true;
  uint32_t dt = _t - _t0;
  uint32_t d = dt/increment;
  uint32_t m = dt%increment;
  if(d < 1) return;
  loopcounter = _lc;
  _lc = 0;
  unixtime += d*5;
  _t0 = _t - m;
  f();            // вызов функции
  _syncf = 0;     // сбрасываем состояние синхронизации после функции-обработчика, чтобы в функции можно было его использовать
  // синхронизируем часы если они еще не синхронизированы или если после последней синхронизации прошло значительное время
  if( !_is_sync || ((unixtime-lastsynctime)>5000)) { sync(); }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Синхронизация даты и времени с сервером NTP
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ticker::sync() {
  Serial.print("Sync ");
//  Serial.print(timeServer);
//  uint32_t _start = millis();
  uint32_t _ut = 0;
  _ut = ntpUnixTime(udp);
//  uint32_t _stop = millis();
  if(_ut) { 
    _syncf = 1;
    //////////////////////////////////////////////////////////////////////////////
    // Синхронизация прошла успешно
    //////////////////////////////////////////////////////////////////////////////
    if( _is_sync ) {    //уже синхронизировались раньше
      lastsyncdelta = _ut - unixtime;             // расхождение времени
      lastsyncinterval = _ut - lastsynctime;      // на интервале
      //int32_t sync_correction = lastsyncdelta*5000/lastsyncinterval; // необходимая коррекция на интервале 5 сек
      //increment -= sync_correction;

    }
    else {              // синхронизируемся первый раз
      starttime = _ut - unixtime;                 // определяем время старта
      _is_sync = true;                            // устанавливаем признак первичной синхронизации
    }
    Serial.print(" u:"); Serial.print(unixtime); 
    Serial.print(" ur:"); Serial.print(_ut); 
    unixtime = _ut;
    lastsynctime = unixtime;
    Serial.print(" d:"); Serial.print(lastsyncdelta); 
    Serial.print(" si:"); Serial.print(lastsyncinterval); 
    Serial.println(" OK"); 
  }
  else {
    //////////////////////////////////////////////////////////////////////////////
    // Синхронизация не выполнена из-за ошибки
    //////////////////////////////////////////////////////////////////////////////
    Serial.println(" error"); 
    _syncf = -1;
  }
//  Serial.print(_stop-_start);
//  Serial.println(unixtime);
}

/*
 * © Francesco Potortì 2013 - GPLv3 - Revision: 1.13
 *
 * Send an NTP packet and wait for the response, return the Unix time
 *
 * To lower the memory footprint, no buffers are allocated for sending
 * and receiving the NTP packets.  Four bytes of memory are allocated
 * for transmision, the rest is random garbage collected from the data
 * memory segment, and the received packet is read one byte at a time.
 * The Unix time is returned, that is, seconds from 1970-01-01T00:00.
 */

uint32_t ntpUnixTime (EthernetUDP &udp)
{
  static int udpInited = udp.begin(123); // open socket on arbitrary port

//  const char timeServer[] = "pool.ntp.org";  // NTP server

  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header

  // Fail if WiFiUdp.begin() could not init a socket
  if (! udpInited) 
    return 0;

  // Clear received data from possible stray received packets
  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(timeServer, 123) // 123 is the NTP port
   && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
   && udp.endPacket()))
    return 0;       // sending request failed

  // Wait for response; check every pollIntv ms up to maxPoll times
  delay(150);
  if (udp.parsePacket() != 48)
    return 0;       // no correct packet received

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read();  // NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Discard the rest of the packet
  udp.flush();

  return time - 2208988800ul;   // convert NTP time to Unix time
}

void ticker::print(Print* s) {
    s->print(unixtime); s->print(';');          // 0. текущее время
    s->print(starttime); s->print(';');         // 1. время запуска
    s->print(lastsynctime); s->print(';');      // 2. время последней синхронизации
    s->print(lastsyncdelta); s->print(';');     // 3. ошибка последней синхронизации
//    s->print(lastsyncinterval); s->print(';');  // 4. последний интервал синхронизации
    s->print(_t0); s->print(';');               // 4. счетчик миллисекунд контроллера
    s->print(loopcounter); s->print(';');       // 5. счетчик циклов выполнения за 5 сек

}

//
// END OF FILE
//
