/*
	GNU GPL v3
	(C) Tomas Kovacik [nail at nodomain dot sk]
	https://github.com/tomaskovacik/

	library for bluetooth modules based on OVC3860 chip by omnivision technologies

	supports SW and HW serial

	to enable SW serial set USE_SW_SERIAL to 1
	to enable debug output set DEBUG to 1, this is using HW Serial (hardcoded)

  based on 2 pdfs:
  https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/BLK-MD-SPK-B_AT_Command_set_original.pdf
  https://github.com/tomaskovacik/kicad-library/blob/master/library/datasheet/OVC3860_based_modules/OVC3860_AT_Command_Application_Notes.pdf

  some commands did not work for me (P*,MM,MN...)
  (file BLK-MD-SPK-B_AT_Command_set_original.pdf is marked in header as -B but description is mentioning -A version of module, maybe
  this is why some commands did not work)

  PSKey confguration implemented: used to change name,pin,UART baudrate (this one is problematic currently, works with original soft in one from milion tryes....)

*/

#ifndef OVC3860_h
#define OVC3860_h
#include <Arduino.h>

#define USE_SW_SERIAL 1

#define DEBUG 1

//cmds definitions:
#define OVC3860_PAIRING_INIT "CA" /* discoverable for 2 minutes */
#define OVC3860_PAIRING_EXIT "CB"
#define OVC3860_CONNECT_HSHF "CC"
#define OVC3860_DISCONNECT_HSHF "CD"
#define OVC3860_CALL_ANSWARE "CE"
#define OVC3860_CALL_REJECT "CF"
#define OVC3860_CALL_HANGUP "CG"
#define OVC3860_CALL_REDIAL "CH"
#define OVC3860_VOICE_CALL_START "CI"
#define OVC3860_VOICE_CALL_CANCEL "CJ"
#define OVC3860_MIC_TOGGLE "CM"
#define OVC3860_TRANSFER_AUDIO_TO_SPEAKER "CO" // to secod BT device??? 
#define OVC3860_RELEASE_REJECT_CALL "CQ"
#define OVC3860_RELEASE_ACCEPT_CALL "CR"
#define OVC3860_HOLD_ACCEPT_CALL "CS"
#define OVC3860_CONFERENCE_CALL "CT"
#define OVC3860_PAIRING_DELETE_THEN_INIT "CV"
#define OVC3860_CALL_DIAL_NUMBER "CW"
#define OVC3860_SEND_DTMF "CX"
#define OVC3860_QUERY_HFP_STATUS "CY"
#define OVC3860_RESET "CZ"
#define OVC3860_MUSIC_TOGGLE_PLAY_PAUSE "MA"
#define OVC3860_MUSIC_STOP "MC"
#define OVC3860_MUSIC_NEXT_TRACK "MD"
#define OVC3860_MUSIC_PREVIOUS_TRACK "ME"
#define OVC3860_QUERY_CONFIGURATION "MF"
#define OVC3860_AUTOCONN_ENABLE "MG"
#define OVC3860_AUTOCONN_DISABLE "MH"
#define OVC3860_AV_SOURCE_CONNECT "MI"
#define OVC3860_AV_SOURCE_DISCONNECT "MJ"
#define OVC3860_CHANGE_LOCAL_NAME "MM"
#define OVC3860_CHANGE_PIN "MN"
#define OVC3860_QUERY_AVRCP_STATUS "MO"
#define OVC3860_AUTO_ANSWER_ENABLE "MP"
#define OVC3860_AUTO_ANSWER_DISABLE "MQ"
#define OVC3860_MUSIC_START_FF "MR"
#define OVC3860_MUSIC_START_RWD "MS"
#define OVC3860_MUSIC_STOP_FF_RWD "MT"
#define OVC3860_QUERY_A2DP_STATUS "MV"
#define OVC3860_WRITE_TO_MEMORY "MW"
#define OVC3860_READ_FROM_MEMORY "MX"
#define OVC3860_QUERY_VERSION "MY"
#define OVC3860_SWITCH_DEVICES "MZ"
#define OVC3860_PB_SYNC_BY_SIM "PA"
#define OVC3860_PB_SYNC_BY_PHONE "PB"
#define OVC3860_PB_READ_NEXT_ITEM "PC"
#define OVC3860_PB_READ_PREVIOUS_ITEM "PD"
#define OVC3860_PB_SYNC_BY_DIALED "PH"
#define OVC3860_PB_SYNC_BY_RECEIVED "PI"
#define OVC3860_PB_SYNC_BY_MISSED "PJ"
#define OVC3860_PB_SYNC_BY_LAST_CALL "PK"
#define OVC3860_GET_LOCAL_LAST_DIALED_LIST "PL"
#define OVC3860_GET_LOCAL_LAST_RECEIVED_LIST "PM"
#define OVC3860_GET_LOCAL_LAST_MISSED_LIST "PN"
#define OVC3860_DIAL_LAST_RECEIVED_CALL "PO"
#define OVC3860_CLEAR_LOCAL_CALL_HISTORY "PR"
#define OVC3860_SPP_DATA_TRANSMIT "ST"
#define OVC3860_SET_CLOCKDEBUG_MODE "VC"
#define OVC3860_VOLUME_DOWN "VD"
#define OVC3860_ENTER_TEST_MODE "VE"
#define OVC3860_SET_FIXED_FREQUENCY "VF"
#define OVC3860_EMC_TEST_MODE "VG"
#define OVC3860_SET_RF_REGISTER "VH"
#define OVC3860_INQUIRY_START "VI"
#define OVC3860_INQUIRY_STOP "VJ"
#define OVC3860_VOLUME_UP "VU"
#define OVC3860_SHUTDOWN_MODULE "VX"

