/*
        GNU GPL v3
        (C) Tomas Kovacik [nail at nodomain dot sk]
        https://github.com/tomaskovacik/

	example for F-6188 module library
*/


#include "OVC3860.h"
#include <SoftwareSerial.h>

uint16_t BTState;
uint16_t AVRCPState;
uint16_t A2DPState;
uint16_t HFPState;
uint16_t CallState;
uint16_t MusicState;
uint16_t PowerState;
uint16_t Audio;
uint16_t AutoConnect;
uint16_t AutoAnswer;
uint8_t volume;

SoftwareSerial swSerial(7, 6); //rxPin, txPin, inverse_logic

OVC3860 BT(&swSerial, 5);

void printAudio();
void printBTState();
void printCallState();
void printMusicState();
void printPowerState();
void printAutoConnect();
void printAutoAnswer();
void printAVRCPState();
void printA2DPState();
void printHFPState();
void getInitStates();

void setup() {
  BT.begin();
  Serial.begin(115200);
  Serial.println(F("press h for help"));
  getInitStates();
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
          Serial.println(F("Connect audio(A2DP) source   t"));
          Serial.println(F("Disconnect audio(A2DP)source u"));
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
          Serial.println(F("Read from memory    F+addr(4B)"));
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
          Serial.println(F("Resetin module(HW way)       ,"));
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
          BT.reset();
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
          BT.connectA2DP();
          break;
        case 'u':
          BT.disconnectA2DP();
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
            BT.readFromMemory(str);
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
        case ',':
          BT.resetModule();
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
          Serial.println(F("read mode               m"));
          Serial.println(F("read class of device    c"));
          Serial.println(F("write class of device   C"));
          break;
        case 'c':
          BT.readClassOfDevice();
        break;
        case 'C':
          BT.writeClassOfDevice();
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
            switch (Serial.read()) {
              case '0':
                BT.writeBaudRate(OVC3860_BAUDRATE_1200);
                break;
              case '1':
                BT.writeBaudRate(OVC3860_BAUDRATE_2400);
                break;
              case '2':
                BT.writeBaudRate(OVC3860_BAUDRATE_4800);
                break;
              case '3':
                BT.writeBaudRate(OVC3860_BAUDRATE_9600);
                break;
              case '4':
                BT.writeBaudRate(OVC3860_BAUDRATE_14400);
                break;
              case '5':
                BT.writeBaudRate(OVC3860_BAUDRATE_19200);
                break;
              case '6':
                BT.writeBaudRate(OVC3860_BAUDRATE_38400);
                break;
              case '7':
                BT.writeBaudRate(OVC3860_BAUDRATE_57600);
                break;
              case '8':
                BT.writeBaudRate(OVC3860_BAUDRATE_115200);
                break;
              case '9':
                BT.writeBaudRate(OVC3860_BAUDRATE_230400);
                break;
              case 'A':
                BT.writeBaudRate(OVC3860_BAUDRATE_460800);
                break;
              case 'B'
              :
                BT.writeBaudRate(OVC3860_BAUDRATE_921600);
                break;
            }
          }
          break;
          case 'm':
            BT.readMode();
          break;
      }
    }
  }

  BT.getNextEventFromBT();

  if (BTState != BT.BTState) {
    printBTState();
    BTState = BT.BTState;
  }

  if (CallState != BT.CallState) {
    printCallState();
    CallState = BT.CallState;
  }

  if (MusicState != BT.MusicState) {
    printMusicState();
    MusicState = BT.MusicState;
  }

  if (PowerState != BT.PowerState) {
    printPowerState();
    PowerState = BT.PowerState;
  }

  if (Audio != BT.Audio) {
    printAudio();
    Audio = BT.Audio;
  }

  if (AutoConnect != BT.AutoConnect) {
    printAutoConnect();
    AutoConnect = BT.AutoConnect;
  }

  if (AutoAnswer != BT.AutoAnswer) {
    printAutoAnswer();
    AutoAnswer = BT.AutoAnswer;
  }

  if (HFPState != BT.HFPState) {
    printHFPState();
    HFPState = BT.HFPState;
  }

  if (A2DPState != BT.A2DPState) {
    printA2DPState();
    A2DPState = BT.A2DPState;
  }

  if (AVRCPState != BT.AVRCPState) {
    printAVRCPState();
    AVRCPState = BT.AVRCPState;
  }

  if (volume != BT.volume) {
    Serial.print("Volume: ");Serial.println(BT.volume);
    volume = BT.volume;
  }
}

