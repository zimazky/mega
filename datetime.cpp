#include "datetime.h"

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
