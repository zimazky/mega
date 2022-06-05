//
//    FILE: button.cpp
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Класс кнопки управления
//

#include "button.h"

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

