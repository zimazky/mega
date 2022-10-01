# MEGA

Прошивка контроллера Arduino MEGA для слежением и поддержанием параметров систем терморегулирования, системы водоснабжения и автополива

Для получения информации и управления контроллером используется API через HTTP GET-запросы

## API

    
    gi           - получить информацию о состоянии
    glYYYYMMDD   - получить лог за дату YYYY.MM.DD (можно заменить обычным GET /log/YYYYMMDD)
    s1t:xx.x     - задать температуру в зоне 1 с точностью до 0.1 (четыре символа, остальное игнорируем)
    s2t:xx.x     - задать температуру в зоне 1 с точностью до 0.1 (четыре символа, остальное игнорируем)
    s1m:x        - задать режим зоны 1 (один символ, остальное игнорируем)
    s2m:x        - задать режим зоны 1 (один символ, остальное игнорируем)
    l:/xxxxx/xx  - получить список файлов в указанном каталоге
    d:/xxxxx/xx  - удалить файл
    v            - получить версию прошивки (не реализовано)

## Распиновка Arduino MEGA
    
                              ______________________________________
                             |                              Digital | D21/SCL
                             |                              Digital | D20/SCA
                             |                                      | AREF
                             |                                      | GND
                             |                                      |
                      NC     |                              Digital | ~D13      Eth SCK
                      IOREF  |                              Digital | ~D12      Eth MISO
                      RESET  |                              Digital | ~D11      Eth MOSI
                      +3V3   |                              Digital | ~D10      Eth SS for Ethernet controller
                      +5V    |                              Digital | ~D9       Zone2 Data            Green
                      GND    |                              Digital | ~D8       Zone2 LED             Blue
    Brown             GND    |                                      |
                      VIN    |                              Digital | ~D7       Zone2 Btn             Blue/W
                             |                              Digital | ~D6       Zone3 Data            Green
    Oran/W Zone1 Pwr  A0/D54 | Analog                       Digital | ~D5       Zone1 Btn             Blue/W
    Gree/W Zone2 Pwr  A1/D55 | Analog                       Digital | ~D4       Eth SS for SD-card
    Green  Hydro Data A2/D56 | Analog                       Digital | ~D3       Zone1 LED             Blue
    Blue   Hydro Pump A3/D57 | Analog                       Digital | ~D2       Zone1 Data            Green
    Bl/W Irrigate Pwr A4/D58 | Analog                       Digital | ~D1/TX0   XXX
                      A5/D59 | Analog                       Digital | ~D0/RX0   XXX
                      A6/D60 | Analog                               | 
                      A7/D61 | Analog                       Digital | D14/TX3   
                             |                              Digital | D15/RX3   
                      A8/D62 | Analog                       Digital | D16/TX2
                      A9/D63 | Analog                       Digital | D17/RX2
                      A10/D64| Analog                       Digital | D18/TX1
                      A11/D65| Analog                       Digital | D19/RX1
                      A12/D66| Analog             Digital Input/I2C | D20/SDA
                      A13/D67| Analog             Digital Input/I2C | D21/SCL
                      A14/D68| Analog                               |
                      A15/D69| Analog                               |
                              ______________________________________
                               G D D D D D D D D D D D D D D D D 5 
                               N 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 V
                               D 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2
                        
                               G D D D D D D D D D D D D D D D D 5 
                               N 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 V
                               D 3 1 9 7 5 3 1 9 7 5 3 1 9 7 5 3
