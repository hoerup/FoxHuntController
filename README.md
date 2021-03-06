# FoxHuntController for Arduino

This project is for controlling a ham radio "fox" for a "fox-hunt". 

The fox will persist of an arduino uno + a DFrobot sim808 gsm/gps shield. The  shield is both for receiving a time signal and for remote control of the fox.

## Transmission
The signal that is transmitted, is build like this:

* Call signal
* 2 extra fox ID characters
* Bearing signal
* 4 x fox ID character
* Bearing signal
* Call signal
* 2 extra fox ID characters

## SMS Commands

The following SMS commands are supported (commands are case-insensitive)

`STATUS`
Returns a status message

`ENABLETX`
Enable transmission. Transmission is allowed. Enable/disable tx is a master-switch for turning transmission of on.

`DISABLETX`
Disable transmission. No transmission is allowed

`ENSCHED`
Enable schedule. If enabled the fox will only transmit within the allowed schedule / timeslot.

`DISCHED`
Disable schedule. If disabled the fox will transmit with no consideration to configured schedule/timeslot. 

`PING`
Sends 3 bearing signals, each lasting 15 seconds

`I:`
Sets the transmission interval (5 and 10 are allowed). Eg. `I:5`   to set 5 minute interval

`A:`
Sets the allowed transmission timeslot/schedule. A:<starttime>-<stoptime> Eg. `A:0800-2230` configures the fox to not transmit before 08:00 and not after 22:30. 
Allowed timeslot/schedule can not cross midnight. **Please note that all times are in UTC**.

`DIT:`
Sets the dit/dot length in milliseconds. Allowed interval is 20-140 (inclusive). 
Eg. `DIT:120` sets the ditlength to 120 milliseconds

`FOX:`
Sets the fox number. 0-7 are arllowed (both inclusive).
Eg. `FOX:4` sets the foxnumber to 4

## Status message
The `status` message contains these elements:
* Fox: fox number/fox letter
* TX: Transmission on/off
* Dit: (it-length / dot-length
* T: Local time in UTC
* Loc: location latitude,longitude
* Int:  transmission interval in minutes (5/10)
* Period: period the fox is allowed to transmit (eg 0800-2200)
