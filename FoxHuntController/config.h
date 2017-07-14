#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <SoftwareSerial.h>


#define PIN_SOFTRX 2 // rx fra debug rs232 shield 
#define PIN_SOFTTX 3 // tx til debug rs232 shield 
#define PIN_GSM_READY 4 // LED out til at indikere at vi har gsm signal
#define PIN_MORSE 5 // 


//3 hardware pins to make a 3bit digital input defining fox number
#define PIN_FOXNO_0 6 // least significant bit
#define PIN_FOXNO_1 7
#define PIN_FOXNO_2 8 // most significant bit

#define PIN_HW_ONOFF 9


#define PIN_GSM_SHIELD_POWERCTRL 12 // This one cant be moved

// contains configuration data that should be saved
struct FoxConfig {
  unsigned short onSms : 1;

  unsigned short startTime;
  unsigned short stopTime;

  unsigned char transmitInterval = 5;
};

//contains data that should be globally available - but due to its dynamic properties shouldn't be saved to eeprom
struct VolatileData {
  unsigned short onHw : 1;
  unsigned short foxNumber : 3;
  unsigned char foxChar;


  unsigned long lastTimeUpdate = 0;  
  short timeHour;
  short timeMinute;
  short timeSecond;
};

struct Time {
  short hour;
  short minute;
  short second;
};

Time getCorrectedTime();

extern FoxConfig globalConfiguration;
extern VolatileData globalVolatile;
extern SoftwareSerial debugSerial;

#endif // __CONFIG_H__
