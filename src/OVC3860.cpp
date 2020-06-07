/*
        GNU GPL v3
        (C) Tomas Kovacik [nail at nodomain dot sk]
        https://github.com/tomaskovacik/

	library for BT modules based on OVC3860 chip by omnivision

  based on 2 pdfs:
  https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/BLK-MD-SPK-B_AT_Command_set_original.pdf
  https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/OVC3860_AT_Command_Application_Notes.pdf

  some commands did not work for me (P*,MM,MN...)
  (file BLK-MD-SPK-B_AT_Command_set_original.pdf is marked in header as -B but description is mentioning -A version of module, maybe
  this is why some commands did not work)

  PSKey confguration implemented: used to change name,pin,UART baudrate (this one is problematic currently, works with original soft in one from milion tryes....)

*/

#include "OVC3860.h"
#include <Arduino.h>

#if defined(USE_SW_SERIAL)
#include <SoftwareSerial.h>
#endif



#if defined(USE_SW_SERIAL)
#if ARDUINO >= 100
OVC3860::OVC3860(SoftwareSerial *ser, uint8_t resetPin)
#else
OVC3860::OVC3860(NewSoftSerial *ser, uint8_t resetPin)
#endif
#else
OVC3860::OVC3860(HardwareSerial *ser, uint8_t resetPin)
#endif
{
  btSerial = ser;
  _reset = resetPin;
}

/*
   Destructor
*/
OVC3860::~OVC3860() {
  btSerial->end();
}

void OVC3860::begin(uint32_t baudrate) {
  _baudrate = baudrate;
  btSerial->begin(_baudrate);
  pinMode(_reset, OUTPUT);
  OVC3860::resetHigh();
}

void OVC3860::resetLow() {
  digitalWrite(_reset, LOW);
}

void OVC3860::resetHigh() {
  digitalWrite(_reset, HIGH);
}

void OVC3860::resetModule() {
#if defined DEBUG 
  DBG(F("reseting module\n"));
#endif
  resetLow();
  delay(100);
  resetHigh();
}

/*
   debug output
*/
#if defined DEBUG
void OVC3860::DBG(String text) {
	Serial.print(text);
}
#endif

