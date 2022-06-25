/*
 * Arduino MEGA
 * 
 *                                                         Digital D21/SCL
 *                                                         Digital D20/SCA
 *                                                                 AREF
 *                                                                 GND
 *                    
 *                    NC                                   Digital ~D13      Eth SCK
 *                    IOREF                                Digital ~D12      Eth MISO
 *                    RESET                                Digital ~D11      Eth MOSI
 *                    +3V3                                 Digital ~D10      Eth SS for Ethernet controller
 *                    +5V                                  Digital ~D9       Zone2 Data            Green
 *                    GND                                  Digital ~D8       Zone2 LED             Blue
 * Brown              GND
 *                    VIN                                  Digital ~D7       Zone2 Btn             Blue/W
 *                                                         Digital ~D6       Zone3 Data            Green
 * Oran/W Zone1 Pwr   A0/D54  Analog                       Digital ~D5       Zone1 Btn             Blue/W
 * Gree/W Zone2 Pwr   A1/D55  Analog                       Digital ~D4       Eth SS for SD-card
 * Green  Hydro Data  A2/D56  Analog                       Digital ~D3       Zone1 LED             Blue
 * Blue   Hydro Pump  A3/D57  Analog                       Digital ~D2       Zone1 Data            Green
 * Bl/W Irrigate Pwr  A4/D58  Analog                       Digital ~D1/TX0   XXX
 *                    A5/D59  Analog                       Digital ~D0/RX0   XXX
 *                    A6/D60  Analog
 *                    A7/D61  Analog                       Digital D14/TX3   
 *                                                         Digital D15/RX3   
 *                    A8/D62  Analog                       Digital D16/TX2
 *                    A9/D63  Analog                       Digital D17/RX2
 *                    A10/D64 Analog                       Digital D18/TX1
 *                    A11/D65 Analog                       Digital D19/RX1
 *                    A12/D66 Analog             Digital Input/I2C D20/SDA
 *                    A13/D67 Analog             Digital Input/I2C D21/SCL
 *                    A14/D68 Analog
 *                    A15/D69 Analog
 *                    
 *                            G D D D D D D D D D D D D D D D D 5 
 *                            N 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 V
 *                            D 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2
 *                      
 *                            G D D D D D D D D D D D D D D D D 5 
 *                            N 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 V
 *                            D 3 1 9 7 5 3 1 9 7 5 3 1 9 7 5 3
 */

#include <SPI.h>
#include <SD.h>
#include "zone.h"
#include "hk3022.h"
#include "irrigate.h"
#include "ticker.h"
#include "webserver.h"

#define NZ 2 // Число действующих зон

//DHT22_pin, LED_pin, BUTTON_pin, POWER_pin, Tc, dT, id
//DHT22_pin, id
//zone z[NZ] = {zone(2,3,5,A0,300,1,'1'),zone(9,8,7,A1,300,1,'2'),zone(6,'3')};
zone z[NZ] = {zone(9,8,7,A1,300,1,'2'),zone(6,'3')};

hk3022 hydro = hk3022(A2, A3);
irrigate izone = irrigate(A4);
ticker tck;
webserver web;
const char _version[] = "20220620"; // Версия прошивки 29576 bytes

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("SD");
  if (!SD.begin(4)) {
    Serial.println(" error");
    return;
  }
  Serial.println(" OK");
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
//  web.begin( mac, {192,168,1,10}, {192,168,1,1}, {255,255,255,0} );  // MAC, IP, GATEWAY, MASK
//  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  web.begin( mac, {192,168,2,2}, {192,168,2,3}, {255,255,255,0} );  // MAC, IP, GATEWAY, MASK

  tck.begin();          // Запускаем тикер
  readconf();           // Читаем конфигурацию
}

uint32_t lc = 0;

void loop() {
  for(int i=0; i<NZ; i++) { z[i].handler(); }
  web.handler(ajax_handler);
  hydro.handler(tck.unixtime);
  tck.handler5s( h5s );
  lc += 1;
}

//
// Вызываем каждые 5 сек
//

