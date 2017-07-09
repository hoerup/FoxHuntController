
#ifndef __MORSE_H__
#define __MORSE_H__

class Morse {

  public:
    Morse(int pin);
    void setDitLength(int len);
    void setMessage(const char* msg);

    void sendLongSignal(short ms);

    void sendMorse();
    

  private:
    void sendDit();
    void sendDah();
    void sendDitAndDah( const char* ditdah);    
    void sendChar(char ch);
  
    short outpin;
    
    short ditlength;    
    short charInterval;
    short wordInterval;
    
    
    const char* message;
  
};


#endif // __MORSE_H__
