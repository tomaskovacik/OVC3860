/*
        GNU GPL v3
        (C) Tomas Kovacik [nail at nodomain dot sk]
        https://github.com/tomaskovacik/

	example for F-6188 module library
*/


#include "OVC3860.h"
#include <SoftwareSerial.h>

uint16_t BTState;
uint16_t CallState;
uint16_t MusicState;
uint16_t PowerState;
uint16_t ASR;
uint16_t Audio;
uint16_t PhoneState;
uint16_t AutoConnect;
uint16_t AutoAnswer;

SoftwareSerial swSerial(7, 6); //rxPin, txPin, inverse_logic

OVC3860 BT(&swSerial, 5);

void setup() {
  BT.begin(115200);
  Serial.begin(115200);
  Serial.println(F("press h for help"));
}

void loop() {
  if (BTState != BT.ConfigMode) {
    if (Serial.available() > 0)
    {
      // read the incoming byte:
      char c = Serial.read();
      switch (c)
      {
        case '1':
          BT.pairingInit();
          break;
        case '2':
          BT.pairingExit();
          break;
        case '3':
          BT.connectHSHF();
          break;
        case '4':
          BT.disconnectHSHF();
          break;
        case '5':
          BT.callAnswer();
          break;
        case '6':
          BT.callReject();
          break;
        case '7':
          BT.callHangUp();
          break;
        case '8':
          BT.callRedial();
          break;
        case '9':
          BT.voiceDialStart();
          break;
        case '0':
          BT.voiceDialEnd();
          break;
        case 'a':
          BT.micToggle();
          break;
        case 'b':
          BT.transferAudio();
          break;
        case 'c':
          BT.callReleaseReject();
          break;
        case 'd':
          BT.callReleaseAccept();
          break;
        case 'e':
          BT.callHoldAccept();
          break;
        case 'f':
          BT.callConference();
          break;
        case 'g':
          BT.pairingDeleteThenInit();
          break;
        case 'h': //help
          Serial.println(F("Pairing Init                 1"));
          Serial.println(F("Pairing Exit                 2"));
          Serial.println(F("Connect HSHF                 3"));
          Serial.println(F("Disconnect HSHF              4"));
          Serial.println(F("Call Answare                 5"));
          Serial.println(F("Call Reject                  6"));
          Serial.println(F("Call HangUp                  7"));
          Serial.println(F("Call Redial                  8"));
          Serial.println(F("Voice Call Start             9"));
          Serial.println(F("Voice Call Cancel            0"));
          Serial.println(F("MicToggle                    a"));
          Serial.println(F("Transfer audio to/from BT    b"));
          Serial.println(F("Call release and reject      c"));
          Serial.println(F("Call release and accept      d"));
          Serial.println(F("Call hold and accept         e"));
          Serial.println(F("Start conference call        f"));
          Serial.println(F("Pairing Delete Then Init     g"));
          Serial.println(F("Print this help              h"));
          Serial.println(F("Call dial number         i+num"));
          Serial.println(F("Send DTMF                    j"));
          Serial.println(F("Query HFP status             k"));
          Serial.println(F("Reset                        l"));
          Serial.println(F("Music: toggle play/pause     m"));
          Serial.println(F("Music: stop                  n"));
          Serial.println(F("Music: next track            o"));
          Serial.println(F("Music: previous track        p"));
          Serial.println(F("Query configuration          q"));
          Serial.println(F("Enable auto connect          r"));
          Serial.println(F("Disable auto connect         s"));
          Serial.println(F("Connect audio source         t"));
          Serial.println(F("Disconnect audio source      u"));
          Serial.println(F("Change local name       v+name"));
          Serial.println(F("Change pin               w+pin"));
          Serial.println(F("Query AVRCP status           x"));
          Serial.println(F("Enable auto answer           y"));
          Serial.println(F("Disable auto answer          z"));
          Serial.println(F("Music start fast forward     A"));
          Serial.println(F("Music: start rewind          B"));
          Serial.println(F("Music: stop ff/rwd           C"));
          Serial.println(F("Query A2DP Status            D"));
          Serial.println(F("Write to memor  E+addr(4B)_hex"));
          Serial.println(F("Read to memory      F+addr(4B)"));
          Serial.println(F("Switch devices               G"));
          Serial.println(F("Query Version                H"));
          Serial.println(F("Sync phonebook by sim        I"));
          Serial.println(F("Sync phonebook by phone      J"));
          Serial.println(F("Read phonebook next item     K"));
          Serial.println(F("Read phonebook previous item L"));
          Serial.println(F("Phonebook sync by dialer     M"));
          Serial.println(F("Phonebook sync by receiver   N"));
          Serial.println(F("Phonebook sync by missed     O"));
          Serial.println(F("Phonebook sync by last call  P"));
          Serial.println(F("Get local last dialed list   Q"));
          Serial.println(F("Get local last received list R"));
          Serial.println(F("Get local last missed list   S"));
          Serial.println(F("Dial last received call      T"));
          Serial.println(F("Clear local call history     U"));
          Serial.println(F("SPP data transmit            V"));
          Serial.println(F("Set clockdebug mode          W"));
          Serial.println(F("Volume down                  X"));
          Serial.println(F("Enter test mode              Y"));
          Serial.println(F("Set fixed frequency          Z"));
          Serial.println(F("EMC test mode                ="));
          Serial.println(F("Set RF register              -"));
          Serial.println(F("Start inquiry device         ]"));
          Serial.println(F("Stop inquiry device          ["));
          Serial.println(F("Volume up                    ;"));
          Serial.println(F("Shutdown module              '"));
          Serial.println(F("Enter config mode            ."));
          break;
        case 'i':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.callDialNumber(str);
          }
          break;
        case 'j':
          BT.sendDTMF();
          break;
        case 'k':
          BT.queryHFPStatus();
          break;
        case 'l':
          BT.resetModule();
          break;
        case 'm':
          BT.musicTogglePlayPause();
          break;
        case 'n':
          BT.musicStop();
          break;
        case 'o':
          BT.musicNextTrack();
          break;
        case 'p':
          BT.musicPreviousTrack();
          break;
        case 'q':
          BT.queryConfiguration();
          break;
        case 'r':
          BT.autoconnEnable();
          BT.queryConfiguration();
          break;
        case 's':
          BT.autoconnDisable();
          BT.queryConfiguration();
          break;
        case 't':
          BT.avSourceConnect();
          break;
        case 'u':
          BT.avSourceDisconnect();
          break;
        case 'v':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.changeLocalName(str);
          }
          break;
        case 'w':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.changePin(str);
          }
          break;
        case 'x':
          BT.queryAvrcpStatus();
          break;
        case 'y':
          BT.autoAnswerEnable();
          BT.queryConfiguration();
          break;
        case 'z':
          BT.autoAnswerDisable();
          BT.queryConfiguration();
          break;
        case 'A':
          BT.musicStartFF();
          break;
        case 'B':
          BT.musicStartRWD();
          break;
        case 'C':
          BT.musicStopFFRWD();
          break;
        case 'D':
          BT.queryA2DPStatus();
          break;
        case 'E':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.writeToMemory(str);
          }
          break;
        case 'F':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.readToMemory(str);
          }
          break;
        case 'G':
          BT.switchDevices();
          break;
        case 'H':
          BT.queryVersion();
          break;
        case 'I':
          BT.pbSyncBySim();
          break;
        case 'J':
          BT.pbSyncByPhone();
          break;
        case 'K':
          BT.pbReadNextItem();
          break;
        case 'L':
          BT.pbReadPreviousItem();
          break;
        case 'M':
          BT.pbSyncByDialer();
          break;
        case 'N':
          BT.pbSyncByReceiver();
          break;
        case 'O':
          BT.pbSyncByMissed();
          break;
        case 'P':
          BT.pbSyncByLastCall();
          break;
        case 'Q':
          BT.getLocalLastDialedList();
          break;
        case 'R':
          BT.getLocalLastReceivedList();
          break;
        case 'S':
          BT.getLocalLastMissedList();
          break;
        case 'T':
          BT.dialLastReceivedCall();
          break;
        case 'U':
          BT.clearLocalCallHistory();
          break;
        case 'V':
          BT.sppDataTransmit();
          break;
        case 'W':
          BT.setClockdebugMode();
          break;
        case 'X':
          BT.volumeDown();
          break;
        case 'Y':
          BT.enterTestMode();
          break;
        case 'Z':
          BT.setFixedFrequency();
          break;
        case '=':
          BT.emcTestMode();
          break;
        case '-':
          BT.setRFRegister();
          break;
        case ']':
          BT.inquiryStart();
          break;
        case '[':
          BT.inquiryStop();
          break;
        case ';':
          BT.volumeUp();
          break;
        case '\'':
          BT.shutdown();
          break;
        case '.':
          BT.enterConfigMode();
          break;
      }
    }
  }
  else
  {
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      switch (c) {
        case 'h':
          Serial.println(F("read name               n"));
          Serial.println(F("write name              N"));
          Serial.println(F("read pin                p"));
          Serial.println(F("read all PSKeys         A"));
          Serial.println(F("quit config mode        q"));
          Serial.println(F("read baudrate           b"));
          Serial.println(F("setBaudrate:"));
          Serial.println(F("         use B0 for 1200b"));
          Serial.println(F("         use B1 for 2400b"));
          Serial.println(F("         use B2 for 4800b"));
          Serial.println(F("         use B3 for 9600b"));
          Serial.println(F("        use B4 for 14400b"));
          Serial.println(F("        use B5 for 19200b"));
          Serial.println(F("        use B6 for 38400b"));
          Serial.println(F("        use B7 for 57600b"));
          Serial.println(F("       use B8 for 115200b"));
          Serial.println(F("       use B9 for 230400b"));
          Serial.println(F("       use BA for 460800b"));
          Serial.println(F("       use BB for 921600b"));
          break;
        case 'n':
          BT.readName();
          break;
        case 'N':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.writeName(str);
          }
          break;
        case 'A':
          BT.readAllPSK();
          break;
        case 'p':
          BT.readPin();
          break;
        case 'P':
          {
            delay(100);
            String str;
            c = 0;
            while (Serial.available() > 0) {
              c = Serial.read();
              str += c;
            }
            BT.writePin(str);
          }
          break;
        case 'q':
          BT.quitConfigMode();
          break;
        case 'b':
          BT.readBaudRate();
        break;
        case 'B':
        {
          delay(100);
        switch (Serial.read()){
          case OVC3860_BAUDRATE_1200:
            BT.writeBaudRate(OVC3860_BAUDRATE_1200);
          break;
          case OVC3860_BAUDRATE_2400:
            BT.writeBaudRate(OVC3860_BAUDRATE_2400);
          break;
          case OVC3860_BAUDRATE_4800:
            BT.writeBaudRate(OVC3860_BAUDRATE_4800);
          break;
          case OVC3860_BAUDRATE_9600:
            BT.writeBaudRate(OVC3860_BAUDRATE_9600);
          break;
          case OVC3860_BAUDRATE_14400:
            BT.writeBaudRate(OVC3860_BAUDRATE_14400);
          break;
          case OVC3860_BAUDRATE_19200:
            BT.writeBaudRate(OVC3860_BAUDRATE_19200);
          break;
          case OVC3860_BAUDRATE_38400:
            BT.writeBaudRate(OVC3860_BAUDRATE_38400);
          break;
          case OVC3860_BAUDRATE_57600:
            BT.writeBaudRate(OVC3860_BAUDRATE_57600);
          break;
          case OVC3860_BAUDRATE_115200:
            BT.writeBaudRate(OVC3860_BAUDRATE_115200);
          break;
          case OVC3860_BAUDRATE_230400:
            BT.writeBaudRate(OVC3860_BAUDRATE_230400);
          break;
          case OVC3860_BAUDRATE_460800:
            BT.writeBaudRate(OVC3860_BAUDRATE_460800);
          break;
          case OVC3860_BAUDRATE_921600:
            BT.writeBaudRate(OVC3860_BAUDRATE_921600);
          break;
        }
        }
        break;
      }
    }
  }

  BT.getNextEventFromBT();

  if (AutoAnswer != BT.AutoAnswer) {
    switch (BT.AutoAnswer) {
      case BT.On:
        Serial.println(F("Auto answer On"));
        break;
      case BT.Off:
        Serial.println(F("Auto answer Off"));
        break;
    }
    AutoAnswer = BT.AutoAnswer;
  }

  if (AutoConnect != BT.AutoConnect) {
    switch (BT.AutoConnect) {
      case BT.On:
        Serial.println(F("Auto connect On"));
        break;
      case BT.Off:
        Serial.println(F("Auto connect Off"));
        break;
    }
    AutoConnect = BT.AutoConnect;
  }

  if (BTState != BT.BTState) {
    switch (BT.BTState) {
      case BT.On:
        Serial.println(F("BT On"));
        break;
      case BT.Off:
        Serial.println(F("BT Off"));
        break;
      case BT.ConfigMode:
        Serial.println(F("BT Config Mode"));
        break;
    }
    BTState = BT.BTState;
  }

}



