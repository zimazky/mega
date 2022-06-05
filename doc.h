/*
 *    FILE: doc.h
 *  AUTHOR: Andrew Zimazky
 * VERSION: 0.0.1
 * PURPOSE: Документация по пинам и схема датчика слежения за температурой
 * Arduino UNO
 * PINs using
 * 
 * DIGITAL
 * 
 * 0  RX    (PCINT16/RXD)        XXX
 * 1  TX    (PCINT17/TXD)        XXX
 * 2        (PCINT18/INT0)       Sensor2 Data					Green
 * 3  [PWM] (PCINT19/OC2B/INT1)  Sensor2 Led					Blue
 * 4        (PCINT20/XCK/T0)     Eth SS for SD-card
 * 5  [PWM] (PCINT21/OC0B/T1)    Sensor2 Button				Blue/White
 * 6  [PWM] (PCINT22/OC0A/AIN0)  Sensor3 Data         Green
 * 7        (PCINT23/AIN1)       Sensor1 Button				Blue/White
 * 8        (PCINT0/CLKO/ICP1)   Sensor1 Led					Blue
 * 9  [PWM] (PCINT1/OC1A)        Sensor1 Data					Green
 * 10 [PWM] (PCINT2/OC1B/SS)     Eth SS for Ethernet controller
 * 11 [PWM] (PCINT3/OC2A/MOSI)   Eth MOSI
 * 12       (PCINT4/MISO)        Eth MISO
 * 13       (PCINT5/SCK)         Eth SCK
 * 
 * ANALOG IN
 * 
 * 5V										                                    Orange
 * GND										                                  Brown
 * A0 (PCINT8/ADC0)               Реле1		                  Orange/White
 * A1 (PCINT9/ADC1)               Реле2		                  Green/White
 * A2 (PCINT10/ADC2)              НК3022 (Датчик давления)  Green
 * A3 (PCINT11/ADC3)			        PumpPower                 Blue/White
 * A4 (PCINT12/SDA/ADC4)		      ---		                    Blue
 * A5 (PCINT13/SCL/ADC5)		      ---		
 * 
 * MY TEMPERATURE SENSOR
 * 
 * Orange 1               5V ---*-------*
 * 			                        |       |
 * 			                      R1 10k    |
 * 			                        |       *-------- DHT22-1
 * Green 	2  DataPin(In/Out) ---*---------------- DHT22-2
 * Brown	3              GND ------------------*- DHT22-4
 *  		                                       |
 * Blue		4      LEDPin(Out) -- LED -- R2 1k --*
 * 			                                       |
 * Blue/W	5 BtnPin(InPullUp) -- Btn -----------*
 * 			                     (Open-High,Closed-Low)
 * 
 */
