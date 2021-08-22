/*
 * Arduino UNO
 * 
 */

#include <SPI.h>
#include <SD.h>
#include "zone.h"
#include "hk3022.h"
#include "ticker.h"
#include "webserver.h"

#define NZ 3 // Число действующих зон

//DHT22_pin, LED_pin, BUTTON_pin, POWER_pin, Tc, dT, id
//DHT22_pin, id

zone z[NZ] = {zone(9,8,7,A0,300,5,'1'),zone(2,3,5,A1,300,1,'2'),zone(6,'3')};
//zone z[NZ] = {zone(2,3,5,A1,300,1,'2'),zone(6,'3')};

hk3022 hydro = hk3022(A2);
ticker tck;
webserver web;
const char _version[] = "20210822"; // Версия прошивки 27080 bytes

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


void loop() {
  for(int i=0; i<NZ; i++) { z[i].handler(); }
  web.handler(ajax_handler);
  tck.handler5s( h5s );
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
  
  for(int i=0; i<NZ; i++) { z[i].handler5s(); }

  hydro.read();
  
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

    // устаревший вариант вывода
    // 0:t1, 1:tc1, 2:h1, 3:m1, 4:p1, 5:dt1, 6:s1, 
    // 7:t2, 8:tc2, 9:h2, 10:m2, 11:p2, 12:dt2, 13:s2,
    // 14:unixtime, 15:starttime, 16:lastsynctime, 17:lastsyncdelta, 18:lastsyncinterval

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

