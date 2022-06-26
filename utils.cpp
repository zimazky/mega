#include "utils.h"

void print_with_semicolon(Stream* s, const String &arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, const char arg[]) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, char arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, unsigned char arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, int arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, unsigned int arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, long arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, unsigned long arg) {
  s->print(';');s->print(arg);
}
void print_with_semicolon(Stream* s, double arg) {
  s->print(';');s->print(arg);
}


