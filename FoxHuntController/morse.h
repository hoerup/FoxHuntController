
#ifndef __MORSE_H__
#define __MORSE_H__

class Morse {

  public:
    Morse(int pin);
    void setDitLength(int len);
    void setMessage(const char* msg);


    void sendMorse();
    

  private:
    void sendDit();
    void sendDah();
    void sendDitAndDah( const char* ditdah);    
    void sendChar(char ch);
  
    int outpin;
    int ditlength;    
    const char* message;
  
};


#endif // __MORSE_H__
