//
//    FILE: utils.h
//  AUTHOR: Andrew Zimazky
// VERSION: 0.0.1
// PURPOSE: Print utils
//

#ifndef utils_h
#define utils_h

#include <Arduino.h>

void print_with_semicolon(Stream* s, const String &arg);
void print_with_semicolon(Stream* s, const char arg[]);
void print_with_semicolon(Stream* s, char arg);
void print_with_semicolon(Stream* s, unsigned char arg);
void print_with_semicolon(Stream* s, int arg);
void print_with_semicolon(Stream* s, unsigned int arg);
void print_with_semicolon(Stream* s, long arg);
void print_with_semicolon(Stream* s, unsigned long arg);
void print_with_semicolon(Stream* s, double arg);

//void print_param(Stream* s, const char name[], unsigned long arg);

#endif
//
// END OF FILE
//
