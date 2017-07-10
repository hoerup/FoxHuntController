/*
 * FoxHuntController
 * 
 * Developed for arduino uno with DFRobot SIM808 GSM shield
 * 
 * For pin assignments please refer to config.h
 * 
 * Make sure you have installed DFRobot_SIM808 from hoerup's fork of DFRobot_SIM808 https://github.com/hoerup/DFRobot_SIM808/ (at least until the enhancements are accepted into main codebase)
 */

#include <EEPROM.h>

#include "morse.h"
#include "smshandler.h"

#include "config.h"



Morse morse(PIN_MORSE);

SmsHandler smsHandler;


SoftwareSerial debugSerial(PIN_SOFTRX, PIN_SOFTTX); //defined "extern" in config.h
FoxConfig globalConfiguration; //defined "extern" in config.h
VolatileData globalVolatile; //defined "extern" in config.h

const char senderId[] = { 'A', 'U', 'V', 'H', '5', 'N', 'D', 'B' };


void setup() {
  Serial.begin(9600);
  debugSerial.begin(9600);

  debugSerial.println( F("FoxHunt booting") );

  pinMode(PIN_FOXNO_0, INPUT);
  pinMode(PIN_FOXNO_1, INPUT);
  pinMode(PIN_FOXNO_2, INPUT);

  pinMode(PIN_HW_ONOFF, INPUT);




  

  EEPROM.get(0, globalConfiguration);
  if (globalConfiguration.transmitInterval == 255) { //initially EEPROM is set to 0xFF
    globalConfiguration.transmitInterval = 10;
    globalConfiguration.startTime = 0;
    globalConfiguration.stopTime = 2359;

    EEPROM.put(0, globalConfiguration);
  }
  

  globalVolatile.currentTime = 0;



  smsHandler.init();
}


void loop() {

  // Vi kan godt aflæse ditital inputs hver loop cycle - det  tager ikke mange ms.
  globalVolatile.foxNumber = ( digitalRead(PIN_FOXNO_2) << 2) &  ( digitalRead(PIN_FOXNO_1) << 1) &  digitalRead(PIN_FOXNO_0);
  globalVolatile.onHw = digitalRead(PIN_HW_ONOFF);
  
  smsHandler.handleSms();
  morseController();
}


//ToDo: Denne funtion skal holde øje med tid + afsendelses interval
//      og såfremt det er tid til afsendelse skal den finde den rigtige besked og af sende den 
void morseController() {
  
  if (globalVolatile.onHw == 0 || globalConfiguration.onSms == 0)  { //this fox has been shut off by switch or sms
    return;
  }

  //no transmission if we don't have a time reading
  if (globalVolatile.currentTime  == 0) {
    return;
  }

  //early exit if it's not this fox's transmit time
  long tmpCurrentTime = globalVolatile.currentTime / 100;
  if ( (tmpCurrentTime % globalConfiguration.transmitInterval) != globalVolatile.foxNumber) {
    return;
  }

  //don't transmit if we are outside allowed transmit timeslot
  if (tmpCurrentTime < globalConfiguration.startTime || tmpCurrentTime > globalConfiguration.stopTime) {
    return;
  }
  

  char call[10];
  sprintf(call, "OZ7FOX%c", senderId[ globalVolatile.foxNumber ]);
  morse.setMessage(call);  
  debugSerial.print( F("Sending morse, call=") );
  debugSerial.println(call);
  

  morse.sendMorse();
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendMorse();
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendMorse();
  
  
}