bool _fe = true; // признак начала записи в лог
void h5s() {
  char s[10];
  /*
  Serial.print(yyyymmdd(s,tck.unixtime)); Serial.print(" ");
  Serial.print(hhmmss(s,tck.unixtime)); Serial.print(" ");
  Serial.print(tck.unixtime-tck.starttime); Serial.print(" ");
  Serial.print(tck._t); Serial.println(" ");
  Serial.println(hydro.pressure);
  */
  Serial.print("lc="); Serial.println(lc);
  lc = 0;

  for(int i=0; i<NZ; i++) { z[i].handler5s(); }

  hydro.handler5s(tck.unixtime);
  izone.handler5s(tck.unixtime, (hydro.mode&3) && tck._is_sync );
  
  if(tck.starttime) { // Если время было синхронизировано
    char f[20] = {'l','o','g','/',0};
    yyyymmdd(&f[4],tck.unixtime);
    
    // вывод логов по температурным зонам
    f[12] = '.'; f[13] = 'z'; f[15] = 0; 
    for(int i=0; i<NZ; i++) {
      // Лог зоны i log/YYYYMMDD.zi
      f[14] = z[i].id;
      File logfile = SD.open(f, FILE_WRITE);
      if (logfile) {
        if (logfile.size() == 0) z[i].logdiff(&logfile,tck.unixtime,true);
        else z[i].logdiff(&logfile,tck.unixtime,_fe);
        logfile.close();
      }
    }

    // вывод логов по системе водоснабжения log/YYYYMMDD.h0
    f[13] = 'h'; f[14] = '0'; 
    File logfile = SD.open(f, FILE_WRITE);
    if (logfile) {
      if (logfile.size() == 0) hydro.logdiff(&logfile,tck.unixtime,true);
      else hydro.logdiff(&logfile,tck.unixtime,_fe);
      logfile.close();
    }

    // вывод логов по системе автополива log/YYYYMMDD.i0
    f[13] = 'i'; f[14] = '0'; 
    logfile = SD.open(f, FILE_WRITE);
    if (logfile) {
      if (logfile.size() == 0) izone.logdiff(&logfile,tck.unixtime,true);
      else izone.logdiff(&logfile,tck.unixtime,_fe);
      logfile.close();
    }
    
    _fe = false;
  }

}

///////////////////////////////////////////////////////////
// Система команд AJAX запросов
// 
// gi           - получить информацию о состоянии
// glYYYYMMDD   - получить лог за дату YYYY.MM.DD (можно заменить обычным GET /log/YYYYMMDD)
// s1t:xx.x     - задать температуру в зоне 1 с точностью до 0.1 (четыре символа, остальное игнорируем)
// s2t:xx.x     - задать температуру в зоне 1 с точностью до 0.1 (четыре символа, остальное игнорируем)
// s1m:x        - задать режим зоны 1 (один символ, остальное игнорируем)
// s2m:x        - задать режим зоны 1 (один символ, остальное игнорируем)
// l:/xxxxx/xx  - получить список файлов в указанном каталоге
// d:/xxxxx/xx  - удалить файл
// v            - получить версию прошивки (не реализовано)

#define AJAX_BEGIN    0
#define AJAX_G        1
#define AJAX_GI       2
#define AJAX_L        3
#define AJAX_D        4
#define AJAX_S        8
#define AJAX_SN       9
#define AJAX_SNT      10
#define AJAX_SNT_END  11
#define AJAX_SNM      12
#define AJAX_SNM_END  13

class ajax_req_parser {
public:
  uint8_t state = 0;  // состояние парсера
  uint8_t z = 0;      // номер зоны (0 - не определена, z <= NZ)
  int16_t x = 0;      // параметр (температура представляется целым числом t*10)
  char* parstr;  

  int8_t parse(char* req) {
    for(int i=0; req[i]!=0; i++) {
      char c = req[i];
      if( c == ':' || c == '.' ) continue; // пропускаем двоеточия и точки
      if( state == AJAX_BEGIN ) {
        if( c == 'g' ) state = AJAX_G;
        if( c == 's' ) state = AJAX_S;                
        if( c == 'l' ) { state = AJAX_L; parstr = &req[i+1]; break; } // список файлов
        if( c == 'd' ) { state = AJAX_D; parstr = &req[i+1]; break; } // удаление файла
        continue;
      }
      if( state == AJAX_G ) {
        if( c == 'i' ) state = AJAX_GI;
        break;
      }
      if( state == AJAX_S ) {
//        if( c == '1' ) { state = AJAX_SN; z = 1; } 
//        if( c == '2' ) { state = AJAX_SN; z = 2; }
        state = AJAX_SN; z = c - '0';
        if( z > NZ ) z = 0; // если неверное значение номера зоны, сбрасываем
        continue;
      }
      if( state == AJAX_SN ) {
        if( c == 't' ) state = AJAX_SNT;
        if( c == 'm' ) state = AJAX_SNM;
        continue;
      }
      if( state == AJAX_SNT ) {
        x = 0;
// Закомментирована обработка отрицательных чисел
//      bool minus = false;
//      if( req[i] == '-' ) { minus = true; i++; }
        while(req[i]!=0) {
          x *= 10; 
          x += req[i]-'0';
          i++;
        }
//      if(minus) x = -x;
        state = AJAX_SNT_END;
        break;
      }
      if( state == AJAX_SNM ) {
        x = c-'0'; state = AJAX_SNM_END;
        break;
      }
    } //конец цикла парсинга строки
    return state;
  }
};

