#include "ticker.h"
#include <ethudp.h>

uint32_t ntpUnixTime (EthernetUDP &udp);

#define timeServer {192,168,2,1}  
//#define timeServer {88,147,254,230}  //ntp4.stratum2.ru

EthernetUDP udp;

const uint8_t timezone = 3; // UTC+3

// Функция определения времени
uint32_t time(uint32_t unixtime) {
  uint32_t localtime = unixtime + 3600*timezone;
  localtime %= 86400;
  return localtime;
}
// Функция определения дня недели
int8_t weekday(uint32_t unixtime) {
  unixtime += 3600*timezone;
  uint32_t days = unixtime/86400;
  int8_t wd = 4 + days%7;
}
// Функция определения дня месяца
int8_t monthday(uint32_t unixtime) {
  unixtime += 3600*timezone;              // приведение к местному времени
  uint32_t days = unixtime/86400;         // количество дней с 01.01.1970 (24*60*60=86400)
  uint32_t years = (days*4+2)/1461;       // количество лет с 01.01.1970 ( (365*4+1)=1461 количество лет в 4-х годах ). Не учитываются переходы столетий
  uint16_t year = 1970+years;             // год
  uint32_t yday = days-(years*1461+1)/4;  // день в году
  bool leap = !(year&3);  // високосный год тот который делится на 4 (это Юлианский принцип, он работает с 1901 по 2099 годы)
                          // для Григорианского календаря требуется проверять делится ли год на 100 и на 400. 
                          // 1700,1800,1900,2100,2200,2300 - не високосные т.к. не делятся на 400, 2000 - високосный, т.к делится и на 100, и на 400)
  if (yday > 58 + leap) yday += 2 - leap;         // приводим к системе с 30-дневным февралем
  uint16_t month = ((yday * 12) + 6)/367;         // определение месяца исходя из количества дней с начала года
  uint16_t mday = yday - ((month * 367) + 5)/12;  // определение дня в месяце

  return ++mday;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция конвертирования unixtime в обычный формат даты YYYY-MM-DD (работает до 28.02.2100)
// unixtime содержит число секунд прошедших с 01.01.1970 00:00:00GMT
// функция работает на интервале времени от 01.01.1970 (unixtime=0x00000000) до 19.01.2038 03:14:07GMT (unixtime=0x7FFFFFFF)
// если расширить определение unixtime как беззнакового целого, то теоретически можно определять время до 07.02.2106 06:28:15GMT (unixtime=0xFFFFFFFF)
// алгоритм не учитывает правила високосных лет Григорианского календаря при переходе через столетия, поэтому корректно функция будет работать до 28.02.2100
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* yyyymmdd(char* datestring, uint32_t unixtime) {
  unixtime += 3600*timezone;              // приведение к местному времени
  uint32_t days = unixtime/86400;         // количество дней с 01.01.1970 (24*60*60=86400)
  uint32_t years = (days*4+2)/1461;       // количество лет с 01.01.1970 ( (365*4+1)=1461 количество лет в 4-х годах ). Не учитываются переходы столетий
  uint16_t year = 1970+years;             // год
  uint32_t yday = days-(years*1461+1)/4;  // день в году
  bool leap = !(year&3);  // високосный год тот который делится на 4 (это Юлианский принцип, он работает с 1901 по 2099 годы)
                          // для Григорианского календаря требуется проверять делится ли год на 100 и на 400. 
                          // 1700,1800,1900,2100,2200,2300 - не високосные т.к. не делятся на 400, 2000 - високосный, т.к делится и на 100, и на 400)
  if (yday > 58 + leap) yday += 2 - leap;         // приводим к системе с 30-дневным февралем
  uint16_t month = ((yday * 12) + 6)/367;         // определение месяца исходя из количества дней с начала года
  uint16_t mday = yday - ((month * 367) + 5)/12;  // определение дня в месяце

  mday++; month++;
  datestring[8] = 0;
  datestring[7] = '0' + mday%10;
  datestring[6] = '0' + mday/10;
  datestring[5] = '0' + month%10;
  datestring[4] = '0' + month/10;
  datestring[3] = '0' + year%10; year /= 10;
  datestring[2] = '0' + year%10; year /= 10;
  datestring[1] = '0' + year%10; year /= 10;
  datestring[0] = '0' + year%10;
  
  return datestring;  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция конвертирования unixtime в обычный формат времени HH:MM:SS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* hhmmss(char* datestring, uint32_t unixtime) {
  unixtime += 3600*timezone;      // приведение к местному времени
  uint32_t secs = unixtime%86400; // количество секунд с начала дня
  uint32_t ss = secs%60;
  uint32_t mins = secs/60;        // количество минут с начала дня
  uint16_t mm = mins%60;
  uint16_t hh = mins/60;
  
  datestring[8] = 0;
  datestring[7] = '0' + ss%10;
  datestring[6] = '0' + ss/10;
  datestring[5] = ':';
  datestring[4] = '0' + mm%10;
  datestring[3] = '0' + mm/10;
  datestring[2] = ':';
  datestring[1] = '0' + hh%10;
  datestring[0] = '0' + hh/10;
  
  return datestring;  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Функция конвертирования unixtime в шестнадцатеричную строку
// Не использовать, проигрываем в размере программы
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char* hex(char* datestring, uint32_t unixtime) {
  datestring[10] = 0;
  uint8_t b;
  for(uint16_t i = 9; i>1; i--) {
    b = '0' + (unixtime & 0xF);
    if( b > '9' ) b += 7;
    datestring[i] = b;
    unixtime >>= 4;
  }
  datestring[1] = 'x';
  datestring[0] = '0';
  return datestring;  
}

void ticker::begin() {
  _t = millis();
  _t0 = _t;
  sync();         // попытка синхронизировать часы в самом начале
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Обработчик вызывает функцию f() каждые 5 секунд
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ticker::handler5s( void (*f)() ) { 
  _t = millis();
  //if(_t < _t0) _is_overflow = true;
  uint32_t dt = _t - _t0;
  uint32_t d = dt/increment;
  uint32_t m = dt%increment;
  if(d < 1) return;
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
    s->print(lastsyncinterval); s->print(';');  // 4. последний интервал синхронизации
    s->print(_t); s->print(';');                // 5. счетчик миллисекунд контроллера
}

//
// END OF FILE
//
