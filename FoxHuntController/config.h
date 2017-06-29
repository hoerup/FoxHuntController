#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <SoftwareSerial.h>

#define SOFTRX 2
#define SOFTTX 3
#define GSM_READY_PIN 4
#define MORSE_PIN 5

#define GSM_SHIELD_POWERCTRL 12 // This one cant be moved


extern SoftwareSerial debugSerial;

#endif // __CONFIG_H__
