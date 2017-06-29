
#ifndef __MORSE_H__
#define __MORSE_H__

class Morse {

  public:
    Morse(int pin);
    void setDitLength(int len);
    

  private:
    int outpin;
    int ditlength;    
    
  
};


#endif // __MORSE_H__
