//    FILE: irrigate.cpp
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Библиотека автополива

/*
 *  
 * К одной зоне полива можно привязать несколько программ. Как и в комерческих контроллерах можно ограничить число доступных программ.
 * Для примера сделать 4 программы (A, B, C, D)
 * В программе задается:
 * 1. int8_t Время запуска программы полива в десятиминутках (от 0 до 143), или признак неактивности. Значение 255 означает, что программа выключена.
 * 2. int8_t Селектор дней полива (по дням недели, четные/нечетные, интервальный полив): 
 *      1 M T W t F S s  - дни недели
 *      0 1 0 0 0 0 0 0  - четные
 *      0 1 0 0 0 0 0 1  - нечетные
 *      0 1 X X X X X X  - (резерв) XXXXXX больше 1
 *      0 0 X X X Y Y Y  - (резерв) интервал XXX дней через YYY дней (макс интервалы XXX и YYY - 7 дней)
 * 3. int8_t Время продолжительности полива в минутах (от 1 до 255 минут или 4,25 часа)
 * 
 * 
 * 
 * 
 */

#include "irrigate.h"
#include "ticker.h"
#include "datetime.h"
#include "utils.h"

irrigate::irrigate( uint8_t pw_pin ) {
  _pw_pin = pw_pin;
  pinMode(_pw_pin, OUTPUT);
}

void irrigate::handler5s(uint32_t unixtime, bool is_hydrosystem_ready) {

  poweron &= 2; // сбрасывам все биты кроме бита ручного запуска
  
  uint32_t mmd = Mduration * 60;
  //if((unixtime-beginmanual)>mmd) poweron = 0; // сбрасываем ручной полив, если время полива превысило ограничение
  
  uint32_t localtime = time(unixtime);
  uint32_t starttime = Astart;
  starttime *= 600; // перевод десятиминуток в секунды
  uint32_t endtime = starttime + Aduration*60; // перевод минут в секунды
  if((localtime>=starttime) && (localtime<endtime)) {
    // время соответствует расписанию
    if(Adays & 128) { // По дням недели
      int8_t wdmask = 1 << weekday(unixtime);
      if(Adays & wdmask) poweron |= 4;
    }
    else {
      int8_t md = monthday(unixtime);
      if((Adays == 65) && (md & 1)) poweron |= 4; // нечетное число
      if((Adays == 64) && (md ^ 1)) poweron |= 4; // четное число
    }
  }

  if(is_hydrosystem_ready && poweron>0) poweron |= 1; // устанавливаем бит открывания клапана
  Serial.print("ir_unixtime="); Serial.println(unixtime);
  Serial.print("ir_localtime="); Serial.println(localtime);
  Serial.print("ir_starttime="); Serial.println(starttime);
  Serial.print("ir_endtime="); Serial.println(endtime);
  Serial.print("ir_poweron="); Serial.println(poweron);
  digitalWrite(_pw_pin, poweron&1);
}


void irrigate::print(Stream* s) {
//    s->print(pressure); s->print(';');             // 0. давление
}

void irrigate::writeconf(Stream* s) {
  return;
}

//-------------------------------------------------------------------------------------------------
// Лог работы автополива:
// Строка представляет собой событие. Событие может выводиться двумя типами записей: 
//   - полная запись (первая запись в файле лога или после перезагрузки);
//   - разностная запись (записывается разность между текущим значением и предыдущим).
// Порядок полей при выводе событий:
// 1. Флаги событий int8_t
//    1 - Активация программ или ручного режима
//    2 - Изменение настроек ручного режима
//    4 - Изменение настроек программы A
//    8 - (Резерв) Изменение настроек программы B
//   16 - (Резерв) Изменение настроек программы C
//   32 - (Резерв) Изменение настроек программы D
//   64 - (Резерв)
//  128 - Признак полной записи (выводятся все поля в виде полного значения)
// 2. Метка времени. Тип unixtime, выводится разница с предыдущим значением в потоке.
// Далее в соответствии с установленными битами флагов событий выводятся параметры:
// 3. Активация программ или ручного режима
// 4. Заданные значения параметров ручного режима (Mduration)
// 5. Заданные значения параметров программы A (Astart, Aduration, Adays)
//-------------------------------------------------------------------------------------------------

void irrigate::logdiff(Stream* s, uint32_t unixtime, bool f) {
  
  uint8_t b = 0;
  // полная запись
  if(f) { _ut = 0; _p = 0; _md = 0; _as = 0; _ad = 0; _ads = 0; b = 128; }
  
  // блок определения байта флагов
  if( poweron != _p ) b += 1;
  if( Mduration != _md ) b += 2;
  if( (Astart!=_as) || (Aduration!=_ad) || (Adays!=_ads) ) b += 4;

  // блок вывода
  if( b ) {
    // флаги
    s->print(b);
    // unixtime
    print_with_semicolon(s, unixtime-_ut); _ut = unixtime;
    // вкючение программы или ручного режима
    if( b & 1) { print_with_semicolon(s, poweron); _p = poweron; }
    // изменение параметров ручного режима
    if( b & 2) { print_with_semicolon(s, Mduration); _md = Mduration; }
    // изменение параметров программы A
    if( b & 4) { 
      print_with_semicolon(s, Astart); 
      print_with_semicolon(s, Aduration); 
      print_with_semicolon(s, Adays); 
      _as = Astart; _ad = Aduration; _ads = Adays; 
    }
    // конец строки
    s->println();
  }
}

//
// END OF FILE
//

