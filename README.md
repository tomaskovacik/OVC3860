# arduino library for OC3860 based modules

This directory contain all files needed to support A2DP bluetooth module based on OmniVision chip OVC3860.

OVC3860 have AT command control support, supported commands are descriped <a href="https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/BLK-MD-SPK-B_AT_Command_set_original.pdf">here</a> and <a href="https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/OVC3860_AT_Command_Application_Notes.pdf">here</a> (some did not work on my modules...).

# how to use it

<a href="https://www.arduino.cc/en/Guide/Libraries">Information about using libraries on arduino site</a>

Copy content of this repository directory or just this two files OVC3860.cpp and OVC3860.h to ~/Arduino/libraries/OVC3860/ directory (if did not exist, create one). Open new project in arduino and use this code, or check code in examples directory examples/OVC3860/OVC3860.ino:

```c
#include "OVC3860.h"
#include <SoftwareSerial.h> //if using SW, with HW no need for this

#define resetBTPin 5
 
SoftwareSerial swSerial(7, 6); //rxPin, txPin

OVC3860 BT(&swSerial, resetBTPin); //in case of HW serial use for example: (&Serial1)

void(){
  BT.begin(); //or BT.begin(9600); for specific baudrate
}

void loop(){
//should be call periodically, to get notifications from module, for example if someone calling...
BT.getNextEventFromBT();
}
```

for more examples look ate examples/OVC3860/OVC3860.ino

Do not forget: getNextEventFromBT function must be called periodicaly, it's run from other function in library to catch responce to commands send to module, but to catch mesages from module which are not expected (incomming call for example) this must be fired periodicaly.

# supported functions

<a href="https://github.com/tomaskovacik/OVC3860/wiki/supported-functions">wiki page about supported functions</a>

# how shoud main code act based on module state

library set variables which can be check, compared etc and based on these main code should react, here is a <a href="">page on wiki talking about it</a>.
