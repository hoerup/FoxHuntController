
#include "morse.h"

#include <Arduino.h>


//Build a struct with the morse code mapping
const char* const MorseMap[] =
{
  ".-" , //A
  "-..." , //B
  "-.-." , //C
  "-.." , //D
  "." , //E
  "..-." , //F
  "--." , //G
  "...." , //H
  ".." , //I
  ".---" , //J
  ".-.-" , //K
  ".-.." , //L
  "--" , //M
  "-." , //N
  "---" , //O
  ".--." , //P
  "--.-" , //Q
  ".-." , //R 
  "..." , //S
  "-" , //T
  "..-", //U
  "...-", //V
  ".--", //W
  "-..-", //X
  "-.--", //Y
  "--..", //Z

  "-----", //0
  ".----", //1
  "..---", //2
  "...--", //3
  "....-", //4
  ".....", //5
  "-....", //6
  "--...", //7
  "---..", //8
  "----." //9
};


int x = 0b0100;


Morse::Morse(short pin) {  
  outpin = pin;
  
  pinMode(outpin, OUTPUT);
  digitalWrite(outpin, HIGH);
}


void Morse::setDitLength(int len) {
  if (len < 40) //len may not be lower than 100
    len = 40;

  if (len > 150) //len may bot me larger than 2000
    len = 150;
      
  ditlength = len;

  charInterval = ditlength * 5;
  wordInterval = ditlength * 7;
}

short Morse::getDitLength() {
  return ditlength;
}

short Morse::getCharInterval() {
  return charInterval;
}

void Morse::setMessage(const char* msg) {
  message = msg;
}


void Morse::sendMorse() {
  int len = strlen(message);
  
  for (int i=0; i<len; i++)  {
    sendChar( message[i] );
    delay( charInterval ); 
  }
  
  delay( wordInterval );  //long delay after transmission
}


void Morse::sendLongSignal(short ms) {
  digitalWrite(outpin, LOW);
  delay(ms);  
  digitalWrite(outpin, HIGH);
  delay(wordInterval);  
}

void Morse::sendDit() {
  digitalWrite(outpin, LOW);
  delay(ditlength);  
  digitalWrite(outpin, HIGH);
  delay(ditlength);
}

void Morse::sendDah() {
  digitalWrite(outpin, LOW);
  delay(ditlength * 3);  
  digitalWrite(outpin, HIGH);
  delay(ditlength);
}

void Morse::sendDitAndDah( const char* ditdah) {
  short len = strlen(ditdah);
  for(short i=0; i<len; i++) {
    if( ditdah[i] == '.') {
      sendDit(); 
    } else {
      sendDah();
    }
  }  
}

void Morse::sendChar(char ch) {
  ch = toupper(ch);

  
  if (ch >= 'A' && ch <= 'Z') {
    short pos = ch - 'A';
    sendDitAndDah( MorseMap[pos] );
  } else if (ch >= '0' && ch <= '9' ) {
    short pos = ch - '0';
    pos += 26; 
    sendDitAndDah( MorseMap[pos] );
  } else {
    delay( ditlength *5);
  }

  

}

