#include "smshandler.h"
#include "config.h"

#include <DFRobot_sim808.h>
#include <EEPROM.h>

#define MESSAGE_LENGTH 80 // ja SMS kan teknisk set være længere - men kommando SMS'erne er væsentlig kortere

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
  pinMode(PIN_GSM_SHIELD_POWERCTRL, OUTPUT);
  
  digitalWrite(PIN_GSM_READY, LOW);

  
  if (! sim808.checkPowerUp() ) {
    debugSerial.println( F("Sending Sim808 boot signal") );
    sim808.powerUpDown(PIN_GSM_SHIELD_POWERCTRL);
  }

 // Initialize sim808 module 
  while(!sim808.init()) {
      delay(1000);
      debugSerial.print( F("Sim808 init error\r\n") );
  }  
  debugSerial.println( F("Sim808 Ready") );


  //************* Turn on the GPS power************
  if( sim808.attachGPS())
      debugSerial.println( F("Open the GPS power success") );
  else 
      debugSerial.println( F("Open the GPS power failure") );

  /*debugSerial.println("Deleting old sms");
  for (int i=1; i<=9; i++) {
    sim808.deleteSMS(i);  
  }*/

  digitalWrite(PIN_GSM_READY, HIGH);

}
void SmsHandler::readGps() {
   
  if (sim808.getGPS()) {    
    debugPrintGps();
    globalVolatile.currentTime = (sim808.GPSdata.hour * 10000L) + (sim808.GPSdata.minute * 100L) + sim808.GPSdata.second;    
  }
  
}

void SmsHandler::handleSms() {
 

  //da GPS'en smider data afsted fortløbende  på seriel porten
  //skal vi sikre at SMS aflæsning ikke ødelægger seneste aflæsning
  if ( abs( millis() - millisLastRead) < 15000 ) {
    return;
  }
  millisLastRead = millis();

  
  sim808.stopGpsDataflow();
  sim808_flush_serial();

  
  
  int messageIndex = sim808.isSMSunread();


  //*********** At least, there is one UNREAD SMS ****-*******
  if (messageIndex > 0) { 
    debugSerial.print( F("messageIndex: ") );
    debugSerial.println(messageIndex);    
    
    sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
    
                 
    //***********In order not to full SIM Memory, is better to delete it**********
    sim808.deleteSMS(messageIndex);    


    parseSms();
  }

  sim808.startGpsDataflow();
}


void SmsHandler::parseSms() {
    debugSerial.print( F("From number: ") );
    debugSerial.println(phone);  
    debugSerial.print( F("Datetime: ") );
    debugSerial.println(datetime);        
    debugSerial.print( F("Recieved Message: ") );
    debugSerial.println(message);      

    char tmpstr[5];
    

    for (int i=0; i<strlen(message); i++) { //in-place str-to-upper
      message[i] = toupper(message[i]); 
    }

    
    if (strncmp("STATUS", message, 6) == 0) {
      sendStatusReply();
      return;
    }

    if (strcmp("ON", message) == 0) {
      globalConfiguration.onSms = 1;
      EEPROM.put(0, globalConfiguration);
      sim808.sendSMS(phone, "Ok");  
      return;
    }

    if (strcmp("OFF", message) == 0) {
      globalConfiguration.onSms = 0;
      EEPROM.put(0, globalConfiguration);
      sim808.sendSMS(phone, "Ok");  
      return;
    }

    // handling of start-stop time
    // format "A:0800-2300"
    if (strncmp("A:", message, 2) == 0) {
      if (strlen(message) != 11) {
        sim808.sendSMS(phone, "Error, inval length");    
        return ;
      }
      strncpy(tmpstr, message+2, 4);
      tmpstr[4] = 0;
      unsigned short tmpStart = atoi(tmpstr);
      if (tmpStart == 0) {
        sim808.sendSMS(phone, "Error parsing start");    
        return ;        
      }

      strncpy(tmpstr, message+7, 4);
      tmpstr[4] = 0;
      unsigned short tmpStop = atoi(tmpstr);
      if (tmpStop == 0) {
        sim808.sendSMS(phone, "Error parsing stop");    
        return ;        
      }

      globalConfiguration.startTime = tmpStart;
      globalConfiguration.stopTime = tmpStop;
      EEPROM.put(0, globalConfiguration);
      
      sim808.sendSMS(phone, "Ok");  
      return;
    }

    //handle set transmission interval
    if (strncmp("I:", message, 2) == 0) {
      strncpy(tmpstr, message+2, 4);
      tmpstr[4] = 0;
      unsigned short tmpInterval = atoi(tmpstr);

      if (tmpInterval != 5 && tmpInterval != 10) {
        sim808.sendSMS(phone, "Error, invalid interval (5/10)");    
        return ;  
      }
      globalConfiguration.transmitInterval = tmpInterval;
      EEPROM.put(0, globalConfiguration);
      
      sim808.sendSMS(phone, "Ok");  
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

  sprintf(reply, "Fox:%i/%c Ho:%i So:%i T:%02d:%02d:%02d(utc) Loc:%s,%s Int:%d Period:%04d-%04d", 
      globalVolatile.foxNumber, globalVolatile.foxChar,      
      globalVolatile.onHw, globalConfiguration.onSms, 
      sim808.GPSdata.hour, sim808.GPSdata.minute, sim808.GPSdata.second,
      lat,lon,
      globalConfiguration.transmitInterval,
      globalConfiguration.startTime, globalConfiguration.stopTime
      
      );
      
  sim808.sendSMS(phone, reply);      
      
}

void SmsHandler::debugPrintGps() {
  debugSerial.print(sim808.GPSdata.year);
  debugSerial.print( F("/") );
  debugSerial.print(sim808.GPSdata.month);
  debugSerial.print( F("/") );
  debugSerial.print(sim808.GPSdata.day);
  debugSerial.print( F(" ") );
  debugSerial.print(sim808.GPSdata.hour);
  debugSerial.print( F(":") );
  debugSerial.print(sim808.GPSdata.minute);
  debugSerial.print( F(":") );
  debugSerial.print(sim808.GPSdata.second);
  debugSerial.print( F(":") );
  debugSerial.println(sim808.GPSdata.centisecond);
  debugSerial.print( F("latitude :") );
  debugSerial.println(sim808.GPSdata.lat, 8);
  debugSerial.print( F("longitude :") );
  debugSerial.println(sim808.GPSdata.lon, 8);
  debugSerial.print( F("speed_kph :") );
  debugSerial.println(sim808.GPSdata.speed_kph);
  debugSerial.print( F("heading :") );
  debugSerial.println(sim808.GPSdata.heading);
  debugSerial.println();
}

