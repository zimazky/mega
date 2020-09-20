

#include <SPI.h>
#include <SD.h>
#include "webserver.h"

// Примеры запросов: 
//
// GET / HTTP/1.1
// Host: 10.0.0.20
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:18.0) Gecko/20100101 Firefox/18.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// Accept-Language: en-ZA,en-GB;q=0.8,en-US;q=0.5,en;q=0.3
// Accept-Encoding: gzip, deflate
// Referer: http://10.0.0.20/?LED2=2
// Connection: keep-alive
//
// GET /?LED2=2 HTTP/1.1
// Host: 10.0.0.20
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:18.0) Gecko/20100101 Firefox/18.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// Accept-Language: en-ZA,en-GB;q=0.8,en-US;q=0.5,en;q=0.3
// Accept-Encoding: gzip, deflate
// Referer: http://10.0.0.20/
// Connection: keep-alive
//
// GET /ajax_switch&nocache=29860.903564600583 HTTP/1.1
// Host: 10.0.0.20
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:18.0) Gecko/20100101 Firefox/18.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// Accept-Language: en-ZA,en-GB;q=0.8,en-US;q=0.5,en;q=0.3
// Accept-Encoding: gzip, deflate
// Referer: http://10.0.0.20/
// Connection: keep-alive
//
// POST /foo.php HTTP/1.1  
// Host: localhost  
// ...
// Content-Type: application/x-www-form-urlencoded  
// Content-Length: 43  
//
// first_name=John&last_name=Doe&action=Submit  
//


// POST / HTTP/1.1
// Host: 192.168.1.2
// Connection: keep-alive
// Content-Length: 1780
// Cache-Control: max-age=0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// Origin: http://192.168.1.2
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundarykyOQrWZuEWAKFl7g
// Referer: http://192.168.1.2/upload.htm
// Accept-Encoding: gzip, deflate
// Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4

const char http200[] = "HTTP/1.1 200 OK";
const char http404[] = "HTTP/1.1 404 Not Found";
const char http501[] = "HTTP/1.1 501 Not Implemented";
const char httpconnectionclose[] = "Connection: close";
const char httpaccesscontrol[] = "Access-Control-Allow-Origin: *";
const char contentlength[] = "Content-Length:";
const char GET[] = "GET";
const char POST[] = "POST";
//////////////////////////////////////////////////////////////////////
// Обработчик HTTP запроса на лету
//////////////////////////////////////////////////////////////////////
class http_req_parser {
public:
#define REQ_SIZE 64
  char req[REQ_SIZE] = {0};

#define PARSER_ERROR                  -1
#define PARSER_BEGIN                  0
#define PARSER_GET                    1
#define PARSER_POST                   2
#define PARSER_METHOD_END             3
#define PARSER_REQ                    4
#define PARSER_REQ_END                5
#define PARSER_REQ_ERROR_OVERFLOW     -2
#define PARSER_NEWLINE                6
#define PARSER_CONTENTLENGTH          7
#define PARSER_CONTENTLENGTH_END      8
#define PARSER_LENGTH                 9
#define PARSER_LENGTH_END             10
#define PARSER_NEWLINE2               11
#define PARSER_HTTP_END               -3

  int8_t state = 0;           // Регистр состояния { BEGIN=0, GET(1,2,3), REQ(1..REQ_SIZE), HTTP(1,2,3,4), END }
  uint8_t i = 0;              // Вспомогательный счетчик
  bool ajax = 0;              // Признак AJAX запроса
  bool post = 0;              // Признак POST запроса
  uint32_t datalength = 0;    // Размер данных при передаче через POST запрос
  

  // Функция определения соответствия символа (c) i-ому символу строки (str)
  // возвращаем значения: соответствует до текущего символа, не соответствует, полностью соответствует (строка закончилась)
  // i - общий счетчик символа в текущей строке
#define STR_MATCH         1
#define STR_END           -1
#define STR_DONOTMATCH    0
  int8_t strmatch( char c, const char* str ) {
    if( i < (strlen(str)) ) { // Не вышли за пределы строки
      if ( c != str[i] ) return STR_DONOTMATCH;
      return STR_MATCH;
    }
//    i;
    return STR_END;
  }
  
