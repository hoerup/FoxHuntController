#include "smshandler.h"
#include "config.h"

#include <DFRobot_sim808.h>


#define MESSAGE_LENGTH 160

char message[MESSAGE_LENGTH];


char phone[16];
char datetime[24];

DFRobot_SIM808 sim808(&Serial);

SmsHandler::SmsHandler()
 :  millisLastRead(0)
{
  
}

void SmsHandler::init() {
  pinMode(PIN_GSM_READY, OUTPUT);
  digitalWrite(PIN_GSM_READY, LOW);

  
  if (! sim808.checkPowerUp() ) {
    sim808.powerUpDown(PIN_GSM_SHIELD_POWERCTRL);
  }

 // Initialize sim808 module 
  while(!sim808.init()) {
      delay(1000);
      debugSerial.print("Sim808 init error\r\n");
  }  
  debugSerial.println("Sim808 Ready");


  //************* Turn on the GPS power************
  if( sim808.attachGPS())
      debugSerial.println("Open the GPS power success");
  else 
      debugSerial.println("Open the GPS power failure");

  /*debugSerial.println("Deleting old sms");
  for (int i=1; i<=9; i++) {
    sim808.deleteSMS(i);  
  }*/

  digitalWrite(PIN_GSM_READY, HIGH);

}


void SmsHandler::handleSms() {
  //gps aflæsning skal køre ofte, bl.a. for at rydde input køen !
  if (sim808.getGPS()) {    
    debugPrintGps();
  }

  //da GPS'en smider data afsted fortløbende  på seriel porten
  //skal vi sikre at SMS aflæsning ikke ødelægger seneste aflæsning
  if ( abs( millis() - millisLastRead) < 10000 || millis() < 10000) {
    return;
  }
  millisLastRead = millis();

  
  sim808.stopGpsDataflow();
  sim808_flush_serial();

  
  
  int messageIndex = sim808.isSMSunread();


  //*********** At least, there is one UNREAD SMS ****-*******
  if (messageIndex > 0) { 
    debugSerial.print("messageIndex: ");
    debugSerial.println(messageIndex);    
    
    sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
    
                 
    //***********In order not to full SIM Memory, is better to delete it**********
    sim808.deleteSMS(messageIndex);    


    parseSms();
  }

  sim808.startGpsDataflow();
}


void SmsHandler::parseSms() {
    debugSerial.print("From number: ");
    debugSerial.println(phone);  
    debugSerial.print("Datetime: ");
    debugSerial.println(datetime);        
    debugSerial.print("Recieved Message: ");
    debugSerial.println(message);      

    for (int i=0; i<strlen(message); i++) { //in-place str-to-upper
      message[i] = toupper(message[i]); 
    }
    
    if (strncmp("STATUS", message, 6) == 0) {
      sendStatusReply();
      return;
    }


    sim808.sendSMS(phone, "Unknown Command");
}

void SmsHandler::sendStatusReply() {
  char reply[120] ;
  
  char lat[10];
  char lon[10];  
  dtostrf(sim808.GPSdata.lat, 2, 6, lat); //since sprintf doesn't support %f
  dtostrf(sim808.GPSdata.lon, 2, 6, lon);

  sprintf(reply, "Fox:%i Ho:%i So:%i T:%02d:%02d:%02d(utc) Loc:%s,%s", 
      globalConfiguration.foxNumber,globalConfiguration.onHw, globalConfiguration.onSms, 
      sim808.GPSdata.hour, sim808.GPSdata.minute, sim808.GPSdata.second,
      lat,lon
      );
      
  sim808.sendSMS(phone, reply);      
      
}

void SmsHandler::debugPrintGps() {
  debugSerial.print(sim808.GPSdata.year);
  debugSerial.print("/");
  debugSerial.print(sim808.GPSdata.month);
  debugSerial.print("/");
  debugSerial.print(sim808.GPSdata.day);
  debugSerial.print(" ");
  debugSerial.print(sim808.GPSdata.hour);
  debugSerial.print(":");
  debugSerial.print(sim808.GPSdata.minute);
  debugSerial.print(":");
  debugSerial.print(sim808.GPSdata.second);
  debugSerial.print(":");
  debugSerial.println(sim808.GPSdata.centisecond);
  debugSerial.print("latitude :");
  debugSerial.println(sim808.GPSdata.lat, 8);
  debugSerial.print("longitude :");
  debugSerial.println(sim808.GPSdata.lon, 8);
  debugSerial.print("speed_kph :");
  debugSerial.println(sim808.GPSdata.speed_kph);
  debugSerial.print("heading :");
  debugSerial.println(sim808.GPSdata.heading);
  debugSerial.println();
}