#define OVC3860_BAUDRATE_1200 0x00
#define OVC3860_BAUDRATE_2400 0x01
#define OVC3860_BAUDRATE_4800 0x02
#define OVC3860_BAUDRATE_9600 0x03
#define OVC3860_BAUDRATE_14400 0x04
#define OVC3860_BAUDRATE_19200 0x05
#define OVC3860_BAUDRATE_38400 0x06
#define OVC3860_BAUDRATE_57600 0x07
#define OVC3860_BAUDRATE_115200 0x08
#define OVC3860_BAUDRATE_230400 0x09
#define OVC3860_BAUDRATE_460800 0x0A
#define OVC3860_BAUDRATE_921600 0x0B

#if defined(USE_SW_SERIAL)
#if ARDUINO >= 100
#include <SoftwareSerial.h>
#else
#include <NewSoftSerial.h>
#endif
#endif


class OVC3860
{
  public:


/*
from datasheet:
HFP Status Value Description:(MG) => uint8_t HFPState
1. Ready (to be connected)
2. Connecting
3. Connected
4. Outgoing Call
5. Incoming Call
6. Ongoing Call
A2DP Status Value Description:(MU) => uint8_t A2DPState
1. Ready (to be connected)
2. Initializing
3. Signalling Active
4. Connected
5. Streaming
AVRCP Status Value Description:(ML) => uint8_t AVRCPState
1. Ready (to be connected)
2. Connecting
3. Connected
 */
    enum STATES
    {
      Ready, //HFP,A2DP.AVRCP
      Connecting, //HFP.AVRCP
      Connected, //HFP,A2DP.AVRCP
      OutgoingCall,//HFP
      IncomingCall,//HFP
      OngoingCall,//HFP
      Initializing,//A2DP
      SignallingActive,//A2DP
      Streaming,//A2DP
      Playing, //Music
      Idle, //Music = paused/stoped
      On, //PowerState
      Off, //PowerState
      Pairing, //BT
      ShutdownInProgress,
      FastForwarding, //Music
      Rewinding, //Music
      ConfirError, //Audio
      CodecClosed,//Audio
      _44800,//Audio
      _44100,//Audio
      _32000,//Audio
      _16000,//Audio
      PhoneCall,
      Listening,
      PhoneHangUp,
      SPPopened,
      SPPclosed,
      Discoverable,
      Disconnected,
      ConfigMode
    };

    uint16_t BTState = Disconnected;
    uint16_t HFPState = Disconnected;
    uint16_t A2DPState = Disconnected;
    uint16_t AVRCPState = Disconnected;
    uint16_t CallState = Disconnected;
    uint16_t MusicState = Idle;
    uint16_t PowerState = Off;
    uint16_t ASR;
    uint16_t Audio;
    uint16_t PhoneState;
    uint16_t AutoAnswer=Off;
    uint16_t AutoConnect=Off;

