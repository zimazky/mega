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
 *  Pump Power                                   Blue/White    A3
 * 
 * 
 */

#include "hk3022.h"
#include "utils.h"

hk3022::hk3022( uint8_t pin, uint8_t pw_pin ) {
  _pin = pin;
  _pw_pin = pw_pin;
  pinMode(_pin, INPUT);
  pinMode(_pw_pin, OUTPUT);
}

void hk3022::handler(uint32_t unixtime) {
  //if( _btn == 0 ) return; // выходим, если нет кнопки управления
  // проверяем кнопку очень часто
  //if( _btn->state() && mode==0 ) mode = 1;
  //else mode = 0;
  //digitalWrite(_led_pin, mode); для варианта с светодиодом индикации
  
  int16_t p1 = analogRead(_pin); delay(5);
  p1 += analogRead(_pin); delay(5);
  p1 += analogRead(_pin);

  //int16_t m = p1%3;
  //pressure = p1/3;
  pressure = (p1 + _pre)/6;
  //if(m >= 2)  pressure += 1;
  _pre = p1;

  poweron = false;
  switch(mode) {
    case 0: // Состояние слежения выключено
      poweron = false;
      break;
    case 1: // Состояние слежения до запуска насоса
      if( pressure < lolimit ) { poweron = true; mode = 2; beginpumpon = unixtime; } // давление упало ниже нижней границы, запуск насоса
      else poweron = false; // проверяем, что мощность не подается
      break;
    case 2: // Состояние слежения после запуска насоса
      if( pressure >= hilimit ) { poweron = false; mode = 1; } // давление достигло заданного значения, остановка насоса
      else { // давление не достигло заданного значения
        int16_t runtime = unixtime - beginpumpon;
        if( (runtime > pumprunlimit) || ((runtime > pumpinittime) && (pressure < drylimit)) ) { 
          // останавливаем насос из-за превышения допустимого времени работы или недостаточного давления, переход в режим сухого хода
          poweron = false; mode = 4; begindrymode = unixtime;
        }
        else poweron = true;  // давление не достигло заданного значения, проверяем, что мощность подается
      }
      break;
    case 4: // Состояние сухого хода
      if(unixtime-begindrymode >= retryinterval) { poweron = false; mode = 1; } // Подошло время повторной попытки заполнить систему (колодец мог заполнится водой)
    default: 
      poweron = false;
  }
  digitalWrite(_pw_pin, poweron);

}

/*
// Функция давления для тестирования
int16_t hk3022::getpressure(uint32_t unixtime) {
  if(beginpumpon == 0) return 0;
  int16_t p = hilimit*(unixtime-beginpumpon)/60;
  if(p>600) p=600;
  if(unixtime-beginpumpon>100) p=0;
  return p;
}
*/

void hk3022::handler5s(uint32_t unixtime) {
  Serial.print("p="); Serial.println(pressure);
  Serial.print("mode="); Serial.println(mode);
  Serial.print("poweron="); Serial.println(poweron);
}

/*
void hk3022::read() {
  pressure = analogRead(_pin);

// Ниже фрагмент для варианта с пересчетом значения датчика в бары
// 0 бар соответствует значению 0,5 В или 102,3
// 5 бар соответствует значению 4,5 В или 920,7
// разница 4,5-0,5=4 В соответствует 818,4
  
  //int16_t raw = analogRead(_pin);
  //int32_t t = raw*10-1023;
  //t *= 5000; t /= 8184;
  //pressure = t; // приводим к значению 5000 для 5 бар
}
*/

void hk3022::print(Stream* s) {
  s->print(";H;0");                          // 0. Тип зоны контроллера
                                             // 1. Идентификатор зоны
  print_with_semicolon(s, pressure);         // 2. Давление
  print_with_semicolon(s, mode);             // 3. Режим
  print_with_semicolon(s, hilimit);          // 4. Верхняя граница
  print_with_semicolon(s, lolimit);          // 5. Нижняя граница
  print_with_semicolon(s, drylimit);         // 6. Граница сухого хода
  print_with_semicolon(s, pumpinittime);     // 7. Время инициализации насоса в секундах (для достижения давления выше предела сухого хода)
  print_with_semicolon(s, pumprunlimit);     // 8. Предел времени непрерывной работы насоса в секундах (исключение длительной работы на сухом ходу)
  print_with_semicolon(s, retryinterval);    // 9. Интервал времени для повторения запуска насоса в секундах (при сухом колодце)
}

