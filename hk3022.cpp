//    FILE: hk3022.cpp
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Библиотека для датчика слежения за давлением в системе водоснабжения

/*
 * 
 *  VCC: Red (Power+)                            Orange        5V
 *  OUT: Yellow/White (Signal output)            Green         A2
 *  GND: Black (Power-)                          Brown         GND
 * 
 * 
 * 
 * 
 */

#include "hk3022.h"

hk3022::hk3022( uint8_t pin ) {
  _pin = pin;
  pinMode( _pin, INPUT );
}

void hk3022::read() {
  int16_t raw = analogRead(_pin);
  
  // 0 бар соответствует значению 0,5 В или 102,3
  // 5 бар соответствует значению 4,5 В или 920,7
  // разница 4,5-0,5=4 В соответствует 818,4
  
  //pressure = (raw*10-1023)*5000/8184; // приводим к значению 5000 для 5 бар
  int32_t t = raw*10-1023;
  t *= 5000; t /= 8184;
  pressure = t; // приводим к значению 5000 для 5 бар
}

void hk3022::print(Stream* s) {
    s->print(pressure); s->print(';');             // 0. давление
}

void hk3022::println(Stream* s) {
    print(s);
    s->println();
}

//-------------------------------------------------------------------------------------------------
// Лог давления в системе водоснабжения:
// Строка представляет собой событие. Событие может выводиться двумя типами записей: 
//   - полная запись (первая запись в файле лога или после перезагрузки);
//   - разностная запись (записывается разность между текущим значением и предыдущим).
// Порядок полей при выводе событий:
// 1. Флаги событий int8_t
//    1 - Изменение фактического давления
//    2 - (Резерв) Заданный предел отключения насоса
//    4 - (Резерв) Заданный предел сухого хода
//  128 - Признак полной записи (выводятся все поля в виде полного значения)
// 2. Метка времени. Тип unixtime, выводится разница с предыдущим значением в потоке.
// Далее в соответствии с установленными битами флагов событий выводятся параметры:
// 3. Фактическое давление. Тип int, выводится разница с предыдущим значением в потоке.
//-------------------------------------------------------------------------------------------------

void hk3022::logdiff(Stream* s, uint32_t unixtime, bool f) {
  
  uint8_t b = 0;
  // полная запись
  if(f) { _ut = 0; _p = 0; b = 128; }
  
  // блок определения байта флагов
  if( pressure != _p ) b += 1;

  // блок вывода
  if( b ) {
    // флаги
    s->print(b);
    // unixtime
    s->print(';'); s->print( unixtime - _ut); _ut = unixtime;
    // давление
    if( b & 1) { s->print(';'); s->print( pressure - _p ); _p = pressure; }
    // конец строки
    s->println();
  }
}

//
// END OF FILE
//
