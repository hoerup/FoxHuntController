# FoxHuntController for Arduino

This project is for controlling a ham radio "fox" for a "fox-hunt". 

The fox will persist of an arduino uno + a DFrobot sim808 gsm/gps shield. The  shield is both for receiving a time signal and for remote control of the fox.


## SMS Commands

The following SMS commands are supported (commands are case-insensitive)

STATUS
Returns a status message with: 
* Fox: fox number/fox letter
* Ho: hardware on/off switch - (currently unused)
* So: SMS on/off
* Dit: (it-length / dot-length
* T: Local time in UTC
* Loc: location latitude,longitude
* Int:  transmission interval in minutes (5/10)
* Period: period the fox is allowed to transmit (eg 0800-2200)

ON
Enable transmission 

OFF
Disable transmission

I:
Sets the transmission interval (5 and 10 are allowed). Eg. 
I:5   to set 5 minute interval