void hk3022::writeconf(Stream* s) {
  s->print("shh");s->println(hilimit);       // Запись верхнего предела давления отключения насоса
  s->print("shl");s->println(lolimit);       // Запись нижнего предела давления включения насоса
  s->print("shd");s->println(drylimit);      // Запись предела давления сухого хода
  s->print("shr");s->println(pumprunlimit);  // Запись предела по времени непрерывной работы насоса
  s->print("shs");s->println(drylimit);      // Запись интервала времени для повторения запуска насоса после перехода в режим "сухой ход"

  s->print("shm");s->println(mode);          // Запись режима
  s->print("shb");s->println(begindrymode);  // Запись времени включения состояния сухого хода
}

//-------------------------------------------------------------------------------------------------
// Лог давления в системе водоснабжения:
// Строка представляет собой событие. Событие может выводиться двумя типами записей: 
//   - полная запись (первая запись в файле лога или после перезагрузки);
//   - разностная запись (записывается разность между текущим значением и предыдущим).
// Порядок полей при выводе событий:
// 1. Флаги событий int8_t
//    1 - Изменение фактического давления
//    2 - Признак передачи данных без преобразования (0В=0, 5В=1023)
//    4 - Режим работы и подача мощности на насос
//        (0 - выключен, 1 - включено слежение до запуска насоса, 2 - включено слежение после запуска насоса, 4 - сухой ход, 8 - бит подачи мощности)
//    8 - Изменились настройки параметров давления (hilimit, lolimit, drylimit)
//   16 - Изменились настройки временных параметров (pumpinittime, pumprunlimit, retryinterval)
//  128 - Признак полной записи (выводятся все поля в виде полного значения)
// 2. Метка времени. Тип unixtime, выводится разница с предыдущим значением в потоке.
// Далее в соответствии с установленными битами флагов событий выводятся параметры:
// 3. Фактическое давление. Тип int16_t, выводится разница с предыдущим значением в потоке.
// 4. Режим работы и подача мощности. Тип int8_t, выводится значение (0 - отключено слежение, 1 - включено слежение до запуска насоса, 2 - слежение после запуска насоса, 3 - сухой ход) += 4 если включен насос.
// 5. Заданные значения параметров давления (hilimit; lolimit; drylimit)
// 6. Заданные значения временных параметров (pumpinittime; pumprunlimit; retryinterval)
//-------------------------------------------------------------------------------------------------
/*
void hk3022::logdiff(Stream* s, uint32_t unixtime, bool f) {
  
  uint8_t b = 0;
  // полная запись
  if(f) { _ut = 0; _p = 0; _pw = 0; _m = 0; _hl = 0; _ll = 0; _dl = 0; _pit = 0; _prl = 0; _ri = 0; b = 128; }
  
  // блок определения байта флагов
  if(pressure != _p) b += 3; // выводим только в виде непреобразованных данных (флаг 1+2)
  if((poweron != _pw) || (mode != _m)) b += 4;
  if((hilimit != _hl) || (lolimit != _ll) || (drylimit != _dl)) b += 8;
  if((pumpinittime != _pit) || (pumprunlimit != _prl) || (retryinterval != _ri)) b += 16;

  // блок вывода
  if( b ) {
    // флаги
    s->print(b);
    // unixtime
    print_with_semicolon(s, unixtime-_ut); _ut = unixtime;
    // давление
    if( b & 1) { print_with_semicolon(s, pressure-_p); _p = pressure; }
    // режим работы и подача мощности
    if( b & 4) { print_with_semicolon(s, mode+(poweron<<3)); _pw = poweron; _m = mode; }
    // значения параметров давления (hilimit; lolimit; drylimit)
    if( b & 8) { 
      print_with_semicolon(s, hilimit);
      print_with_semicolon(s, lolimit);
      print_with_semicolon(s, drylimit);
      _hl = hilimit; _ll = lolimit; _dl = drylimit;
    }
    // значения временных параметров (pumpinittime; pumprunlimit; retryinterval)
    if( b & 16) { 
      print_with_semicolon(s, pumpinittime);
      print_with_semicolon(s, pumprunlimit);
      print_with_semicolon(s, retryinterval);
      _pit = pumpinittime; _prl = pumprunlimit; _ri = retryinterval;
    }
    // конец строки
    s->println();
  }
}
*/

