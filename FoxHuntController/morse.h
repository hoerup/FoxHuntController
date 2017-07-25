
#ifndef __MORSE_H__
#define __MORSE_H__

class Morse {

  public:
    Morse(short pin);
    void setDitLength(int len);
//    void setMessage(const char* msg);

    void sendLongSignal(short ms);

    void sendMorse(const char* message);

    short getDitLength();
    short getCharInterval();
    

  private:
    void sendDit();
    void sendDah();
    void sendDitAndDah( const char* ditdah);    
    void sendChar(char ch);
  
    short outpin;
    
    short ditlength;    
    short charInterval;
    short wordInterval;
      
};


#endif // __MORSE_H__