/*
   AA1 = 44800
   AA2 = 44100
   AA4 = 32000
   AA8 = 16000
   AE = config error
   AF = codec closed
   AS = phone call

   II HSHF enters pairing state, discoverable for 2 minutes
   IJ2 HSHF exits pairing mode and enters listening
   IV HSHF State Is Connected indication
   IA HSHF State Is Listening
   IC Call-setup status is outgoing
   IF Call-setup status is idle/hang-up indication
   IG ongoing call indication
   IP5 Outgoing call number length indication,IPXX The next or previous PB number length indication, IP<lehgth>: Length of Phone Number
   IR12345 Outgoing call number indication (IP, IR indications only supported by HFP1.5 version.)
   IS

   MA playing
   MB pause
   MC Outgoing call number indication (audio transfered from handset(phone?) to module)
   MD The voice is on phone indication ((audio transfered to handset(phone?) from module)
   MGX The HSHF applications state is X indication, 1 – “Ready”, 2 – “Connecting”, 3 – “Connected”, 4 –“outgoing call”, 5 –“incoming call”,6 – “ongoing call”
   MGXY X and Y are auto answer and auto connect after power on configuration, X:1 – “support auto answer”, 0 – “not support auto answer”, Y:1 – “support auto connect after power on”, 0 – “no
   MFXY Power ON Init Complete Report AutoAnswer and PowerOn Auto Connection Configuration

   PE The voice dial start indication
   PF The voice dial is not supported indication
   PA0 The phone does not support the phone book feature
   PA1 The phone supports the phone book feature
   PBYY The next or previous PB number, PB<phonebook>: One Phonebook Indication
   PC End of Phonebook/Call History
   MN<phone number>, One Phonebook Indication

*/
uint8_t OVC3860::decodeReceivedString(String receivedString) {
    receivedString.trim();
#if defined DEBUG 
  DBG(receivedString);
  DBG(F("\n"));
#endif
  //while (receivedString[0] == 0x20) {receivedString = receivedString.substring(0);};
  if (memcmp(&receivedString[0], "AX_PA", 5) == 0) {
  } else if (memcmp(&receivedString[0], "AA1", 3) == 0) { //The audio sample rating is set 48000
    PowerState = On;
    Audio = ASR_44800;
  } else if (memcmp(&receivedString[0], "AA2", 3) == 0) { //The audio sample rating is set 44100
    PowerState = On;
    Audio = ASR_44100;
  } else if (memcmp(&receivedString[0], "AA4", 3) == 0) { //The audio sample rating is set 32000
    PowerState = On;
    Audio = ASR_32000;
  } else if (memcmp(&receivedString[0], "AA8", 3) == 0) { //The audio sample rating is set 16000
    PowerState = On;
    Audio = ASR_16000;
  } else if (memcmp(&receivedString[0], "AE", 2) == 0) { //Audio config error
    PowerState = On;
    Audio = ConfigError;
  } else if (memcmp(&receivedString[0], "AF", 2) == 0) { //Audio codec is closed
    PowerState = On;
    Audio = CodecClosed;
  } else if (memcmp(&receivedString[0], "AS", 2) == 0) { //Audio codec is in phone call mode
    PowerState = On;
    Audio = PhoneCall;
  } else if (memcmp(&receivedString[0], "EPER", 3) == 0) { //Error eeprom parameter
    PowerState = On;
    return 0;
  } else if (memcmp(&receivedString[0], "ERR", 3) == 0) { //The command is error
    PowerState = On;
    return 0;
  } else if (memcmp(&receivedString[0], "II", 2) == 0) { //HSHF enters pairing state indication
#if defined DEBUG 
    DBG(F("II\n"));
#endif
    PowerState = On;
    BTState = Discoverable;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "IJ2", 3) == 0) {//HSHF exits pairing mode and enters listening
    PowerState = On;
    BTState = Listening;
  } else if (memcmp(&receivedString[0], "IA", 2) == 0) { //Disconnected,HSHF state is listening
    PowerState = On;
    HFPState = Disconnected;
  } else if (memcmp(&receivedString[0], "IC", 2) == 0) { //Call-setup status is outgoing
    PowerState = On;
    CallState = OngoingCall;
  } else if (memcmp(&receivedString[0], "IF", 2) == 0) { //Phone hand up,Call-setup status is idle
    PowerState = On;
    CallState = PhoneHangUp;
  } else if (memcmp(&receivedString[0], "IG", 2) == 0) {
    PowerState = On;
  } else if (memcmp(&receivedString[0], "IL", 2) == 0) { //Hold Active Call, Accept Other Call
    PowerState = On;
    CallState = OngoingCall;
  } else if (memcmp(&receivedString[0], "IM", 2) == 0) { //Make Conference Call
    PowerState = On;
    CallState = OngoingCall;
  } else if (memcmp(&receivedString[0], "IN", 2) == 0) { //Release Held Call, Reject Waiting Call
    PowerState = On;
    CallState = PhoneHangUp;
  } else if (memcmp(&receivedString[0], "IP", 2) == 0) { //IPX Outgoing call number length(X) indication
    PowerState = On;
    CallState = OngoingCall;
  } else if (memcmp(&receivedString[0], "IR", 2) == 0) { //Outgoing call number indication
    PowerState = On;
    CallState = OutgoingCall;
  } else if (memcmp(&receivedString[0], "IS", 2) == 0) { //IS<version> , Power ON Init Complete
    PowerState = On;
  } else if (memcmp(&receivedString[0], "IT", 2) == 0) { //Release Active Call, Accept Other Call
    PowerState = On;
    CallState = OngoingCall;
  } else if (memcmp(&receivedString[0], "IV", 2) == 0) { //Connected
    PowerState = On;
    HFPState = Connected;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "MA", 2) == 0) { //AV pause/stop Indication
    PowerState = On;
    MusicState = Idle;
    CallState = PhoneHangUp;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "MB", 2) == 0) { //AV play Indication
    PowerState = On;
    MusicState = Playing;
    CallState = PhoneHangUp;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "MC", 2) == 0) { //Indication the voice is on Bluetooth
    PowerState = On;
  } else if (memcmp(&receivedString[0], "MD", 2) == 0) { //Indication the voice is on phone
    PowerState = On;
  } else if (memcmp(&receivedString[0], "ME", 2) == 0) {
    PowerState = On;
  } else if (memcmp(&receivedString[0], "MEM:", 4) == 0) {
    PowerState = On;
    //need to chage return to be long long (64bit?) to return memory data
  } else if (memcmp(&receivedString[0], "MF", 2) == 0) { //MFXY: X and Y are auto answer and auto connect configuration
    PowerState = On;
#if defined DEBUG
    DBG(F("MF"));
#endif
    switch (receivedString[2]) {
      case '0':
        AutoAnswer = Off;
#if defined DEBUG
        DBG(F("AA OFF"));
#endif
        break;
      case '1':
        AutoAnswer = On;
#if defined DEBUG
        DBG(F("AA ON"));
#endif
        break;
    }
    switch (receivedString[3]) {
      case '0':
        AutoConnect = Off;
#if defined DEBUG
        DBG(F("AC OFF"));
#endif
        break;
      case '1':
        AutoConnect = On;
#if defined DEBUG
        DBG(F("AA ON"));
#endif
        break;
    }
  } else if (memcmp(&receivedString[0], "MG", 2) == 0) { //MGX: The HSHF applications state is X indication, Report Current HFP Status
    /*
      HFP Status Value Description:(MG)
      1. Ready (to be connected)
      2. Connecting
      3. Connected
      4. Outgoing Call
      5. Incoming Call
      6. Ongoing Call
    */
    PowerState = On;
    switch (receivedString[2]) {
      case '1':
        HFPState = Ready;
        break;
      case '2':
        HFPState = Connecting;
        break;
      case '3':
        HFPState = Connected;
        break;
      case '4':
        HFPState = OutgoingCall;
        break;
      case '5':
        HFPState = IncomingCall;
        break;
      case '6':
        HFPState = OngoingCall;
        break;
    }
  } else if (memcmp(&receivedString[0], "ML", 2) == 0) { //Report Current AVRCP Status
    /*
      AVRCP Status Value Description:(ML)
      1. Ready (to be connected)
      2. Connecting
      3. Connected
    */
    PowerState = On;
    switch (receivedString[2]) {
      case '1':
        AVRCPState = Ready;
        break;
      case '2':
        AVRCPState = Connecting;
        break;
      case '3':
        AVRCPState = Connected;
        break;
    }
  } else if (memcmp(&receivedString[0], "MM", 2) == 0) { //name
    PowerState = On;
  } else if (memcmp(&receivedString[0], "MN", 2) == 0) { //pin
    PowerState = On;
  } else if (memcmp(&receivedString[0], "MP", 2) == 0) { //Music Pause
#if defined DEBUG 
    DBG(F("MP"));
#endif
    PowerState = On;
    MusicState = Idle;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "MR", 2) == 0) { //Music Resume
    PowerState = On;
    MusicState = Playing;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "MS", 2) == 0) { //Backward song
    PowerState = On;
    MusicState = Rewinding;
  } else if (memcmp(&receivedString[0], "MU", 2) == 0) { //Report Current A2DP Status
    /*
      A2DP Status Value Description:(MU)
      1. Ready (to be connected)
      2. Initializing
      3. Signalling Active
      4. Connected
      5. Streaming
    */
    PowerState = On;
    switch (receivedString[2]) {
      case '1':
        A2DPState = Ready;
        break;
      case '2':
        A2DPState = Initializing;
        break;
      case '3':
        A2DPState = SignallingActive;
        break;
      case '4':
        A2DPState = Connected;
        break;
      case '5':
        A2DPState = Streaming;
        break;
    }
  } else if (memcmp(&receivedString[0], "MX", 2) == 0) { //Forward song
    PowerState = On;
    MusicState = FastForwarding;
  } else if (memcmp(&receivedString[0], "MY", 2) == 0) { //AV Disconnect Indication
    PowerState = On;
    OVC3860::queryA2DPStatus();
  } else if (memcmp(&receivedString[0], "M0", 2) == 0) { //
    PowerState = On;
    BTState = Disconnected;
  } else if (memcmp(&receivedString[0], "M1", 2) == 0) { //AV Disconnect Indication
    PowerState = On;
    BTState = Connected;
  } else if (memcmp(&receivedString[0], "M2", 2) == 0) { //AV Disconnect Indication
    PowerState = On;
    CallState = IncomingCall;
  } else if (memcmp(&receivedString[0], "M3", 2) == 0) { //AV Disconnect Indication
    PowerState = On;
    CallState = OutgoingCall;
  } else if (memcmp(&receivedString[0], "M4", 2) == 0) { //AV Disconnect Indication
    PowerState = On;
    CallState = OngoingCall;
  } else if (memcmp(&receivedString[0], "NOEP", 4) == 0) {//No eeprom
    PowerState = On;
    return 0;
  } else if (memcmp(&receivedString[0], "NUM", 3) == 0) {
    CallState = IncomingCall;
    PowerState = On;
    CallerID = receivedString.substring(4);
  } else if (memcmp(&receivedString[0], "OK", 2) == 0) {
    PowerState = On;
    return 1;
  } else if (memcmp(&receivedString[0], "PA", 2) == 0) {
    PowerState = On;
  } else if (memcmp(&receivedString[0], "PB", 2) == 0) {
    PowerState = On;
  } else if (memcmp(&receivedString[0], "PC", 2) == 0) { //?
    PowerState = On;
  } else if (memcmp(&receivedString[0], "PE", 2) == 0) { //The voice dial start indication
    PowerState = On;
  } else if (memcmp(&receivedString[0], "PF", 2) == 0) { //The voice dial is not supported/stopped indication
    PowerState = On;
  } else if (memcmp(&receivedString[0], "SC", 2) == 0) { //SPP opened
    PowerState = On;
    BTState = SPPopened;
#if defined DEBUG 
    DBG(F("SPP opened\n"));
#endif
  } else if (memcmp(&receivedString[0], "SD", 2) == 0) { //SPP closed
    PowerState = On;
    BTState = SPPclosed;
#if defined DEBUG 
    DBG(F("SPP closed\n"));
#endif
  } else if (memcmp(&receivedString[0], "SW", 2) == 0) { //Command Accepted
    PowerState = On;
  } else if (memcmp(&receivedString[0], "VOL", 3) == 0) { //Command Accepted
    PowerState = On;
    volume = receivedString.substring(3).toInt();
  } else {
#if defined DEBUG 
    DBG(F("Received unknown string:"));
    DBG(receivedString);
    DBG(F("\n"));
#endif
  }
  return 1;
}

uint8_t OVC3860::sendRawData(uint8_t _size, uint8_t _data[]) {
#if defined(USE_PSKCONFIG)
  for (uint8_t i = 0; i < _size; i++ ) {
    btSerial -> write(_data[i]);
  }

#if defined DEBUG 
  DBG(F("Sending raw data: "));
#endif
  for (uint8_t i = 0; i < _size; i++ ) {
#if defined DEBUG 
    DBG(String(_data[i], HEX));
#endif
  }
#if defined DEBUG 
  DBG(F("\n"));
#endif
#endif
}

uint8_t OVC3860::quitConfigMode() { //responce: 0x60,0x00,0x00,0x00
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Quiting config mode\n"));
#endif
    // OVC3860::ResetModule();
    uint8_t Data[4] = {0x50, 0x00, 0x00, 0x00};
    sendRawData(4, Data);
    BTState = Disconnected;
    btSerial -> end(); // end comunication in 115200b for config mode
    delay(100);
    btSerial -> begin(_baudrate); //restor user set bauMrate
  }
#endif
}