//-------------------------------------------------------------------------------------------------
// Лог давления в системе водоснабжения с выводом в единый лог-файл с новым форматом:
// Строка представляет собой событие. Событие может выводиться двумя типами записей: 
//   - полная запись (первая запись в файле лога или после перезагрузки);
//   - разностная запись (записывается разность между текущим значением и предыдущим).
// Порядок полей при выводе событий:
// 1. Номер зоны ('H' + номер)
// 2. Флаги событий int8_t (в новой версии без разделителя перед флагом)
//    1 - Изменение фактического давления
//    2 - Признак передачи данных без преобразования (0В=0, 5В=1023)
//    4 - Режим работы и подача мощности на насос
//        (0 - выключен, 1 - включено слежение до запуска насоса, 2 - включено слежение после запуска насоса, 4 - сухой ход, 8 - бит подачи мощности)
//    8 - Изменились настройки параметров давления (hilimit, lolimit, drylimit)
//   16 - Изменились настройки временных параметров (pumpinittime, pumprunlimit, retryinterval)
//  128 - Признак полной записи (выводятся все поля в виде полного значения)
// Далее в соответствии с установленными битами флагов событий выводятся параметры:
// 3. Фактическое давление. Тип int16_t, выводится разница с предыдущим значением в потоке.
// 4. Режим работы и подача мощности. Тип int8_t, выводится значение (0 - отключено слежение, 1 - включено слежение до запуска насоса, 2 - слежение после запуска насоса, 3 - сухой ход) += 4 если включен насос.
// 5. Заданные значения параметров давления (hilimit; lolimit; drylimit)
// 6. Заданные значения временных параметров (pumpinittime; pumprunlimit; retryinterval)
// 
// Возвращает true если был вывод (false если не было изменеий и вывод не производился)
//-------------------------------------------------------------------------------------------------

bool hk3022::logdiff_n(Stream* s, bool f) {
  
  uint8_t b = 0;
  // полная запись
  if(f) { _ut = 0; _p = 0; _dp = 0; _pw = 0; _m = 0; _hl = 0; _ll = 0; _dl = 0; _pit = 0; _prl = 0; _ri = 0; b = 128; }
  
  // блок определения байта флагов
  int16_t dp = pressure - _p;
  if(abs(dp)>1 || (dp!=0 && (dp+_dp)!=0)) b += 3; // выводим только в виде непреобразованных данных (флаг 1+2)
                                       // фильтруем чтобы не было дребезга 
  if((poweron != _pw) || (mode != _m)) b += 4;
  if((hilimit != _hl) || (lolimit != _ll) || (drylimit != _dl)) b += 8;
  if((pumpinittime != _pit) || (pumprunlimit != _prl) || (retryinterval != _ri)) b += 16;

  // блок вывода
  if( b ) {
    // номер зоны
    s->print("H0");
    // флаги
    s->print(b);
    // давление
    if( b & 1) { print_with_semicolon(s, _dp = dp); _p = pressure; }
    // режим работы и подача мощности
    if( b & 4) { print_with_semicolon(s, mode+(poweron<<3)); _pw = poweron; _m = mode; }
    // значения параметров давления (hilimit; lolimit; drylimit)
    if( b & 8) { 
      print_with_semicolon(s, hilimit);
      print_with_semicolon(s, lolimit);
      print_with_semicolon(s, drylimit);
      _hl = hilimit; _ll = lolimit; _dl = drylimit;
    }
    // значения временных параметров (pumpinittime; pumprunlimit; retryinterval)
    if( b & 16) { 
      print_with_semicolon(s, pumpinittime);
      print_with_semicolon(s, pumprunlimit);
      print_with_semicolon(s, retryinterval);
      _pit = pumpinittime; _prl = pumprunlimit; _ri = retryinterval;
    }
    s->print(';');
    return true;
  }
  return false;
}
//
// END OF FILE
//

