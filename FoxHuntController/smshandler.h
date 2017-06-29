#ifndef __SMSHANDLER_H__
#define __SMSHANDLER_H__


class SmsHandler {
  public:
    SmsHandler();

    void handleSms();
    void init();
    
  private:

    void parseSms();
    void debugPrintGps();

    unsigned long millisLastRead;
};



#endif // __SMSHANDLER_H__
