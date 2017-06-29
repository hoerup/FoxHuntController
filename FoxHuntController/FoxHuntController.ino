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

#include <DFRobot_sim808.h>
#include "morse.h"
#include "smshandler.h"

#include "config.h"



Morse morse(MORSE_PIN);

SmsHandler smsHandler;
SoftwareSerial debugSerial(SOFTRX,SOFTTX);


void setup() {
  Serial.begin(9600);
  debugSerial.begin(9600);

  debugSerial.println("FoxHunt booting");


  smsHandler.init();
    
}

void loop() {
  smsHandler.handleSms();
 
}
