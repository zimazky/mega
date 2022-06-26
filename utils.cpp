#include "utils.h"

template<class T> void semicolon_print(Stream* s, T arg) {
  s->print(';'); s->print(arg);
}