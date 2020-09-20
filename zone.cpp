//
//    FILE: zone.cpp
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Библиотека для датчика слежения за температурной зоной
//          Датчик состоит из: 
//          - датчика температуры и влажности DHT22;
//          - кнопки управления (включает/выключает зону из процесса слежения)
//          - индикатора состояния зоны слежения (выкл, слежение вкл, нагрев)
//
// СХЕМА ДАТЧИКА
// 1...............5V ---*-------*
//                       |       |
//                       R1 10k  |
//                       |       *-------- DHT22-1
// 2..DataPin(In/Out) ---*---------------- DHT22-2
// 3..............GND ------------------*- DHT22-4
//                                      |
// 4......LEDPin(Out) -- LED -- R2 1k --*
//                                      |
// 5.BtnPin(InPullUp) -- Btn -----------*
//                      (Open-High,Closed-Low)
//

#include "zone.h"

button::button( uint8_t pin ) {
    _pin = pin;
    _old_value = HIGH;
    pinMode( _pin, INPUT_PULLUP );
  }

uint8_t button::state() {
    uint8_t v1 = digitalRead( _pin ); delay(5);
    uint8_t v2 = digitalRead( _pin ); delay(5);
    uint8_t v3 = digitalRead( _pin );
    if( v1<_old_value && v2<_old_value && v3<_old_value ) {
      _old_value = v1;
      return 1;
    }
    _old_value = v1;
    return 0;
  }

// Конструктор для зоны без управления
zone::zone(uint8_t dht_pin, char i) {
    id = i;
    _dht_pin = dht_pin;
  }

// Конструктор для зоны с управлением
zone::zone(uint8_t dht_pin, uint8_t led_pin, uint8_t btn_pin, uint8_t pw_pin, uint16_t tc, uint16_t dt, char i) {
    id = i;
    _btn = new button(btn_pin);
    _led_pin = led_pin;
    _dht_pin = dht_pin;
    _pw_pin = pw_pin;
    target_temperature = tc;
    delta = dt;
    pinMode(_led_pin, OUTPUT);
    pinMode(_pw_pin, OUTPUT);
  }

void zone::handler() {
  if( _btn == 0 ) return; // выходим, если датчик без управления
  // проверяем кнопку очень часто
  if( _btn->state() ) { mode = !mode; }
  digitalWrite(_led_pin, mode);
}
  
void zone::handler5s() {
    
    // READ DATA
    dht_status = _dht.read22(_dht_pin);
    temperature = _dht.temperature;
    humidity = _dht.humidity;
    //Serial.println((int)_btn);
	if( _btn == 0 ) return; // выходим, если датчик без управления
    if( dht_status == DHTLIB_OK ) {
      if( mode ) { 
        if( is_tc ) { // Состояние после достижения заданной температуры tc, но температура еще не упала ниже нижней границы
          if( temperature < (target_temperature-delta) ) { poweron = true; is_tc = false; } // температура упала ниже нижней границы
          else poweron = false; // проверяем, что мощность не подается
        }
        else {  // Состояние до достижения заданной температуры
          if( temperature >= target_temperature ) { poweron = false; is_tc = true; } // температура достигла заданного значения
          else poweron = true;  // температура не достигла заданного значения, проверяем, что мощность подается
        }
      }
      else {  // если режим поддержания температуры выключен, мощность на обогреватели не подаем
        poweron = false;
        is_tc = false;
      }
    }
    else {
      poweron = false; //если датчик в состоянии ошибки выключаем питание
    }
    digitalWrite(_pw_pin, poweron);
  }

void zone::print(Stream* s) {
    s->print(temperature); s->print(';');          // 0. температура1 t1
    s->print(target_temperature); s->print(';');   // 1. заданная температура1 tc1
    s->print(humidity); s->print(';');             // 2. влажность1 h1
    s->print(mode); s->print(';');                 // 3. режим работы1 m1
    s->print(poweron); s->print(';');              // 4. подача энергии p1
    s->print(delta); s->print(';');                // 5. гистерезис теипературы dt1
    s->print(dht_status); s->print(';');           // 6. состояние датчика s1
}

void zone::println(Stream* s) {
    print(s);
    s->println();
}

void zone::writeconf(Stream* s) {
  if( _btn == 0 ) return; // выходим, если датчик без управления
  s->print('s');s->print(id);s->print('t');s->println(target_temperature); // Запись заданной температуры
  s->print('s');s->print(id);s->print('m');s->println(mode);               // Запись режима
}

//-------------------------------------------------------------------------------------------------
// Лог температурной зоны:
// Строка представляет собой событие. Событие может выводиться двумя типами записей: 
//   - полная запись (первая запись в файле лога или после перезагрузки);
//   - разностная запись (записывается разность между текущим значением и предыдущим).
// Порядок полей при выводе событий:
// 1. Флаги событий int8_t
//    1 - Изменение фактической температуры
//    2 - Изменение фактической влажности
//    4 - Бит подачи мощности на обогреватель
//    8 - Бит режима работы
//   16 - Изменение заданной температуры
//   32 - Изменение заданного гистерезиса температуры
//   64 - Изменение состояния датчика
//  128 - Признак полной записи (выводятся все поля в виде полного значения)
// 2. Метка времени. Тип unixtime, выводится разница с предыдущим значением в потоке.
// Далее в соответствии с установленными битами флагов событий выводятся параметры:
// 3. Фактическая температура. Тип int, выводится разница с предыдущим значением в потоке.
// 4. Фактическая влажность. Тип int, выводится разница с предыдущим значением в потоке.
// 5. Заданная температура. Тип int, выводится разница с предыдущим значением в потоке.
// 6. Заданный гистерезис температуры. Тип int, выводится разница с предыдущим значением в потоке.
// 7. Состояние датчика.  Тип int, выводится полное значение.
//-------------------------------------------------------------------------------------------------

void zone::logdiff(Stream* s, uint32_t unixtime, bool f) {
  
  uint8_t b = 0;
  // полная запись
  if(f) { _ut = 0; _t = 0; _h = 0; _tc = 0; _dt = 0; _s = 0; _m = 0; _p = 0; b = 128; }
  
  // блок определения байта флагов
  if( temperature != _t ) b += 1;
  if( humidity != _h ) b += 2;
  if( target_temperature != _tc) b += 16;
  if( delta != _dt ) b += 32;
  if( dht_status != _s ) b += 64;

  // блок вывода
  if( b || (poweron != _p) || (mode != _m) ) {
    b += (poweron<<2) + (mode<<3);
    _p = poweron; _m = mode;
    // флаги
    s->print(b);
    // unixtime
    s->print(';'); s->print( unixtime - _ut); _ut = unixtime;
    // температура
    if( b & 1) { s->print(';'); s->print( temperature - _t ); _t = temperature; }
    // влажность
    if( b & 2) { s->print(';'); s->print( humidity - _h ); _h = humidity; }
    // заданная температура
    if( b & 16) { s->print(';'); s->print(target_temperature); _tc = target_temperature; }
    // гистерезис теипературы
    if( b & 32) { s->print(';'); s->print(delta); _dt = delta; }
    // состояние датчика
    if( b & 64) { s->print(';'); s->print(dht_status); _s = dht_status; }
    // конец строки
    s->println();
  }
}

//
// END OF FILE
//
