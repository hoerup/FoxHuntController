/*
 * FoxHuntController
 * 
 * Developed for arduino uno with DFRobot SIM808 GSM shield
 * 
 * Pin0 & Pin1 (RX/TX) are reserved for communication between arduino and the GSM chip
 * Pin12 is reserved for controlling gsm shield powerup/down
 * 
 * Pin2 - readyLED - used to indicate that GSM is ready
 * Pin8 - radiocontrol
 */

#include <DFRobot_sim808.h>

#include "morse.h"

DFRobot_SIM808 sim808(&Serial);

#define GSM_READY_PIN 2
#define GSM_SHIELD_POWERCTRL 12

void setup() {
  Serial.begin(9600);

  pinMode(GSM_READY_PIN, OUTPUT);
  pinMode(GSM_SHIELD_POWERCTRL, OUTPUT);

  sim808.powerUpDown(GSM_SHIELD_POWERCTRL);
 
 // Initialize sim808 module 
  while(!sim808.init()) {
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }  

  digitalWrite(GSM_READY_PIN, HIGH);

  
}

void loop() {
  // put your main code here, to run repeatedly:
}