uint8_t OVC3860::enterConfigMode() {
#if defined(USE_PSKCONFIG)
  while (BTState != ConfigMode) {
    btSerial -> end(); //end costum baudrate comunication
    delay(100);
    btSerial -> begin(115200); //initialize comunication in 115200b for config mode

    uint8_t initConfigData[9] = {0xC5, 0xC7, 0xC7, 0xC9, 0xD0, 0xD7, 0xC9, 0xD1, 0xCD};

    OVC3860::resetModule();

    while ((btSerial -> available()) != 7) {} //wait for 7bytes

    if (btSerial -> read() == 0x04 && btSerial -> read() == 0x0F && btSerial -> read() == 0x04 && btSerial -> read() == 0x00 && btSerial -> read() == 0x01 && btSerial -> read() == 0x00 && btSerial -> read() == 0x00) {
      sendRawData(9, initConfigData);
    }
    else
      return false;

    while ((btSerial -> available()) != 7) {} //wait for 7bytes

    if (btSerial -> read() == 0x04 && btSerial -> read() == 0x0F && btSerial -> read() == 0x04 && btSerial -> read() == 0x01 && btSerial -> read() == 0x01 && btSerial -> read() == 0x00 && btSerial -> read() == 0x00) {
#if defined DEBUG 
      DBG(F("Config mode!\n"));
#endif
      BTState = ConfigMode;
    }
    else
      return false;

    delay(1000);
  }
#endif
}

uint8_t OVC3860::readName() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading name\n"));
#endif
    uint8_t Data[4] = {0x11, 0xc7, 0x00, 0x10};
    return sendRawData(4, Data);
  }
#endif
}

uint8_t OVC3860::readBTAddress() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading name\n"));
#endif
    uint8_t Data[4] = {0x10, 0x18, 0x00, 0x6};
    return sendRawData(4, Data);
  }
#endif
}

uint8_t OVC3860::readSysBTMode() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading mode \n"));
#endif
    uint8_t Data[4] = {0x10, 0x07, 0x00, 0x01};
    return sendRawData(4, Data);
  }
#endif
}

uint8_t OVC3860::writeSysBTMode(uint8_t mode) {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
    uint8_t Data[5] = {0x30, 0x07, 0x00, 0x01, mode};
    return sendRawData(5, Data);
  }
#endif
}

uint8_t OVC3860::writeName(String NewName) { //resposce: 0x41,0xc7,0x00,0x10
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
    if (NewName.length() - 2 > 16) { //count for termination char
#if defined DEBUG 
      DBG(F("name to long, max 16chars\n"));
#endif
      return false;
    } else {
#if defined DEBUG 
      DBG(F("Writing name:"));
      DBG(NewName);
      DBG(F("\n"));
#endif
      uint8_t Data[20] = {0x31, 0xc7, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      for (uint8_t i = 0; i < NewName.length() - 2; i++) {
        Data[i + 4] = NewName[i];
      }
      return sendRawData(20, Data);
    }
  }
#endif
}


uint8_t OVC3860::readClassOfDevice() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading class of device:\n"));
#endif
    uint8_t Data[4] = {0x10, 36, 0x00, 0x03};
    return sendRawData(4, Data);
  }
#endif
}

uint8_t OVC3860::writeClassOfDevice() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Writing class of device: 0x24 0x04 0x14\n"));
#endif
    uint8_t Data[7] = {0x30, 36, 0x00, 0x03, 0x14, 0x04, 0x24};
    return sendRawData(7, Data);
  }
#endif
}


uint8_t OVC3860::readAllPSK() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading All PSK\n"));
#endif
    uint8_t Data[4] = {0x10, 0x00, 0x03, 0x3D};
    return sendRawData(4, Data);
  }
#endif
}


uint8_t OVC3860::readPin() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading Pin\n"));
#endif
    uint8_t Data[4] = {0x11, 0xBF, 0x00, 0x08};
    return sendRawData(4, Data);
  }
#else
#endif
}

uint8_t OVC3860::writePin(String NewPin) { //resposce: 0x41,0xc7,0x00,0x10
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
    if (NewPin.length() - 2 > 8) { //count for termination char
#if defined DEBUG 
      DBG(F("name to long, max 8 chars"));
#endif
      return false;
    } else {
#if defined DEBUG 
      DBG(F("Writing pin: "));
      DBG(NewPin);
      DBG(F("\n"));
#endif
      uint8_t Data[12] = {0x31, 0xBF, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      for (uint8_t i = 0; i < NewPin.length() - 2; i++) {
        Data[i + 4] = NewPin[i];
      }
      return sendRawData(12, Data);
    }
  }
#endif
}


uint8_t OVC3860::readBaudRate() {
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Reading baudrate\n"));
#endif
    uint8_t Data[4] = {0x10, 0x11, 0x00, 0x01};
    return sendRawData(4, Data);
  }
#endif
}

uint8_t OVC3860::writeBaudRate(uint8_t NewBaudRate) { //resposce: 0x41,0xc7,0x00,0x10
#if defined(USE_PSKCONFIG)
  if (BTState != ConfigMode) {
    return false;
  } else {
#if defined DEBUG 
    DBG(F("Writing baudrate\n"));
#endif
    uint8_t Data[5] = {0x30, 0x11, 0x00, 0x01, NewBaudRate};
    return sendRawData(5, Data);
  }
#endif
}


uint8_t OVC3860::decodeReceivedDataArray(uint8_t data[]) {
#if defined(USE_PSKCONFIG)
  uint16_t packetSize = ((data[2] << 8) | (data[3] & 0xff)); //+ (start,address, packetsize1 and packetsize2)
  packetSize += 4;
  uint16_t addressFull = (((data[0] << 8) | (data[1] & 0xff)) & 0x0FFF);
  if ( addressFull == OVC3860_PSKEY_ADDR_NAME ) {
    //decode name:
    BT_NAME = "";
    for (uint16_t i = 4; i < packetSize; i++) {
      if (data[i] == 0) break;
      BT_NAME = BT_NAME + char(data[i]);
    }
  }
  if ( addressFull == OVC3860_PSKEY_ADDR_PIN ) {
    //decode pin:
    BT_PIN = "";
    for (uint16_t i = 4; i < packetSize; i++) {
      if (data[i] == 0) break;
      BT_PIN = BT_PIN + char(data[i]);
    }
  }

  if ( addressFull == OVC3860_PSKEY_ADDR_LOCAL_BTADDR ) {
    //decode address:
#if defined DEBUG 
    DBG(F("address: "));
#endif
    for (uint16_t i = 4; i < packetSize; i++) {
      if (data[i] == 0) break;
#if defined DEBUG 
      DBG(String(data[i], HEX));
      DBG(F(":"));
#endif
      BT_ADDR[i - 4] = (uint8_t)data[i];
    }
#if defined DEBUG 
    DBG(F("\n"));
#endif
  }

  if ( addressFull == (OVC3860_PSKEY_READ_RESPONCE | OVC3860_PSKEY_ADDR_BTSYS_MODE)) {
    //decode bt mode:
#if defined DEBUG 
    DBG(F("BT Mode: "));
    DBG(String(data[5], HEX));
    DBG(F("\n"));
#endif
  }

  for (uint16_t i = 0; i < packetSize; i++) {
#if defined DEBUG 
    DBG(String(data[i], HEX));
    DBG(F(" "));
    DBG(String(data[i]));
    DBG(F("\n"));
#endif
  }

#endif
}

uint8_t OVC3860::checkResponce(void){
  uint8_t timeout=500;//500ms -- datasheet did not stated any timeout for "OK" responce, so I give him 500ms
  while (!getNextEventFromBT() && timeout > 0)
  {
    timeout--;
    delay(1); // wait 1milisecond
  }
  if (!timeout) return false;
  return true;
}


