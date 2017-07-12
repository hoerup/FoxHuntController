#ifndef __SMSHANDLER_H__
#define __SMSHANDLER_H__


class SmsHandler {
  public:
    SmsHandler();
    void readGps();
    void handleSms();
    void init();
    
  private:

    void parseSms();
    void sendStatusReply();
    void debugPrintGps();

    unsigned long millisLastRead;
};



#endif // __SMSHANDLER_H__
