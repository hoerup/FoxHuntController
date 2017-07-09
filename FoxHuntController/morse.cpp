
#include "morse.h"

#include <Arduino.h>


//Build a struct with the morse code mapping
static const char* MorseMap[] =
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


Morse::Morse(short pin) {  
  outpin = pin;

  this->setDitLength( 200 );
  
  pinMode(outpin, OUTPUT);
  digitalWrite(outpin, LOW);
}


void Morse::setDitLength(int len) {
  if (len < 50) //len may not be lower than 100
    len = 50;

  if (len > 2000) //len may bot me larger than 2000
    len = 2000;
      
  ditlength = len;

  charInterval = ditlength * 5;
  wordInterval = ditlength * 7;
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
  digitalWrite(outpin, HIGH);
  delay(ms);  
  digitalWrite(outpin, LOW);
  delay(wordInterval);  
}

void Morse::sendDit() {
  digitalWrite(outpin, HIGH);
  delay(ditlength);  
  digitalWrite(outpin, LOW);
  delay(ditlength);
}

void Morse::sendDah() {
  digitalWrite(outpin, HIGH);
  delay(ditlength * 3);  
  digitalWrite(outpin, LOW);
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

