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
```c
begin(uint32_t baudrate = 115200); //parameter baudrate is communication speed between MCU and arduino, default 115200

sendData(String cmd); //construct string of AT+cmd and send it to serial port

getNextEventFromBT(); //parse data send from module and set internal variales, call this periodicaly, to parse data received from module ASAP

sendRawData(uint8_t _size, uint8_t data[]); //send data[] to serial port

pairingInit(); //initialize pairing mode

pairingExit(); //exit pairing mode

connectHSHF(); //conect to handsfreedevice

disconnectHSHF(); //disconnect handsfree, but it leave A2DP connected

callAnswer(); //accept incomming call

callReject(); //reject incomming call

callHangUp(); //hang up call

callRedial(); //Redial the last outgoing call

voiceDialStart(); //Voice dialing

voiceDialEnd(); //cancel the voice dialing

micToggle(); //Mute/Unmute MIC

transferAudio(); //Transfer audio between HSHF and phone speaker

callReleaseReject(); //causes the module to release held call, and reject waiting call.

callReleaseAccept(); //causes the module to release active call, accept other call.

callHoldAccept(); //causes the module to hold active call, accept other call.

callConference(); //causes the module to make a conference call

pairingDeleteThenInit(); //delete current pairing and then initialize pairing mode

callDialNumber(String number); //causes the module to dial number

sendDTMF(char c); //causes the module to send one DTMF characeter c, supported: 0-9, #, *, A-D

queryHFPStatus(); //queries the module’s HFP current status

reset(); //software reset module

musicTogglePlayPause(); //If the module is connected with a AV Source, this command causes the AV source to play/pause music. If module isn’t connected AV source, this command will cause module try to connected
  current connected mobile’s AV source.

musicStop(); //If the module is connected with a AV Source, this command causes the AV Source to Stop Music.

musicNextTrack(); //If the module is connected with a AV Source, this command causes the AV Source to Play next song.

musicPreviousTrack(); //If the module is connected with a AV Source, this command causes the AV Source to play last song.

queryConfiguration(); //query audo connect and auto answare configuration

autoconnEnable(); //configure auto connect feature to be enabled 

autoconnDisable(); //configure auto connect feature to be disabled

connectA2DP(); //connect A2DP/ Media audio

disconnectA2DP(); //disconnect A2DP / media audio

changeLocalName(String name = ""); //without parameter module should return actual name, did not work for me

changePin(String pin = ""); //without parameter module should return actual pin, did not work for me

queryAvrcpStatus();  //queries the module’s AVRCP current status

autoAnswerEnable(); //configure auto answer call feature to be enabled 

autoAnswerDisable(); //configure auto answer call feature to be disabled 

musicStartFF(); //If the module is connected with a AV Source, this command causes the AV Source to start fast forward.

musicStartRWD(); //If the module is connected with a AV Source, this command causes the AV Source to start rewind.

musicStopFFRWD(); //If the module is connected with a AV Source, this command causes the AV Source to stop fast forward or rewind.

queryA2DPStatus(); //queries the module’s A2DP current status

writeToMemory(String data); //This command causes the module to write a byte into a given memory address.

readFromMemory(String addr); //This command causes the module to read a byte from a given memory address.

switchDevices(); //causes the module to switch two remote devices

queryVersion(); //queries the module’s software version

pbSyncBySim(); //causes the module to synchronize the phonebook which is stored by SIM

pbSyncByPhone(); //causes the module to synchronize the phonebook which is stored by phone

pbReadNextItem(); //causes the module to read next one phonebook item from phone or local

pbReadPreviousItem(); //causes the module to read previous one phonebook item from phone or local

pbSyncByDialer(); //causes the module to synchronize the dialed calls list

pbSyncByReceiver(); //causes the module to synchronize the received calls list

pbSyncByMissed(); causes the module to synchronize the missed calls list

pbSyncByLastCall(); //causes the module to synchronize the last call list

getLocalLastDialedList(); //causes the module to read one of recently dialed call number(record by module)

getLocalLastReceivedList(); //causes the module to read one of recently received call number(record by module)

getLocalLastMissedList(); //causes the module to read one of recently missed call number(record by module)

dialLastReceivedCall(); //causes the module to dial last received phone number

clearLocalCallHistory(); //causes the module to clear call history(record by module)

sppDataTransmit(); //send SPP data to the remote device

setClockdebugMode(); //causes the module to enter clock debug mode

volumeDown(); //causes the module to decrease the speaker volume

enterTestMode(); //not usefull for normal user

setFixedFrequency(); //not usefull for normal user

emcTestMode(); //not usefull for normal user

setRFRegister(); //not usefull for normal user

inquiryStart(); //causes the module to inquiry Bluetooth devices

inquiryStop();  //causes the module to cancel inquiry Bluetooth devices

volumeUp(); //causes the module to increase the speaker volume

shutdown(); //shutdown module, need to be power cycled to start again, HW reset did not work

enterConfigMode(); //reset module (hw way) and enable pskey configuration mode 

quitConfigMode(); //disable pskey configuration mode

readName(); //read name of module in configuration mode

writeName(String NewName); //write name of module in configuration mode

readAllPSK(); //read all pskeys - did not work for me right now, also using pskey config app did not work for this.

readPin(); //read pin of module in configuration mode

writePin(String NewPin); //write pin of module in configuration mode
 
readBaudRate(); //read baudrate of module in configuration mode - did not work for me right now, also using pskey config app did not work for this. 

writeBaudRate(uint8_t NewBaudRate = OVC3860_BAUDRATE_115200); //write baudrate of module in configuration mode - did not work for me right now, also using pskey config app did not work for this.

readSysBTMode(); //read bluetooth mode 

writeSysBTMode(uint8_t mode); //write bluetooth mode, 00-UART,03-HEADSET,04-CARKIT,06-AVSINK,09-MOBILE

readClassOfDevice(); // read class of device

writeClassOfDevice(); // write class of device

readBTAddress(); //read bluetooth address of module

resetModule();//reset module HW way = puting reset pin low and then high


