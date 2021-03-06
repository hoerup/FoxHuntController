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


//const char* callsign = "OZ7FOX";
//const char* callsign = "OZ17SJ";

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


  

  EEPROM.get(0, globalConfiguration);
  if (globalConfiguration.transmitInterval == 255) { //initially EEPROM is set to 0xFF
    globalConfiguration.transmitInterval = 10;
    globalConfiguration.startTime = 0;
    globalConfiguration.stopTime = 2359;

    globalConfiguration.ditLength = 100;

    EEPROM.put(0, globalConfiguration);
  }
  

  globalVolatile.lastTimeUpdate = 0;
  globalVolatile.sendBearing = 0;

  morse.setDitLength( globalConfiguration.ditLength );


  smsHandler.init(); 
}


void loop() {

  globalVolatile.foxChar = senderId[ globalConfiguration.foxNumber ];


  smsHandler.readGps(); //gps aflæsning skal køre ofte, bl.a. for at rydde input køen !
  
  smsHandler.handleSms();
  morseController();
}


//ToDo: Denne funtion skal holde øje med tid + afsendelses interval
//      og såfremt det er tid til afsendelse skal den finde den rigtige besked og af sende den 
void morseController() {

  if (globalVolatile.sendBearing == 1) {
    globalVolatile.sendBearing = 0;

    sendBearingSignal(15000);
    sendBearingSignal(15000);
    sendBearingSignal(15000);

    return;
  }
  
  if (globalConfiguration.enableTx == 0)  { //this fox has been shut off by sms
    return;
  }

  //no transmission if we don't have a time reading
  if (globalVolatile.lastTimeUpdate  == 0) {
    return;
  }

  Time currentTime = getCorrectedTime();


  //early exit if it's not this fox's transmit time
  if ( (currentTime.minute % globalConfiguration.transmitInterval) != globalConfiguration.foxNumber) {
    return;
  }

  if (globalConfiguration.enableSched == 1) {
    //don't transmit if we are outside allowed transmit timeslot
    short tmpCurrentTime = (currentTime.hour * 100) + currentTime.minute;
    if (tmpCurrentTime < globalConfiguration.startTime || tmpCurrentTime > globalConfiguration.stopTime) {
      return;
    }
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
  

  char mid[3];
  char call[10];

  sprintf(mid, "%c%c%", globalVolatile.foxChar, globalVolatile.foxChar);  
  sprintf(call, "OZ17SJ%c", globalVolatile.foxChar);
 
  debugSerial.print( F("Sending morse, call=") );
  debugSerial.println(call);
  debugSerial.print( F("start=") );
  debugSerial.println(mid);
  

  /////////////////////////////////////////
  //første sending af call
  long startTime = millis();    
  morse.sendMorse(call);
  morse.sendMorse(mid);  
  long stopTime = millis();

  long elapsed = stopTime-startTime;
  int bearingLength = (50000L - (3*elapsed) ) / 2; // samlet udsendelse skal vare 50 ca sekunder - så for at finde længden af pejlestreg tages samlet tid og fratrækkes 3x kaldesignal. Den resterende mængde deles i 2 (2 pejlestreger)


  
  debugSerial.print( F("elapsed=") );
  debugSerial.println(elapsed);
  debugSerial.print( F("bearingLength=") );
  debugSerial.println(bearingLength);
  
  if (bearingLength < 1000) {  //ditlength=140 giver en lille negativ bearing Length - så vi overstyrer og siger mindst 1 sek
    bearingLength = 1000;
  }
  

  sendBearingSignal(bearingLength);//Pejlestreg

  /////////////////////////////////////////
  // midterste sending  
  morse.sendMorse(mid);
  morse.sendMorse(mid);  

  
  sendBearingSignal(bearingLength);//Pejlestreg

  /////////////////////////////////////////
  //sidste sending af call    
  morse.sendMorse(call);  
  morse.sendMorse(mid);


  debugSerial.println( F("Done sending morse") );
}

void sendBearingSignal(int ms) {
  long start = millis();
  digitalWrite(PIN_MORSE, LOW);

  while ( (start+ms) >  millis() ) {
    smsHandler.readGps();
  }

  digitalWrite(PIN_MORSE, HIGH);
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

