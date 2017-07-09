/*
 * FoxHuntController
 * 
 * Developed for arduino uno with DFRobot SIM808 GSM shield
 * 
 * Pin0 & Pin1 (RX/TX) are reserved for communication between arduino and the GSM chip
 * Pin12 is reserved for controlling gsm shield powerup/down
 * 
 * Pin2 - readyLED - used to indicate that GSM is ready
 * Pin8 - radiocontrol - used to send morse signal to radio
 * 
 * 
 * Make sure you have installed DFRobot_SIM808 from https://github.com/DFRobot/DFRobot_SIM808/
 */


#include "morse.h"
#include "smshandler.h"

#include "config.h"



Morse morse(PIN_MORSE);

SmsHandler smsHandler;


SoftwareSerial debugSerial(PIN_SOFTRX, PIN_SOFTTX); //defined "extern" in config.h
FoxConfig globalConfiguration; //defined "extern" in config.h

char senderId[] = { 'A', 'U', 'V', 'H', '5', 'N', 'D', 'B' };


void setup() {
  Serial.begin(9600);
  debugSerial.begin(9600);

  debugSerial.println("FoxHunt booting");

  pinMode(PIN_FOXNO_0, INPUT);
  pinMode(PIN_FOXNO_1, INPUT);
  pinMode(PIN_FOXNO_2, INPUT);

  pinMode(PIN_HW_ONOFF, INPUT);



  smsHandler.init();
  
  globalConfiguration.onSms = 1; //default on, until turned off    
}


void loop() {

  // Vi kan godt aflæse ditital inputs hver loop cycle - det  tager ikke mange ms.
  globalConfiguration.foxNumber = ( digitalRead(PIN_FOXNO_2) << 2) &  ( digitalRead(PIN_FOXNO_1) << 1) &  digitalRead(PIN_FOXNO_0);
  globalConfiguration.onHw = digitalRead(PIN_HW_ONOFF);
  
  smsHandler.handleSms();
  morseController();
}


//ToDo: Denne funtion skal holde øje med tid + afsendelses interval
//      og såfremt det er tid til afsendelse skal den finde den rigtige besked og af sende den 
void morseController() {
  
  if (globalConfiguration.onHw == 0 || globalConfiguration.onSms == 0)  { //this fox has been shut off by switch or sms
    return;
  }
  

  //early exit hvis det ikke er tid til udsendelse

  char call[10];
  sprintf(call, "OZ7FOX%c", senderId[ globalConfiguration.foxNumber ]);
  morse.setMessage(call);  
  debugSerial.print("Sending morse, call=");
  debugSerial.println(call);
  
  
  morse.sendMorse();
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendMorse();
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendLongSignal(10000);//Pejlestreg
  morse.sendMorse();

  
  
  
  
}