  int8_t parse(char c) {            // Разбираем запрос на лету
    if( state == PARSER_BEGIN ) { 
      if( strmatch(c, GET) ) { i++; return state = PARSER_GET; }              // Начало запроса GET
      else if( strmatch(c, POST) ) { i++; return state = PARSER_POST; }       // Начало запроса POST
      else { i++; return state = PARSER_ERROR; }                              // Тип запроса не поддерживается
    }
    else if( state == PARSER_GET ) {
      int8_t isg = strmatch(c, GET);
      if( isg == STR_END ) { i++; return state = PARSER_METHOD_END; }         // Метод запроса определен (post == 0 - GET, post == 1 - POST)
      else if( isg == STR_DONOTMATCH ) { i++; return state = PARSER_ERROR; }  // Метод запроса не поддерживается
      else { i++; return state; }                                             // Запрос GET в процессе определения
    }
    else if( state == PARSER_POST ) {
      int8_t isg = strmatch(c, POST);
      if( isg == STR_END ) { i++; post = true; return state = PARSER_METHOD_END; } //Метод запроса определен (post == 0 - GET, post == 1 - POST)
      else if( isg == STR_DONOTMATCH ) { i++; return state = PARSER_ERROR; }  // Тип запроса не поддерживается
      else { i++; return state; }                                             // Запрос POST в процессе определения
    }
    else if( state == PARSER_METHOD_END ) {
      if( c == ' ' ) return state;                                            // пропускаем пробелы (PARSER_METHOD_END)
      else { i = 0; req[i++] = c; return state = PARSER_REQ; }                // начало строки запроса
    }
    else if( state == PARSER_REQ) {
      ///////////////////////////////////////////////////
      // Строка запроса завершена
      ///////////////////////////////////////////////////
      if( c == ' '  || (ajax && c=='&') ) {       // пробел или & в ajax запросе
        req[i]=0;                                 // завершаем строку нулем
        return state = PARSER_REQ_END;            // конец запроса (PARSER_REQ_END)
      }
      ///////////////////////////////////////////////////
      // Строка запроса еще не завершена
      ///////////////////////////////////////////////////
      else {
        ///////////////////////////////////////////////////
        // Проверка на AJAX запрос
        ///////////////////////////////////////////////////
        if( ajax==0 && i==2 && req[0]=='/' && req[1]=='?' ) {
          ajax = true;                            // устанавливаем признак AJAX запроса
          i=0;                                    // первый символ строки AJAX запроса
        }
        req[i] = c;                               // очередной символ строки запроса
        i++;
        if( i >= REQ_SIZE ) return state = PARSER_REQ_ERROR_OVERFLOW; // переполнение строки запроса
        return state;               // (PARSER_REQ)
      }
    }
    else if( state == PARSER_REQ_END ) {
      if( c == '\n' ) { i=0; return state = PARSER_NEWLINE; } // ждем конца строки заголовка
      else return state;
    }
    else if( state == PARSER_NEWLINE ) {              
      if( c == '\n' ) return state = PARSER_HTTP_END;                           // Проверяем на пустую строку - конец всего заголовка
      if( c == '\r' ) return state;                                             // Перевод строки не влияет на состояние
      if( strmatch(c,contentlength) ) { i++; return state = PARSER_CONTENTLENGTH; }
      else return state = PARSER_REQ_END;
    }
    else if( state == PARSER_CONTENTLENGTH ) {              
      if( c == '\n' ) { i=0; return state = PARSER_NEWLINE; }                   // проверяем на конец строки заголовка
      int8_t isc = strmatch(c,contentlength);
      if( isc == STR_END ) { i++; return state = PARSER_CONTENTLENGTH_END; }    // Ключевое слово определено
      else if( isc == STR_DONOTMATCH ) { i++; return state = PARSER_REQ_END; }  // Ключевое слово не найдено 
      else { i++; return state; }                                               // Ключевое слово в процессе определения
    }
    else if( state == PARSER_CONTENTLENGTH_END ) {              
      if( c == ' ' ) return state;                                              // пропускаем пробелы (PARSER_CONTENTLENGTH_END)
      else { datalength = c-'0'; return state = PARSER_LENGTH; }                // начало строки размера данных
    }
    else if( state == PARSER_LENGTH ) {              
      if( c == '\r' || c == '\n' || c == ' ' ) return state = PARSER_LENGTH_END;// конец строки размера данных (PARSER_LENGTH_END)
      else { datalength *= 10; datalength += c-'0'; return state; }             // обработка строки размера данных продолжается
    }
    else if( state == PARSER_LENGTH_END ) {              
      if( c == '\n' ) return state = PARSER_NEWLINE2;  // 
      else return state;
    }
    else if( state == PARSER_NEWLINE2 ) {              
      if( c == '\n' ) return state = PARSER_HTTP_END;                           // Проверяем на пустую строку - конец всего заголовка
      if( c == '\r' ) return state;                                             // Перевод строки не влияет на состояние
      else return state = PARSER_LENGTH_END;
    }
    return state = PARSER_ERROR;  
  }

};
//////////////////////////////////////////////////////////////////////
// Простой обработчик тела multipart/ на лету
//////////////////////////////////////////////////////////////////////
class multipart_parser {
public:  
  int8_t parse(char c) {
    // 1. Читаем первую строку, которая должна содержать --boundary\r\n. Определяем длину первой строки и запоминаем ее.
    // 2. Определяем имя файла
    // 3. Ждем пустую строку, запоминаем число байт с начала. Отсюда начинается файл, длина файла = Размер данных (Content-Length) минус число байт с начала данных и минус (длина первой строки + 2)
    // 4. Читаем данные и записываем в файл (предварительно удаляем старый)
  }
};


EthernetServer server(80);
  