void getInitStates() {
  BT.queryConfiguration();
  BT.queryHFPStatus();
  BT.queryA2DPStatus();
}


void printHFPState() {
  switch (BT.HFPState) {
    case BT.Ready:
      Serial.println(F("HFP Ready"));
      break;
    case BT.Connecting:
      Serial.println(F("HFP Connecting"));
      break;
    case BT.Connected:
      Serial.println(F("HFP Connected"));
      break;
    case BT.OutgoingCall:
      Serial.println(F("HFP OutgoingCall"));
      break;
    case BT.IncomingCall:
      Serial.println(F("HFP IncomingCall"));
      break;
    case BT.OngoingCall:
      Serial.println(F("HFP OngoingCall"));
      break;
    case BT.Disconnected:
      Serial.println(F("HFP Disconnected"));
      break;
  }
}

void printA2DPState() {
  switch (BT.A2DPState) {
    case BT.Ready:
      Serial.println(F("A2DP ready"));
      break;
    case BT.Initializing:
      Serial.println(F("A2DP initializing"));
      break;
    case BT.SignallingActive:
      Serial.println(F("A2DP signalling active"));
      break;
    case BT.Connected:
      Serial.println(F("A2DP connected"));
      break;
    case BT.Streaming:
      Serial.println(F("A2DP streaming"));
      break;
  }
}

void printAVRCPState() {
  switch (BT.AVRCPState) {
    case BT.Ready:
      Serial.println(F("AVRCP Ready"));
      break;
    case BT.Connecting:
      Serial.println(F("AVRCP Connecting"));
      break;
    case BT.Connected:
      Serial.println(F("AVRCP Connected"));
      break;
  }
}

void printAutoConnect() {
  switch (BT.AutoConnect) {
    case BT.On:
      Serial.println(F("AutoConnect On"));
      break;
    case BT.Off:
      Serial.println(F("AutoConnect Off"));
      break;
  }
}

void printAudio() {
  switch (BT.Audio) {
    case BT.ConfigError:
      Serial.println(F("Audio: config error"));
      break;
    case BT.CodecClosed:
      Serial.println(F("Audio: codec closed"));
      break;
    case BT.ASR_44800:
      Serial.println(F("Audio: 44800kHz"));
      break;
    case BT.ASR_44100:
      Serial.println(F("Audio: 44100kHz"));
      break;
    case BT.ASR_32000:
      Serial.println(F("Audio: 32000kHz"));
      break;
    case BT.ASR_16000:
      Serial.println(F("Audio: 16000kHz"));
      break;
  }
}

void printBTState() {
  switch (BT.BTState) {
    case BT.Disconnected:
      Serial.println(F("BT Disconnected"));
      break;
    case BT.Connected:
      Serial.println(F("BT Connected"));
      break;
    case BT.SPPopened:
      Serial.println(F("BT SPP opened"));
      break;
    case BT.SPPclosed:
      Serial.println(F("BT SPP closed"));
      break;
    case BT.Discoverable:
      Serial.println(F("BT Discoverable"));
      break;
      case BT.Listening:
      Serial.println(F("BT Listening"));
      break;
  }
}

void printAutoAnswer() {
  switch (BT.AutoAnswer) {
    case BT.On:
      Serial.println(F("Auto answer On"));
      break;
    case BT.Off:
      Serial.println(F("Auto answer Off"));
      break;
  }
}

void printCallState() {
  switch (BT.CallState) {
    case BT.Idle:
      Serial.println(F("Call: Idle"));
      break;
    case BT.OutgoingCall:
      Serial.println(F("Call: outgoing call"));
      break;
    case BT.IncomingCall:
      Serial.println(F("Call: incoming call"));
      break;
    case BT.OngoingCall:
      Serial.println(F("Call: ongoing call"));
      break;
  }
}

void printMusicState() {
  switch (BT.MusicState) {
    case BT.Playing:
      Serial.println(F("Music playing."));
      break;
    case BT.FastForwarding:
      Serial.println(F("Music fast forwarding."));
      break;
    case BT.Rewinding:
      Serial.println(F("Music rewinding."));
      break;
    case BT.Idle:
      Serial.println(F("Music stoped/paused."));
      break;
  }
}

void printPowerState() {
  switch (BT.PowerState) {
    case BT.On:
      Serial.println(F("Auto connect On"));
      break;
    case BT.Off:
      Serial.println(F("Auto connect Off"));
      break;
  }
}