uint8_t OVC3860::getNextEventFromBT() {
  delay(200);//delay needed to fillup buffers otherwise this will return false
#if defined(USE_PSKCONFIG)
  if (BTState == ConfigMode) {

    if (btSerial -> available()) {

      uint8_t startByte = btSerial -> read();

      switch (startByte >> 4) {
        case 0x2: //response to read
          {

            uint8_t addressByte = btSerial -> read();
            uint8_t packetSize1 = btSerial -> read();
            uint8_t packetSize2 = btSerial -> read();

            uint16_t packetSize = ((packetSize1 << 8) | (packetSize2 & 0xff)); //+ (start,address, packetsize1 and packetsize2)

            if (packetSize < 64 - 4) {
              uint8_t data[packetSize + 4];

              data[0] = startByte;
              data[1] = addressByte;
              data[2] = packetSize1;
              data[3] = packetSize2;

              for (uint16_t i = 4; i < packetSize + 4; i++) {
                data[i] = btSerial -> read();
              }

              return OVC3860::decodeReceivedDataArray(data);

            } else {
              //wanna read more then 64bytes:
              uint16_t start_addr = (((startByte & 0x0F) << 8) | (addressByte & 0xff));
              while (btSerial -> available()) {
                btSerial -> read(); //read what we have in buffer, then request data in 64Bytes blokcs:
              }
              uint8_t Data[4];
              uint8_t data[64];
              for ( uint16_t i = 0 ; i < (packetSize + start_addr) ; i = i + 64 ) {
                Data[0] = ((start_addr + i) >> 8) | 0x10;
                Data[1] = (start_addr + i) & 0xFF;
                Data[2] = 0;
                Data[3] = 64;
                sendRawData(4, Data);
                delay(100);
                uint8_t data_i = 0;
                while (btSerial -> available()) {
                  data[data_i++] = btSerial -> read();
                }
                return OVC3860::decodeReceivedDataArray(data);
              }
            }
          }
          break;
        case 0x4: //response to write, send only 4bytes,
          {
            uint8_t addressByte = btSerial -> read();
            uint8_t packetSize1 = btSerial -> read();
            uint8_t packetSize2 = btSerial -> read();

#if defined DEBUG 
            DBG(F("received raw data: "));
            DBG(String(startByte, HEX));
            DBG(String(addressByte, HEX));
            DBG(String(packetSize1, HEX));
            DBG(String(packetSize2, HEX));
            DBG(F("\n"));
#endif
	    return 1;
          }
          break;
        default:
          {
            Serial.println("ID: ");
            Serial.println(startByte, HEX);
          }
      }
    }

  } else {
#endif
    char c;
    String receivedString = "";
    while (btSerial -> available() > 0 && c != '\n') { //read serial buffer until \n
      c = (btSerial -> read());
      if (c == 0xA) {
        if (receivedString == "") { //nothing before enter was received
 #if defined DEBUG
          DBG(F("received only empty string\n running again myself...\n"));
#endif
          return OVC3860::checkResponce();
        }
        return decodeReceivedString(receivedString);
      }
      //append received buffer with received character
      if (c != 0xD) receivedString += c;  // skip \r
    }
#if defined(USE_PSKCONFIG)
  }
#endif
  return 0;
}

uint8_t OVC3860::sendData(String cmd) {
  OVC3860::getNextEventFromBT();
  String Command = "AT#" + cmd + "\r\n";
#if defined DEBUG 
  DBG(F("sending "));
  DBG(Command);
  DBG(F("\n"));
#endif
  delay(100);
  btSerial -> print(Command);
  return OVC3860::checkResponce();
}

/*
  Pairing
  Set pairing, waiting for the remote device to connect, the command format is:
  AT#CA // discoverable for 2 minutes
  ovc3860 returns the indication:
  II // state indication, HSHF enters pairing state indication
  if 2 minutes' timeout is arrived(no peer connect to ovc3860 device), returns the indication:
  IJ2 // state indication, HSHF exits pairing mode and enters listening
  The device can't be found, if need to be search, repeat Pairing operation.

  Enter Pairing Mode #CA

  Command
  #CA
  Current Status(s)
  Any
  Possible Indication(s)
  II
  Indication Description
  Enter Pairing Mode Indication

  Description
  This command puts the module in the pairing mode. The information response and causes will
  indicate the command success or failure. Enter pairing mode indication or failure indication
  will be sent to the host.

  Note:
  1. This command will cause a disconnection if module has already connected with some device.
  2. Module will exit pairing mode if connection not happen in 2 minutes.

  Syntax: AT#CA
*/
uint8_t OVC3860::pairingInit() {
  return OVC3860::sendData(OVC3860_PAIRING_INIT);
}

/*
  Exit pairing
  Exit pairing, can not be found by peers, the command format is:
  AT#CB // exit pairing mode, non-discoverable
  return:
  IJ2 // state indication, HSHF exits pairing mode and enters listening

  Cancel Pairing Mode #CB

  Command
  #CB
  Current Status(s)
  Pairing
  Possible   Indication(s)
  IJ2
  Indication Description
  Exit Pairing Mode Indication

  Description
  If the module is in pairing mode, this command causes the module to exit the pairing mode and
  enter the idle mode. The information response and causes will indicate the command success or
  failure.

  Syntax: AT#CB
*/
uint8_t OVC3860::pairingExit() {
  return OVC3860::sendData(OVC3860_PAIRING_EXIT);
}

/*
  Active connection
  Actively connect to the last successful connected device
  AT#CC // Connect to remote Device
  then input the local PIN code, and ovc3860 returns HSHF's state indication:
  IV
  HSHF state is connected indication
  if Bluetooth mobile phone is turned off or not in Bluetooth signal range, will return a stat
  us indication:
  IJ2 // HSHF exits pairing mode and enters listening state indication

  Connect HFP to Handset #CC
  Command
  #CC
  Current Status(s)
  HFP Status = 1
  Possible Indication(s)
  IV
  Description Indication
  Connecting Indication

  Note: You can get current HFP status by #CY.

  Description
  This command causes the module to connect to a paired handset. The information response and
  causes will indicate the command success or failure. Connect Indication will be sent to the host
  after the connection is established. Otherwise Disconnect Indication will be sent to the host.

  Syntax: AT#CC
*/
uint8_t OVC3860::connectHSHF() {
  return OVC3860::sendData(OVC3860_CONNECT_HSHF);
}

/*
  Exit connecting
  Send the command:
  AT#CD // ACL disconnect from remote
  ovc3860 returns the indication:
  IA // HSHF state is listening
  if the remote device disconnects connection actively, ovc3860 also returns the same indication

  Disconnect HFP from Handset #CD
  Command
  #CD
  Current Status(s)
  HFP Status ≥ 3
  Possible Indication(s)
  IA
  Description Indication
  Disconnected Indication

  Description
  This command causes the module to disconnect from the connected handset. The information
  response and causes will indicate the command success or failure. Disconnect Indication will be
  sent to the host after the connection is dropped.

  Syntax: AT#CD
*/
uint8_t OVC3860::disconnectHSHF() {
  return OVC3860::sendData(OVC3860_DISCONNECT_HSHF);
}

/*
  Answering the phone
  The phone receives a call, ovc3860 returns the indication, such as:   02167850001
  Receive a incoming call 02167850001
  at this time, user may refuse to answer the phone:
  AT#CF // Refuse to answer the phone
  ovc3860 returns:
  IF // Call-setup status is idle

  Reject Call #CF

  Command
  #CF
  Current Status(s)
  HFP status = 4
  Possible Indication(s)
  IF
  Indication Description
  Hang up Indication

  Description
  This command causes the module to reject an incoming call. The information response and causes
  will indicate the command success or failure.

  Syntax: AT#CF
*/
uint8_t OVC3860::callReject() {
  return OVC3860::sendData(OVC3860_CALL_REJECT);
}

/*
  user may answer the phone:
  AT#CE
  return the indication:
  IF // hang-up indication
  if the other party hangs up the phone, also return:
  IF // hang-up indication

  Answer Call #CE
  Command
  #CE
  Current Status(s)
  HFP Status = 4
  Possible
  Indication(s)
  IG
  Indication Description
  Pick up Indication

  Description
  This command causes the module to answer an incoming call. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#CE
*/
uint8_t OVC3860::callAnswer() {
  return OVC3860::sendData(OVC3860_CALL_ANSWARE);
}

