
#ifndef webserver_h
#define webserver_h

#include <eth.h>

extern const char http200[];
extern const char http404[];
extern const char http501[];
extern const char httpconnectionclose[];
extern const char httpaccesscontrol[];

class webserver {
public:  
  void begin(byte* mac, IPAddress ip, IPAddress gateway, IPAddress subnet);
  void handler(void (*ajaxf)(EthernetClient cl, char* req));
};

#endif
//
// END OF FILE
//
