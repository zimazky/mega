//
//    FILE: button.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Class button definition
//

#ifndef button_h
#define button_h

#include <Arduino.h>

class button {
public:  
  button( uint8_t pin );
  uint8_t state();            // Состояние кнопки (нажата/отпущена)
private:
  uint8_t _pin;
  uint8_t _old_value;
};

#endif
//
// END OF FILE
//