/*
  End Call
  Command
  #CG
  Current Status(s)
  HFP Status = 5 | 6
  Posible Indication(s)
  IF
  Indication Description
  Hang up Indication

  Description
  This command causes the module to end an active call. The information response and causes will
  indicate the command success or failure.
  Syntax: AT#CG
*/
uint8_t OVC3860::callHangUp() {
  return OVC3860::sendData(OVC3860_CALL_HANGUP);
}

/*
  Redialing
  Redial the last outgoing call:
  AT#CH
  return:
  ICv//Call-setup status is outgoing

  Redial #CH
  Command
  #CH
  Current Status(s)
  HFP Status = 3 | 6
  Possible Indication(s)
  IC
  Indication Description
  Outgoing Call Indication

  Description
  This command causes the module to redial the last number called in the phone. The information
  response and causes will indicate the command success or failure.

  Syntax: AT#CH
*/
uint8_t OVC3860::callRedial() {
  return OVC3860::sendData(OVC3860_CALL_REDIAL);
}

/*
  Voice dialing
  The command format is:
  AT#CI
  return:
  PE // The voice dial start indication
  or,
  PF // The voice dial is not supported indication

  Voice Dial #CI
  Command
  #CI
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PE
  PF
  Indication Description
  Voice Dial Start Indication
  Handset Not Support Void Dial

  Description
  This command causes the module to active voice dial functionary in the phone. The information
  response and causes will indicate the command success or failure.
  Note: Voice dialing not works in some handset while .

  Syntax: AT#CI
*/
uint8_t OVC3860::voiceDialStart() {
  return OVC3860::sendData(OVC3860_VOICE_CALL_START);
}

/*
  cancel the voice dialing:
  AT#CJ
  return:
  PF //The voice dial is stopped indication

  Cancel Voice Dial #CJ
  Command
  #CJ
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PF
  Indication Description
  Voice Dial Stop Indication

  Description
  This command causes the module to cancel on going voice dial in the phone. The information
  response and causes will indicate the command success or failure.

  Syntax: AT#CJ
*/
uint8_t OVC3860::voiceDialEnd() {
  return OVC3860::sendData(OVC3860_VOICE_CALL_CANCEL);
}

/*
  Mute/Unmute MIC #CM

  Command
  #CM
  Current Status(s)
  HFP Status = 6
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted by Module

  Description
  This command causes the module to mute or unmute the MIC. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#CM
*/
uint8_t OVC3860::micToggle() {
  return OVC3860::sendData(OVC3860_MIC_TOGGLE);
}

/*
  Audio transfer
  Transfer audio between HSHF and phone speaker:
  AT#CO
  when transfered to HSHF, ovc3860 returns:
  MC //The voice is on Bluetooth indication
  when transfered to HSHF:
  MD //The voice is on phone indication

  Transfer Call to/from Handset #CO

  Command
  #CO
  Current Status(s)
  HFP Status = 6(without audio)
  HFP Status = 6(without audio)
  Possible Indication(s)
  MC
  MD
  Indication Description
  HFP Audio Connected
  MD HFP Audio Disconnect

  Description
  This command causes the module to transfer the active call from the module to the handset ( MD
  will received ) or from the handset to the module ( MC will received ). The information response
  and causes will indicate the command success or failure.

  Syntax: AT#CO
*/
uint8_t OVC3860::transferAudio() {
  return OVC3860::sendData(OVC3860_TRANSFER_AUDIO_TO_SPEAKER);
}

/*
  Release&Reject Call #CQ
  Command
  #CQ
  Current Status(s)
  HFP Status = 6
  Possible Indication(s)
  IN
  Indication Description
  Release Held Call, Reject Waiting Call
  by
  Description
  This command causes the module to release held call, and reject waiting call. The information
  response and causes will indicate the command success or failure.

  Syntax: AT#CQ
*/
uint8_t OVC3860::callReleaseReject() {
  return OVC3860::sendData(OVC3860_RELEASE_REJECT_CALL);
}

/*
  Release&Accept Call #CR

  Command
  #CR
  Current Status(s)
  HFP Status = 6
  Possible Indication(s)
  IT
  Indication Description
  Release Active Call, Accept Other Call

  Description
  This command causes the module to release active call, accept other call.
  The information response and causes will indicate the command success or failure.

  Syntax: AT#CR
*/
uint8_t OVC3860::callReleaseAccept() {
  return OVC3860::sendData(OVC3860_RELEASE_ACCEPT_CALL);
}

/*
  Hold&Accept Call #CS

  Command
  #CS
  Current Status(s)
  HFP Status = 6
  Possible Indication(s)
  IM
  Indication Description
  Release Active Call, Accept Other Call

  Description
  This command causes the module to hold active call, accept other call. The information response
  and causes will indicate the command success or failure.

  Syntax: AT#CS
*/
uint8_t OVC3860::callHoldAccept() {
  return OVC3860::sendData(OVC3860_HOLD_ACCEPT_CALL);
}

/*
  Conference Call #CT

  Command
  #CT
  Current Status(s)
  HFP Status = 6
  Possible Indication(s)
  IM
  Indication Description
  Make Conference Call

  Description
  This command causes the module to make a conference call. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#CT
*/
uint8_t OVC3860::callConference() {
  return OVC3860::sendData(OVC3860_CONFERENCE_CALL);
}


/*
  taken from BLK module datasheet
  BLK-MD-SPK-B_AT_Command_set_original( BLK-MD-SPK-B AT Command Application Guide)

  no mention of this command in OVC3860_AT_Command_Application_Notes.pdf

  module responce:

  sending AT#CV
  ERR2
  ERR2
  OK
  OK
*/
uint8_t OVC3860::pairingDeleteThenInit() {
  return OVC3860::sendData(OVC3860_PAIRING_DELETE_THEN_INIT);
}

/*
  Dialing
  For example, dialing 10086, the command format is:
  AT#CW10086
  return
  IC // Call-setup status is outgoing
  IP5 // Outgoing call number length indication
  IR10086 // Outgoing call number indication

  Dial One Call #CW

  Command
  #CW
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  IC
  IP<lehgth>
  IR<phone number>
  Outgoing Call Indication
  Length of Phone Number
  Current Call Indication

  Description
  This command causes the module to dial one call. The information response and causes will
  indicate the command success or failure.
  Note: IP, IR indications only supported by HFP1.5 version.


  Syntax: AT#CW13800138000
*/
uint8_t OVC3860::callDialNumber(String number) {
  return OVC3860::sendData(OVC3860_CALL_DIAL_NUMBER + number);
}

/*
  Sending DTMF
  For example, sending number “1”
  AT#CX1
  return:
  OK // send DTMF successfully indication
  NOTE: supported sending characters (0-9, #, *, A-D).

  Send DTMF #CX

  Command
  #CX
  Current Status(s)
  HFP Status = 6
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to send one DTMF. The information response and causes will
  indicate the command success or failure.
  Syntax:
  AT#CX1
  AT#CX5

*/
uint8_t OVC3860::sendDTMF(char c) {
  switch (c)
{
case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
case '#':
case '*':
case 'A':
case 'B':
case 'C':
case 'D':
  return OVC3860::sendData(OVC3860_SEND_DTMF+String(c));
default:
#if defined DEBUG
	DBG(F("Unsupported characeter, supported: 0-9, #, *, A-D"));
#endif
return false;
}
}

/*
  Query the HSHF applications state, the command is:
  AT#CY
  ovc3860 returns:
  MGX // The HSHF applications state is X indication
  NOTE: X is the return parameter:
          1 – “Ready”
          2 – “Connecting”
          3 – “Connected”
          4 –“Outgoing call”
          5 –“Incoming call”
          6 – “Ongoing call”.

  Query HFP Status #CY

  Command
  #CY
  Current Status(s)
  Any
  Possible Indication(s)
  MG<code>
  Report Current HFP Status

  Description
  This command queries the module’s HFP current status. The information response and causes will
  indicate the command success or failure.

  Syntax: AT#CY
*/
uint8_t OVC3860::queryHFPStatus() {
  return OVC3860::sendData(OVC3860_QUERY_HFP_STATUS);
}

