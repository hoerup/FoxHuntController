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

unsigned long lastMorseTransmission = 0;
unsigned long lastTimePrint = 0;


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
  

  globalVolatile.lastTimeUpdate = 0;



  smsHandler.init(); 
}


void loop() {

  // Vi kan godt aflæse ditital inputs hver loop cycle - det  tager ikke mange ms.
  globalVolatile.foxNumber = ( digitalRead(PIN_FOXNO_2) << 2) &  ( digitalRead(PIN_FOXNO_1) << 1) &  digitalRead(PIN_FOXNO_0);
  globalVolatile.onHw = digitalRead(PIN_HW_ONOFF);
  globalVolatile.foxChar = senderId[ globalVolatile.foxNumber ];

  smsHandler.readGps(); //gps aflæsning skal køre ofte, bl.a. for at rydde input køen !
  
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
  if (globalVolatile.lastTimeUpdate  == 0) {
    return;
  }

  Time currentTime = getCorrectedTime();


  //early exit if it's not this fox's transmit time
  if ( (currentTime.minute % globalConfiguration.transmitInterval) != globalVolatile.foxNumber) {
    return;
  }

  //don't transmit if we are outside allowed transmit timeslot
  short tmpCurrentTime = (currentTime.hour * 100) + currentTime.minute;
  if (tmpCurrentTime < globalConfiguration.startTime || tmpCurrentTime > globalConfiguration.stopTime) {
    return;
  }

  // there must go a whole minute between transmissions
  if ( ( millis() - lastMorseTransmission) <= 60000 ) {
    return;
  }

  if (currentTime.second >= 59) { //bugfix no transmission at 59th second (prevent double transmisison)
    return;
  }
  lastMorseTransmission = millis();

  printTime(currentTime);
  

  char call[10];
  sprintf(call, "OZ7FOX%c", globalVolatile.foxChar);
  morse.setMessage(call);  
  debugSerial.print( F("Sending morse, call=") );
  debugSerial.println(call);
  


  long startTime = millis();
  morse.sendMorse();//første sending af call
  long stopTime = millis();

  long elapsed = stopTime-startTime;
  int bearingLength = (50000L - (3*elapsed) ) / 4; // samlet udsendelse skal vare 50 ca sekunder - så for at finde længden af pejlestreg tages samlet tid og fratrækkes 3x kaldesignal. Den resterende mængde deles i 4 (4 pejlestreger)
  
  debugSerial.print( F("elapsed=") );
  debugSerial.println(elapsed);
  debugSerial.print( F("bearingLength=") );
  debugSerial.println(bearingLength);
  
  
  sendBearingSignal(bearingLength);//Pejlestreg
  sendBearingSignal(bearingLength);//Pejlestreg
  morse.sendMorse(); //midterste sending af call
  
  sendBearingSignal(bearingLength);//Pejlestreg
  sendBearingSignal(bearingLength);//Pejlestreg
  morse.sendMorse(); //sidste sending af call


  debugSerial.println( F("Done sending morse") );
}

void sendBearingSignal(int ms) {
  long start = millis();
  digitalWrite(PIN_MORSE, HIGH);

  while ( (start+ms) >  millis() ) {
    smsHandler.readGps();
  }

  digitalWrite(PIN_MORSE, LOW);
  delay( morse.getCharInterval() );
}


Time getCorrectedTime() {
  
  Time time;
  
  long elapsed = millis() - globalVolatile.lastTimeUpdate;
  elapsed /= 1000; //ikke intereseret i millisekunder
  
  short elapsHour = elapsed / (60*60);
  short elapsMin = (elapsed/60) % (60);
  short elapsSec = elapsed % 60;

  time.second = globalVolatile.timeSecond + elapsSec;  
  time.minute = globalVolatile.timeMinute + elapsMin;
  time.hour = (globalVolatile.timeHour + elapsHour) % 24;

  //overskydende sekunder
  time.minute += time.second / 60;
  time.second %= 60;

  //overskydende minutter
  time.hour += time.minute / 60;
  time.minute %= 60;

  time.hour %= 24;

  if ( elapsed > 2 && (millis() - lastTimePrint)  >= 1000) {
    printTime(time);
    lastTimePrint = millis();
  }

    
  return time;
}


void printTime(Time& time) {
  debugSerial.print( F("Time: "));
  debugSerial.print( time.hour);
  debugSerial.print( F(":") );
  debugSerial.print( time.minute);
  debugSerial.print( F(":") );
  debugSerial.println( time.second);
}