void ajax_handler(EthernetClient client, char* req) {
  ajax_req_parser ajaxp;
  ajaxp.parse(req);
  
  /////////////////////////////////////////
  // gi Вывод информации о состоянии
  /////////////////////////////////////////
  if( ajaxp.state == AJAX_GI ) {
    client.println(http200);
    client.println(httpaccesscontrol);
    client.println(httpconnectionclose);
    client.println();

    // новый вариант вывода
    // 0:version, 1:numofzones,
    // 2:unixtime, 3:starttime, 4:lastsynctime, 5:lastsyncdelta, 6:lastsyncinterval, 7:tickcounter
    // 8:t1, 9:tc1, 10:h1, 11:m1, 12:p1, 13:dt1, 14:s1, 
    // 15:t2, 16:tc2, 17:h2, 18:m2, 19:p2, 20:dt2, 21:s2,
    // 22:t3, 23:tc3, 24:h3, 25:m3, 26:p3, 27:dt3, 28:s3,
    // 29:p_hydro

    //client.print(_version); 
    client.print(_version); client.print(';');
      // 0.версия прошивки
      // 1.число зон
    client.print(NZ); client.print(';');
    tck.print(&client);
      // 2.текущее время
      // 3.время запуска
      // 4.время последней синхронизации
      // 5.ошибка последней синхронизации
      // 6.последний интервал синхронизации
      // 7.счетчик миллисекунд контроллера
    for(int i=0;i<NZ;i++) {
      z[i].print(&client);
      // 8. температура1 t1
      // 9. заданная температура1 tc1
      // 10. влажность1 h1
      // 11. режим работы1 m1
      // 12. подача энергии p1
      // 13. гистерезис температуры dt1
      // 14. состояние датчика s1
    }
    hydro.print(&client);
      // 29. давление в системе водоснабжения
    client.println();
    return;
  }
  /////////////////////////////////////////
  // snt:xx.x Задание температуры
  /////////////////////////////////////////
  
  // !!!!!!!!!!!!!!!!!!!!!!!!ДОБАВИТЬ ПРОВЕРКУ ПО ajaxp.z-1!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  if( ajaxp.state == AJAX_SNT_END ) {
    if( ajaxp.z == 0 ) return; // зона не определена
    z[ajaxp.z-1].target_temperature = ajaxp.x;
    client.println(http200);
    client.println(httpaccesscontrol);
    client.println(httpconnectionclose);
    client.println();
    writeconf();
    return;
  }
  /////////////////////////////////////////
  // snm:x Задание режима
  /////////////////////////////////////////
  if( ajaxp.state == AJAX_SNM_END ) {
    if( ajaxp.z == 0 ) return; // зона не определена
    z[ajaxp.z-1].mode = ajaxp.x;
    client.println(http200);
    client.println(httpaccesscontrol);
    client.println(httpconnectionclose);
    client.println();
    writeconf();
    return;
  }
  /*
  /////////////////////////////////////////
  // l:/xxxxx/xxxx Запрос списка файлов в директории /xxxxx/xxxx. Выводим с разделителем ';'
  /////////////////////////////////////////
  if( ajaxp.state == AJAX_L ) {
    client.println(http200);
    client.println(httpconnectionclose);
    client.println();
    File dir = SD.open(ajaxp.parstr);
    if(dir.isDirectory()) {
      dir.rewindDirectory();
      client.print('.');
      while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
          // no more files
          break;
        }
        client.print(';');
        client.print(entry.name());
        if (entry.isDirectory()) {
          client.print('/');
        } else {
          // files have sizes, directories do not
          //client.print(entry.size());
        }
        entry.close();
      }
    }
    dir.close();

    client.println();
    return;
  }

  /////////////////////////////////////////
  // d:/xxxxx/xxxx Удаление файла /xxxxx/xxxx
  /////////////////////////////////////////
  
  if( ajaxp.state == AJAX_D ) {
    client.println(http200);
    client.println(httpconnectionclose);
    client.println();
    client.print(ajaxp.parstr);
    if(SD.remove(ajaxp.parstr)) {
      client.print(" deleted");
    }
    else {
      client.print(" not found");
    }
    client.println();
    return;
  }
*/
  client.println(http501);
  client.println();
  
}

void writeconf() {
  SD.remove("conf");  // удаляем старую конфигурацию
  File f = SD.open("conf", FILE_WRITE);
  if(f) {
    for(int i=0; i<NZ; i++) z[i].writeconf(&f);
  }
  f.close();
}

void readconf() {
  File f = SD.open("conf");
  char s[20];
  if(f) {
    while(f.available()) {
      for( int8_t i = 0; i < 20; i++ ) {
        s[i] = f.read();
        if( s[i] == '\r' ) s[i] = 0; 
        if( s[i] == '\n' ) { s[i] = 0; break; }
      }
      ajax_req_parser ajaxp;
      ajaxp.parse(s);
      Serial.println(s);
      if( ajaxp.state == AJAX_SNT_END ) {
        z[ajaxp.z-1].target_temperature = ajaxp.x;
      }
      if( ajaxp.state == AJAX_SNM_END ) {
        z[ajaxp.z-1].mode = ajaxp.x;
      }
    }
  }
  f.close();
}