/*
  Reset #CZ

  Command
  #CZ
  Current Status(s)
  Any
  Possible
  IS<version>
  MF<a><b>
  Indication Description
  Power ON Init Complete
  Report Auto Answer and PowerOn Auto
  Connection Configuration

  Description
  This command causes the module to reset. The information response and causes will indicate the
  command success or failure.

  Syntax: AT#CZ
*/
uint8_t OVC3860::reset() {
  return OVC3860::sendData(OVC3860_RESET);
}

/*
  Play/Pause Music #MA

  Command
  #MA
  Current Status(s)
  A2DP State = 5
  Possible Indication(s)
  MA
  MB
  Indication Description
  AV pause/stop Indication
  AV play Indication

  Description
  If the module is connected with a AV Source, this command causes the AV source to play/pause
  music. If module isn’t connected AV source, this command will cause module try to connected
  current connected mobile’s AV source. The information response and causes will indicate the
  command success or failure.

  Syntax: AT#MA
*/
uint8_t OVC3860::musicTogglePlayPause() {
  return OVC3860::sendData(OVC3860_MUSIC_TOGGLE_PLAY_PAUSE);
}

/*
  Stop Music #MC

  Command
  #MC
  Current Status(s)
  A2DP State = 5
  Possible Indication(s)
  MA
  Indication Description
  AV pause/stop Indication

  Description
  If the module is connected with a AV Source, this command causes the AV Source to Stop Music.
  The information response and causes will indicate the command success or failure.

  Syntax: AT#MC
*/
uint8_t OVC3860::musicStop() {
  return OVC3860::sendData(OVC3860_MUSIC_STOP);
}

/*
  Forward Music #MD

  Command
  #MD
  Current Status(s)
  A2DP State = 5
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted by Module

  Description
  If the module is connected with a AV Source, this command causes the AV Source to Play next
  song. The information response and causes will indicate the command success or failure.

  Syntax: AT#MD
*/
uint8_t OVC3860::musicNextTrack() {
  return OVC3860::sendData(OVC3860_MUSIC_NEXT_TRACK);
}

/*
  Backward Music #ME

  Command
  #ME
  Current Status(s)
  A2DP State = 5
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted by Module

  Description
  If the module is connected with a AV Source, this command causes the AV Source to play last
  song. The information response and causes will indicate the command success or failure.

  Syntax: AT#ME
*/
uint8_t OVC3860::musicPreviousTrack() {
  return OVC3860::sendData(OVC3860_MUSIC_PREVIOUS_TRACK);
}

/*
  Query Auto Answer and PowerOn Auto Connection Configuration #MF

  Command Current Status(s)
  #MF
  Current Status(s)
  Any
  Possible Indication(s)
  MF<a><b>
  Indication Description
  Report Auto Answer and PowerOn Auto
  Connection Configuration

  Description
  This command queries the module’s auto answer configuration and poweron auto connect
  configuration. The information response and causes will indicate the command success or failure

  Syntax: MF<a><b>
  Value:
  < a >: auto answer configuration, where 0: disable, 1: enabled
  < b >: poweron auto configuration, where 0: disable, 1: enabled
*/
uint8_t OVC3860::queryConfiguration() {
  return OVC3860::sendData(OVC3860_QUERY_CONFIGURATION);
}

/*
  Enable PowerOn Auto Connection #MG

  Command
  #MG
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command enables the module to connect to the last used AG after PowerOn. The information
  response and causes will indicate the command success or failure.

  Syntax: AT#MG
*/
uint8_t OVC3860::autoconnEnable() {
  return OVC3860::sendData(OVC3860_AUTOCONN_ENABLE);
}
/*
  Disable PowerOn Auto Connection #MH

  Command
  #MH
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command disables the module to connect to the Last used AG after PowerOn. The
  information response and causes will indicate the command success or failure.

  Syntax: AT#MH
*/
uint8_t OVC3860::autoconnDisable() {
  return OVC3860::sendData(OVC3860_AUTOCONN_DISABLE);
}

/*
  Connect to AV Source #MI

  Command
  #MI
  Current Status(s)
  A2DP State = 1
  Possible Indication(s)
  MA
  MB
  Indication Description
  AV pause/stop Indication
  AV play Indication

  Description
  If the module is connected with a HFP phone, this command causes the module try to connect to
  the phone’s AV Source. The information response and causes will indicate the command success
  or failure.
  Note: Music will be played automatic after A2DP connected in some handset.

  Syntax: AT#MI
*/
uint8_t OVC3860::connectA2DP() {
  return OVC3860::sendData(OVC3860_AV_SOURCE_CONNECT);
}

/*
  Disconnect from AV Source #MJ

  Command
  #MJ
  Current Status(s)
  A2DP Status ≥ 3
  Possible Indication(s)
  MY
  Indication Description
  AV Disconnect Indication

  Description
  This module causes the module to disconnect from the connected phone’s AV source. The
  information response and causes will indicate the command success or failure.

  Syntax: AT#MJ
*/
uint8_t OVC3860::disconnectA2DP() {
  return OVC3860::sendData(OVC3860_AV_SOURCE_DISCONNECT);
}

/*
  Change Local Device Name Casually #MM

  Command
  #MM
  Current Status(s)
  Any
  Parameters
  [new name]
  Possible Indication(s)
  [MM<current name>]
  Indication Description
  Report Current Local Device Name

  Description
  This command causes the module to change the device name. The information response and causes will indicate the command
  success or failure.

  Syntax: AT#MM<new name>

  Value:< new name >: local device name

  ※ If new name is empty, the module will report current local device name.

  example:
  AT#MMMy Car Kit\r\n :the new name is “My Car Kit”
  AT#MM\r\n :indication will be MM<current name>
*/
uint8_t OVC3860::changeLocalName(String name) {
  if (!OVC3860::sendData(OVC3860_CHANGE_LOCAL_NAME + name)) {
#if defined(USE_PSKCONFIG)
    OVC3860::enterConfigMode();
    if (name.length() > 2) {// \r\n
      OVC3860::writeName(name);
    }
    OVC3860::readName();
    OVC3860::quitConfigMode();
    return true;
#endif
    return false;
  }
  return true;
}

/*
  Change Local Device Pin Code #MN

  Command
  #MN
  Current Status(s)
  Any
  Parameters
  [new pin]
  Possible Indication(s)
  [MN<current pin>]
  Indication Description
  Report current local device Pin code

  Description
  This command causes the module to change the device pin code. The information response and causes will indicate the
  command success or failure.

  Syntax: AT#MN<new pin>\r\n

  Value:<new pin >: local device pin (4 digital),

  ※ If new pin is empty, the module will report current local pin code.

  example:
  AT#MN1234 :the new pin is :1234
  AT#MN :indication will be MP<current pin>
*/
uint8_t OVC3860::changePin(String pin) {
  if (!OVC3860::sendData(OVC3860_CHANGE_PIN + pin)) {
#if defined(USE_PSKCONFIG)
    OVC3860::enterConfigMode();
    if (pin.length() > 2) { // \r\n
      OVC3860::writePin(pin);
    }
    OVC3860::readPin();
    OVC3860::quitConfigMode();
    return true;
#endif
    return false;
  }
}

/*
  Query AVRCP Status #MO

  Command
  #MO
  Current Status(s)
  Any
  Possible Indication(s)
  ML<code>
  <code> Status
  1 Ready (to be connected)
  2 Connecting
  3 Connected
  Indication Description
  Report Current AVRCP Status

  Description
  This command queries the module’s AVRCP current status. The information response and causes
  will indicate the command success or failure.

  Syntax: AT#MO

  Responce ML<code>:
  <code>
  1 Ready (to be connected)
  2 Connecting
  3 Connected

*/
uint8_t OVC3860::queryAvrcpStatus() {
  return OVC3860::sendData(OVC3860_QUERY_AVRCP_STATUS);
}