void webserver::begin(byte* mac, IPAddress ip, IPAddress gateway, IPAddress subnet) {
  Ethernet.begin(mac, ip, gateway, subnet);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  Serial.print("Web IP:");
  Serial.println(Ethernet.localIP());
}

int8_t strsearch( char c, const char* str, int8_t i ) {
  if( i < (strlen(str)) ) { // Не вышли за пределы строки
    if ( c != str[i] ) return STR_DONOTMATCH;
    return STR_MATCH;
  }
  return STR_END;
}

void webserver::handler(void (*ajaxf)(EthernetClient cl, char* req)) {
    
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    http_req_parser parser;
    while( client.connected() ) {
      if( client.available() ) {
        char c = client.read();
        Serial.write(c);
        parser.parse(c);
        ///////////////////////////////////////////////////
        // Обработчик POST запроса
        ///////////////////////////////////////////////////
        if( parser.post ) { 
          if( parser.state == PARSER_HTTP_END ) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Вставляем код для принятия данных от клиента
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Serial.print("Length: "); Serial.println(parser.datalength);

            client.println(http200);
            client.println();

            uint8_t p = 0;   // число байт с начала тела запроса
            while( client.read() != '\n' ) { p++; }   // читаем первую строку, определяем длину --boundary\r\n
            uint8_t b = ++p;   // здесь определим длину строки boundary --boundary\r\n, должна быть меньше 2+70+2=74
            bool emptystr = true;
            char filename[16];
            // ищем пустую строку
            int8_t j = 0;
            int8_t m = 0;
            filename[m++]='/';
            while( 1 ) { 
              c = client.read();
              client.write(c);
              p++;
              if( c == '\n' ) {
                if(emptystr) break;
                emptystr = true;
              }
              else if( c != '\r' ) emptystr = false;
              // поиск имени файла
              int8_t isf = strsearch(c,"filename=",j);
              if( j>=0 ) { //ищем параметр filename=
                if( isf == STR_DONOTMATCH ) { j = 0; }
                else if( isf == STR_MATCH ) { j++; }
                else j = -1;    // нашли строку
              }
              else if( j == -1 ) { // сохраняем имя файла
                if( c == ' ' || c == '\r' || c == '\n' ) { filename[m] = 0; j = -2; }
                else if( c != '"' ) { filename[m] = c; m++; }
              }
            }
            parser.datalength -= (p+b+4);  //??? p+b+2+2
//            Serial.print("Length: "); Serial.println(parser.datalength);
//            Serial.print("Filename: "); Serial.println(filename);

            SD.remove(filename);
            File f = SD.open(filename, FILE_WRITE);
            uint8_t buf[128];                           // буфер для ускорения вывода
            if(f) {
              for(;parser.datalength>=128;parser.datalength-=128) {
                client.read(buf,128);
                client.write(buf,128);
                f.write(buf,128);
              }
              client.read(buf,parser.datalength);
              client.write(buf,parser.datalength);
              f.write(buf,parser.datalength);              
              
              f.close();
            }

            break;    // выходим из цикла после ответа
          }
          else if( parser.state < 0 ) {
            /////////////////////////////////////////////////
            // Запрос не распознан/ не поддерживается
            client.println(http501);
            client.println();

            break;    // выходим из цикла после ответа
          }
        }
        ///////////////////////////////////////////////////
        // Обработчик GET запроса
        ///////////////////////////////////////////////////
        else if( parser.state == PARSER_REQ_END ) { // считали строку запроса полностью
          Serial.println();
          if( parser.ajax ) {
            ajaxf( client, parser.req ); // переходим в обработчик AJAX запроса
            break;    // выходим из цикла
          }
          Serial.println(parser.req);
          
          /////////////////////////////////////////////////
          // send web page
          File f;
          if( parser.req[0] == '/'  &&  parser.req[1] == 0 ) f = SD.open("index.htm");
          else f = SD.open(parser.req);               // открываем файл
          char buffer[128];                           // буфер для ускорения вывода
          if ( f && !f.isDirectory() ) {
            ///////////////////////////////////////////////
            // Файл открыт успешно и это не каталог
            client.println(http200);
            client.println(httpaccesscontrol);
            client.println(httpconnectionclose);
            client.println();
            ///////////////////////////////////////////////
            // Используем буферизованное чтение/вывод
            while(f.available()) client.write(buffer,f.read(buffer,128)); // отдаем файл клиенту
          }
          else {
            ///////////////////////////////////////////////
            // Файла нет
            client.println(http404);
            client.println();
            //Serial.println(http404);
          } 
          f.close();         
          break;    // выходим из цикла после ответа
        }
        else if( parser.state < 0 ) { 
          /////////////////////////////////////////////////
          // Запрос не распознан/ не поддерживается
          client.println(http501);
          client.println();
          break;    // выходим из цикла после ответа
        }
      }
    }
//    Serial.println();
//    Serial.print("PARSER STATE: "); Serial.println(parser.state);
    // give the web browser time to receive the data
    //delay(1);
    // close the connection:
    client.stop();
  }
    
}
  
//
// END OF FILE
//