    String CallerID;
    String BT_ADDR;
    String BT_NAME;
    String BT_PIN;

#if defined(USE_SW_SERIAL)
#if ARDUINO >= 100
    OVC3860(SoftwareSerial *ser, uint8_t resetPin);
#else
    OVC3860(NewSoftSerial  *ser, uint8_t resetPin);
#endif
#endif
    OVC3860(HardwareSerial *ser, uint8_t resetPin);
    void begin(uint32_t baudrate = 9600);
    ~OVC3860();

    uint8_t sendData(String cmd);
    uint8_t getNextEventFromBT();
    uint8_t sendRawData(uint8_t _size, uint8_t data[]);
    void ResetModule();
    String returnBtModuleName(String receivedString);
    
    uint8_t PairingInit();
    uint8_t PairingExit();
    uint8_t ConnectHSHF();
    uint8_t DisconnectHSHF();
    uint8_t CallAnswer();
    uint8_t CallReject();
    uint8_t CallHangUp();
    uint8_t CallRedial();
    uint8_t VoiceDialStart();
    uint8_t VoiceDialEnd();
    uint8_t MicToggle();
    uint8_t TransferAudio();
    uint8_t CallReleaseReject();
    uint8_t CallReleaseAccept();
    uint8_t CallHoldAccept();
    uint8_t CallConference();
    uint8_t PairingDeleteThenInit();
    uint8_t CallDialNumber(String number);
    uint8_t SendDTMF();
    uint8_t QueryHFPStatus();
    uint8_t Reset();
    uint8_t MusicTogglePlayPause();
    uint8_t MusicStop();
    uint8_t MusicNextTrack();
    uint8_t MusicPreviousTrack();
    uint8_t QueryConfiguration();
    uint8_t AutoconnEnable();
    uint8_t AutoconnDisable();
    uint8_t AvSourceConnect();
    uint8_t AvSourceDisconnect();
    uint8_t ChangeLocalName(String name);
    uint8_t ChangePin(String pin);
    uint8_t QueryAvrcpStatus();
    uint8_t AutoAnswerEnable();
    uint8_t AutoAnswerDisable();
    uint8_t MusicStartFF();
    uint8_t MusicStartRWD();
    uint8_t MusicStopFFRWD();
    uint8_t QueryA2DPStatus();
    uint8_t WriteToMemory(String data);
    uint8_t ReadToMemory(String addr);
    uint8_t SwitchDevices();
    uint8_t QueryVersion();
    uint8_t PbSyncBySim();
    uint8_t PbSyncByPhone();
    uint8_t PbReadNextItem();
    uint8_t PbReadPreviousItem();
    uint8_t PbSyncByDialer();
    uint8_t PbSyncByReceiver();
    uint8_t PbSyncByMissed();
    uint8_t PbSyncByLastCall();
    uint8_t GetLocalLastDialedList();
    uint8_t GetLocalLastReceivedList();
    uint8_t GetLocalLastMissedList();
    uint8_t DialLastReceivedCall();
    uint8_t ClearLocalCallHistory();
    uint8_t SppDataTransmit();
    uint8_t SetClockdebugMode();
    uint8_t VolumeDown();
    uint8_t EnterTestMode();
    uint8_t SetFixedFrequency();
    uint8_t EmcTestMode();
    uint8_t SetRFRegister();
    uint8_t InquiryStart();
    uint8_t InquiryStop();
    uint8_t VolumeUp();
    uint8_t Shutdown();
    uint8_t EnterConfigMode();
    uint8_t QuitConfigMode();
    uint8_t ReadName();
    uint8_t WriteName(String NewName);
    uint8_t ReadAllPSK();
    uint8_t ReadPin();
    uint8_t WritePin(String NewPin);
    uint8_t ReadBaudRate();
    uint8_t WriteBaudRate(uint8_t NewBaudRate = OVC3860_BAUDRATE_115200);
    void resetModule();

  private:
    uint8_t _reset;
    uint8_t decodeReceivedString(String receivedString);
    uint8_t decodeReceivedDataArray(uint8_t data[]);
    void DBG(String text);
    void resetHigh();
    void resetLow();


#if  defined(USE_SW_SERIAL)
#if ARDUINO >= 100
    SoftwareSerial *btSerial;
#else
    NewSoftSerial  *btSerial;
#endif
#else
    HardwareSerial *btSerial;
#endif
};


#endif