/*
  Enable Auto Answer #MP

  Command
  #MP
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command enables the module auto answer an incoming call. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#MP
*/
uint8_t OVC3860::autoAnswerEnable() {
  return OVC3860::sendData(OVC3860_AUTO_ANSWER_ENABLE);
}

/*
  Disable Auto Answer #MQ

  Command
  #MQ
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command disables the module auto answer an incoming call. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#MQ
*/
uint8_t OVC3860::autoAnswerDisable() {
  return OVC3860::sendData(OVC3860_AUTO_ANSWER_DISABLE);
}

/*
  Start Fast Forward #MR

  Command
  #MR
  Current Status(s)
  A2DP Status = 5 OK
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted by Module

  Description
  If the module is connected with a AV Source, this command causes the AV Source to start fast
  forward. The information response and causes will indicate the command success or failure.

  Syntax: AT#MR
*/
uint8_t OVC3860::musicStartFF() {
  return OVC3860::sendData(OVC3860_MUSIC_START_FF);
}

/*
  Start Rewind #MS

  Command
  #MS
  Current Status(s)
  A2DP Status = 5
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted by Module

  Description
  If the module is connected with a AV Source, this command causes the AV Source to start rewind.
  The information response and causes will indicate the command success or failure.

  Syntax: AT#MS
*/
uint8_t OVC3860::musicStartRWD() {
  return OVC3860::sendData(OVC3860_MUSIC_START_RWD);
}

/*
  Stop Fast Forward / Rewind #MT

  Command
  #MT
  Current Status(s)
  A2DP Status = 5 (after started Fast Forward or Rewind)
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted by Module

  Description
  If the module is connected with a AV Source, this command causes the AV Source to stop fast
  forward or rewind. The information response and causes will indicate the command success or failure.

  Syntax: AT#MT
*/
uint8_t OVC3860::musicStopFFRWD() {
  return OVC3860::sendData(OVC3860_MUSIC_STOP_FF_RWD);
}

/*
  Query A2DP Status #MV

  Command
  #MV
  Current Status(s)
  Any
  Possible Indication(s)
  MU<code>
  Indication Description
  Report Current A2DP Status

  Description
  This command queries the module’s A2DP current status. The information response and causes
  will indicate the command success or failure.

  Syntax: AT#MV

  Responce:
  MU<code>:1-5, status of A2DP
  1 Ready
  2 Initiating
  3 SignallingActive
  4 Connected
  5 Streaming
*/
uint8_t OVC3860::queryA2DPStatus() {
  return OVC3860::sendData(OVC3860_QUERY_A2DP_STATUS);
}

/*
  Write to Memory #MW

  Command
  #MW
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to write a byte into a given memory address. The information
  response and causes will indicate the command success or failure.

  Syntax: AT#MWADDR_VAL
  ADDR: a given 32-bit, hexadecimal address
  VAL: a written hexadecimal byte value
*/
uint8_t OVC3860::writeToMemory(String data) {
  return OVC3860::sendData(OVC3860_WRITE_TO_MEMORY + data);
}

/*
  Read from Memory #MX

  Command
  #MX
  Current Status(s)
  Any
  Possible Indication(s)
  MEM:<val>
  Indication Description
  The Returned Value


  Description
  This command causes the module to read a byte from a given memory address. The information
  response and causes will indicate the returned value from module reading.

  Syntax: AT#MXADDR
  ADDR: a given 32-bit, hexadecimal address
  <val>: a read hexadecimal byte value
*/
uint8_t OVC3860::readFromMemory(String data) {
  return OVC3860::sendData(OVC3860_READ_FROM_MEMORY + data);
}

/*
  Switch Two Remote Devices #MZ

  Command
  #MZ
  Current Status(s)
  Any
  Possible
  Indication(s)
  SW
  Indication Description
  Command Accepted

  Description
  This command causes the module to switch two remote devices. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#MZ
*/
uint8_t OVC3860::switchDevices() {
  return OVC3860::sendData(OVC3860_SWITCH_DEVICES);
}

/* taken from BLK-MD-SPK-B AT Command Application Guide, did not work on module for me (kovo)
    pdf: https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/BLK-MD-SPK-B_AT_Command_set_original.pdf
  Query Module Software Version #MY

  Command
  #MY
  Current Status(s)
  Any
  Parameters
  MW<version>
  Indication Description
  Report Module Software Version

  Description
  This command queries the module’s software version. The information response and causes will indicate the command
  success or failure.

  Syntax: AT#MY
*/
uint8_t OVC3860::queryVersion() {
  return OVC3860::sendData(OVC3860_QUERY_VERSION);
}

/*
  Synchronize Phonebook Stored by SIM(via AT Command) #PA

  Command
  #PA
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PA1
  PA0
  Indication Description
  Phonebook Synchronize Indication
  Command Not Supported

  Description
  This command causes the module to synchronize the phonebook which is stored by SIM. The information response and causes will indicate the command success or failure.

  Syntax: AT#PA
*/
uint8_t OVC3860::pbSyncBySim() {
  return OVC3860::sendData(OVC3860_PB_SYNC_BY_SIM);
}

/*
  Synchronize Phonebook Stored by Phone(via AT Command) #PB

  Command
  #PB
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PA1
  PA0
  Indication Description
  Phonebook Synchronize Indication
  Command Not Supported

  Description
  This command causes the module to synchronize the phonebook which is stored by phone. The information response and causes will indicate the command success or failure.

  Syntax: AT#PB
*/
uint8_t OVC3860::pbSyncByPhone() {
  return OVC3860::sendData(OVC3860_PB_SYNC_BY_PHONE);
}

/*
  Read Next One Phonebook Item #PC

  Command
  #PC
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  IP<lehgth>
  PB<phonebook>
  PC
  Indication Description
  Length of Phone Number
  One Phonebook Indication
  End of Phonebook/Call History

  Description
  This command causes the module to read next one phonebook item from phone or local. The information response and causes will indicate the command success or failure.

  Syntax: AT#PC
*/
uint8_t OVC3860::pbReadNextItem() {
  return OVC3860::sendData(OVC3860_PB_READ_NEXT_ITEM);
}

/*
  Read Previous One Phonebook Item #PD

  Command
  #PD
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  IP<lehgth>
  PB<phonebook>
  PC
  Indication Description
  Length of Phone Number
  One Phonebook Indication
  End of Phonebook/Call History

  Description
  This command causes the module to read previous one phonebook item from phone or local. The information response and causes will indicate the command success or failure.

  Syntax: AT#PD
*/
uint8_t OVC3860::pbReadPreviousItem() {
  return OVC3860::sendData(OVC3860_PB_READ_PREVIOUS_ITEM);
}

/*
  Synchronize Dialed Calls List (via AT Command) #PH

  Command
  #PH
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PA1
  PA0
  Indication Description
  Phonebook Synchronize Indication
  Command Not Supported

  Description
  This command causes the module to synchronize the dialed calls list. The information response and causes will indicate the command success or failure.

  Syntax: AT#PH
*/
uint8_t OVC3860::pbSyncByDialer() {
  return OVC3860::sendData(OVC3860_PB_SYNC_BY_DIALED);

}

/*
  Synchronize Received Calls List (via AT Command) #PI

  Command
  #PI
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PA1
  PA0
  Indication Description
  Phonebook Synchronize Indication
  Command Not Supported

  Description
  This command causes the module to synchronize the received calls list. The information response and causes will indicate the
  command success or failure.

  Syntax: AT#PI
*/
uint8_t OVC3860::pbSyncByReceiver() {
  return OVC3860::sendData(OVC3860_PB_SYNC_BY_RECEIVED);
}

/*
  Synchronize Missed Calls List (via AT Command) #PJ

  Command
  #PJ
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PA1
  PA0
  Indication Description
  Phonebook Synchronize Indication
  Command Not Supported

  Description
  This command causes the module to synchronize the missed calls list. The information response and causes
  will indicate the command success or failure.

  Syntax: AT#PJ
*/
uint8_t OVC3860::pbSyncByMissed() {
  return OVC3860::sendData(OVC3860_PB_SYNC_BY_MISSED);
}

/*
  Synchronize Last Call List (via AT Command) #PK

  Command
  #PK
  Current Status(s)
  HFP Status = 3
  Possible Indication(s)
  PA1
  PA0
  Indication Description
  Phonebook Synchronize Indication
  Command Not Supported

  Description
  This command causes the module to synchronize the last call list. The information response and causes will indicate the
  command success or failure.

  Syntax: AT#PK
*/
uint8_t OVC3860::pbSyncByLastCall() {
  return OVC3860::sendData(OVC3860_PB_SYNC_BY_LAST_CALL);
}

/*
  Get Recent Dialed Call History (record by module) #PL

  Command
  #PL
  Current Status(s)
  Any
  Possible Indication(s)
  IP<length>
  MN<phone number>
  PC
  Indication Description
  Length of Phone Number Indication
  One Phonebook Indication
  End of Phonbook/Call History Download Ind

  Description
  This command causes the module to read one of recently dialed call number(record by module). The information response and
  causes will indicate the command success or failure.

  Syntax: AT#PL
*/
uint8_t OVC3860::getLocalLastDialedList() {
  return OVC3860::sendData(OVC3860_GET_LOCAL_LAST_DIALED_LIST);
}

/*
  Get Recent Received Call History (record by module) #PM

  Command
  #PM
  Current Status(s)
  Any
  Possible Indication(s)
  IP<length>
  MN<phone number>
  PC
  Indication Description
  Length of Phone Number Indication
  One Phonebook Indication
  End of Phonbook/Call History Download Ind

  Description
  This command causes the module to read one of recently received call number(record by module). The information response
  and causes will indicate the command success or failure.

  Syntax: AT#PM
*/
uint8_t OVC3860::getLocalLastReceivedList() {
  return OVC3860::sendData(OVC3860_GET_LOCAL_LAST_RECEIVED_LIST);
}

/*
  Get Recent Missed Call History (record by module) #PN

  Command
  #PN
  Current Status(s)
  Any
  Possible Indication(s)
  IP<length>
  MN<phone number>
  PC
  Indication Description
  Length of Phone Numbervoid print Indication
  One Phonebook Indication
  End of Phonbook/Call History Download Ind

  Description
  This command causes the module to read one of recently missed call number(record by module). The information response
  and causes will indicate the command success or failure.

  Syntax: AT#PN
*/
uint8_t OVC3860::getLocalLastMissedList() {
  return OVC3860::sendData(OVC3860_GET_LOCAL_LAST_MISSED_LIST);
}

/*
  Dial Last Received Phone Number #PO

  Command
  #PO
  Current Status(s)
  HFP Status = 3 | 6
  Possible Indication(s)
  IC
  IP<length>
  IR<phone number>
  Indication Description
  Outgoing Call Indication
  Length of Phone Number Indication
  Current Call Indication

  Description
  This command causes the module to dial last received phone number. The information response and causes will indicate the
  command success or failure.

  Note: IP,IR indication only supported by HFP1.5 version.

  Syntax: AT#PO
*/
uint8_t OVC3860::dialLastReceivedCall() {
  return OVC3860::sendData(OVC3860_DIAL_LAST_RECEIVED_CALL);
}

/*
  Clear Call History (record by module) #PR

  Command
  Current Status(s)
  Possible Indication(s) Indication Description
  #PO
  Any
  OK
  Command Accepted by Module

  Description
  This command causes the module to clear call history(record by module). The information response and causes will indicate
  the command success or failure.

  Syntax: AT#PR
*/
uint8_t OVC3860::clearLocalCallHistory() {
  return OVC3860::sendData(OVC3860_CLEAR_LOCAL_CALL_HISTORY);
}

/*
  SPP data transmit #ST

  Command
  #ST
  Current Status(s)
  When SPP is connected
  Possible Indication(s)
  OK
  IndicationDescription
  Command Accepted


  Description
  This command will send SPP data to the remote device.

  Syntax: AT#STdata
  data: the string you need to send. The max len is 20.
*/
uint8_t OVC3860::sppDataTransmit() {
  return OVC3860::sendData(OVC3860_SPP_DATA_TRANSMIT);
}

/*
  Set Clock Debug Mode #VC
  Command
  #VC
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  IndicationDescription
  Command Accepted

  Description
  This command causes the module to enter clock debug mode. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#VC
*/
uint8_t OVC3860::setClockdebugMode() {
  return OVC3860::sendData(OVC3860_SET_CLOCKDEBUG_MODE);
}

/*
  Speaker Volume Down #VD

  Command
  #VD
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  IndicationDescription
  Command Accepted

  Description
  This command causes the module to decrease the speaker volume. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#VD
*/
uint8_t OVC3860::volumeDown() {
  return OVC3860::sendData(OVC3860_VOLUME_DOWN);
}

/*
  Enter BQB Test Mode #VE

  Command
  #VE
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  IndicationDescription
  Command Accepted

  Description
  This command causes the module to enter test mode. The information response and causes will
  indicate the command success or failure.

  Syntax: AT#VE
*/
uint8_t OVC3860::enterTestMode() {
  return OVC3860::sendData(OVC3860_ENTER_TEST_MODE);
}

/*
  Set to Fixed Frequency #VF

  Command
  #VF
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to work at 2404MHz. The information response and causes will
  indicate the command success or failure.

  Syntax: AT#VF
*/
uint8_t OVC3860::setFixedFrequency() {
  return OVC3860::sendData(OVC3860_SET_FIXED_FREQUENCY);
}

/*
  Enter EMC Test Mode #VG

  Command
  #VG
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to work at 2404MHz. The information response and causes will
  indicate the command success or failure.

  Syntax: AT#VGxx_yy

  xx: if set to be 0~78, the frequency is fixed at (2402+xx)MHz, If set to 88, the frequency is in
  hopping mode.

  yy: set the tx packet type according to the following table.

  Packet Type Value
  DH1          '04
  DH3          '08
  DH5           10
  2DH1         '05
  2DH3          11
  2DH5          13
  3DH1         '06
  3DH3          12
  3DH5          14
*/
uint8_t OVC3860::emcTestMode() {
  return OVC3860::sendData(OVC3860_EMC_TEST_MODE);
}

/*
  Set RF Register #VH

  Command
  #VH
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to set a RF register with a given value. The information
  response and causes will indicate the command success or failure.

  Syntax: AT#VHxx_yy
  xx: a register address
  yy: a byte value
  Example: AT#VH54_88(set RF reg 0x54 to be 0x88)
*/
uint8_t OVC3860::setRFRegister() {
  return OVC3860::sendData(OVC3860_SET_RF_REGISTER);
}

/*
  Start Inquiry #VI

  Command
  #VI
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to inquiry Bluetooth devices. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#VI
*/
uint8_t OVC3860::inquiryStart() {
  return OVC3860::sendData(OVC3860_INQUIRY_START);
}

/*
  Cancel Inquiry #VJ

  Command
  #VJ
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to cancel inquiry Bluetooth devices. The information response
  and causes will indicate the command success or failure.

  Syntax: AT#VJ
*/
uint8_t OVC3860::inquiryStop() {
  return OVC3860::sendData(OVC3860_INQUIRY_STOP);
}

/*
  Speaker Volume Up #VU

  Command
  #VU
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to increase the speaker volume. The information response and
  causes will indicate the command success or failure.

  Syntax: AT#VU
*/
uint8_t OVC3860::volumeUp() {
  return OVC3860::sendData(OVC3860_VOLUME_UP);
}

/*
  Power Off OOL #VX

  Command
  #VX
  Current Status(s)
  Any
  Possible Indication(s)
  OK
  Indication Description
  Command Accepted

  Description
  This command causes the module to power off OOL. The information response and causes will
  indicate the command success or failure.

  Syntax: AT#VX
*/
uint8_t OVC3860::shutdown() {
  return OVC3860::sendData(OVC3860_SHUTDOWN_MODULE);
}
