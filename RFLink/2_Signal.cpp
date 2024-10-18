// ************************************* //
// * Arduino Project RFLink32        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Marc RIVES             * //
// * More details in RFLink.ino file   * //
// ************************************* //

#include <Arduino.h>
#include "1_Radio.h"
#include "2_Signal.h"
#include "5_Plugin.h"
#include "4_Display.h"

unsigned long SignalCRC = 0L;   // holds the bitstream value for some plugins to identify RF repeats
unsigned long SignalCRC_1 = 0L; // holds the previous SignalCRC (for mixed burst protocols)
byte SignalHash = 0L;           // holds the processed plugin number
byte SignalHashPrevious = 0L;   // holds the last processed plugin number
unsigned long RepeatingTimer = 0L;

namespace RFLink
{
  namespace Signal
  {

    RawSignalStruct RawSignal = {0, 0, 0, 0, 0UL, false, -9999.0F, EndReasons::Unknown}; // current message

    #define SLICER_DEFAULT_RFM69 Slicer_enum::Legacy
    #define SLICER_DEFAULT_CC1101 Slicer_enum::Legacy
    #define SLICER_DEFAULT_SX1278 Slicer_enum::RSSI_Advanced

    namespace commands
    {
      const char sendRF[] PROGMEM = "sendRF";
      const char testRF[] PROGMEM = "testRF";
      const char testRFMoveForward[] PROGMEM = "testRFMoveForward";
      const char enableVerboseSignalFetchLoop[] PROGMEM = "enableVerboseSignalFetchLoop";
      const char disableVerboseSignalFetchLoop[] PROGMEM = "disableVerboseSignalFetchLoop";
    }

    namespace counters {
      unsigned long int receivedSignalsCount;
      unsigned long int successfullyDecodedSignalsCount;
    }

    namespace runtime {
      bool verboseSignalFetchLoop = false;
      Slicer_enum appliedSlicer = Slicer_enum::Default;
    }

    namespace params
    {
      // All json variable names
      unsigned short int sample_rate;
      unsigned long int min_raw_pulses;
      unsigned long int seek_timeout;
      unsigned long int min_preamble;
      unsigned long int min_pulse_len;
      unsigned long int signal_end_timeout;
      unsigned long int signal_repeat_time;
      unsigned long int scan_high_time;

      Slicer_enum slicer = Slicer_enum::Default;
    }

    const char json_name_sample_rate[] = "sample_rate";
    const char json_name_min_raw_pulses[] = "min_raw_pulses";
    const char json_name_seek_timeout[] = "seek_timeout";
    const char json_name_min_preamble[] = "min_preamble";
    const char json_name_min_pulse_len[] = "min_pulse_len";
    const char json_name_signal_end_timeout[] = "signal_end_timeout";
    const char json_name_signal_repeat_time[] = "signal_repeat_time";
    const char json_name_scan_high_time[] = "scan_high_time";
    const char json_name_slicer[] = "slicer";

    Config::ConfigItem configItems[] = {
            Config::ConfigItem(json_name_sample_rate, Config::SectionId::Signal_id, DEFAULT_RAWSIGNAL_SAMPLE_RATE, paramsUpdatedCallback),
            Config::ConfigItem(json_name_min_raw_pulses, Config::SectionId::Signal_id, MIN_RAW_PULSES, paramsUpdatedCallback),
            Config::ConfigItem(json_name_seek_timeout, Config::SectionId::Signal_id, SIGNAL_SEEK_TIMEOUT_MS, paramsUpdatedCallback),
            Config::ConfigItem(json_name_min_preamble, Config::SectionId::Signal_id, SIGNAL_MIN_PREAMBLE_US, paramsUpdatedCallback),
            Config::ConfigItem(json_name_min_pulse_len, Config::SectionId::Signal_id, MIN_PULSE_LENGTH_US, paramsUpdatedCallback),
            Config::ConfigItem(json_name_signal_end_timeout, Config::SectionId::Signal_id, SIGNAL_END_TIMEOUT_US, paramsUpdatedCallback),
            Config::ConfigItem(json_name_signal_repeat_time, Config::SectionId::Signal_id, SIGNAL_REPEAT_TIME_MS, paramsUpdatedCallback),
            Config::ConfigItem(json_name_scan_high_time, Config::SectionId::Signal_id, SCAN_HIGH_TIME_MS, paramsUpdatedCallback),

            Config::ConfigItem(json_name_slicer, Config::SectionId::Signal_id, Slicer_enum::Default, paramsUpdatedCallback, true),

            Config::ConfigItem()};

    void paramsUpdatedCallback()
    {
      refreshParametersFromConfig();
    }

    void refreshParametersFromConfig(bool triggerChanges)
    {

      Config::ConfigItem *item;
      bool changesDetected = false;

      
      item = Config::findConfigItem(json_name_sample_rate, Config::SectionId::Signal_id);
      if (item->getLongIntValue() != params::sample_rate)
      {
        changesDetected = true;
        params::sample_rate = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_min_raw_pulses, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::min_raw_pulses)
      {
        changesDetected = true;
        params::min_raw_pulses = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_seek_timeout, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::seek_timeout)
      {
        changesDetected = true;
        params::seek_timeout = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_min_preamble, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::min_preamble)
      {
        changesDetected = true;
        params::min_preamble = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_min_pulse_len, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::min_pulse_len)
      {
        changesDetected = true;
        params::min_pulse_len = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_signal_end_timeout, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::signal_end_timeout)
      {
        changesDetected = true;
        params::signal_end_timeout = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_signal_repeat_time, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::signal_repeat_time)
      {
        changesDetected = true;
        params::signal_repeat_time = item->getLongIntValue();
      }

      item = Config::findConfigItem(json_name_scan_high_time, Config::SectionId::Signal_id);
      if (item->getUnsignedLongIntValue() != params::scan_high_time)
      {
        changesDetected = true;
        params::scan_high_time = item->getLongIntValue();
      }


      long int value;
      item = Config::findConfigItem(json_name_slicer, Config::SectionId::Signal_id);
      if(item->isUndefined()){
        if(params::slicer != Slicer_enum::Default)
          changesDetected = true;
        params::slicer = Slicer_enum::Default;
      }
      else {
        value = item->getLongIntValue();
        if (value < Slicer_enum::Default || value >= Slicer_enum::SLICERS_EOF ) {
          Serial.println(F("Invalid Slicer provided, resetting to default value"));
          if(item->canBeNull) {
            item->deleteJsonRecord();
          }
          else {
            item->setLongIntValue(item->getLongIntDefaultValue());
          }
          changesDetected = true;
        } else if (params::slicer != value) {
          changesDetected = true;
          params::slicer = (Slicer_enum) value;
        }
      }

      updateSlicer(params::slicer);


      // Applying changes will happen in mainLoop()
      if (triggerChanges && changesDetected)
      {
        Serial.println(F("Signal parameters have changed."));
        if (AsyncSignalScanner::isStopped())
        {
          AsyncSignalScanner::startScanning();
        }
      }
    }

    void setup()
    {
      refreshParametersFromConfig();
    }

    boolean ScanEvent()
    {
      if (Radio::current_State != Radio::States::Radio_RX)
        return false;

      if (!RawSignal.readyForDecoder)
      {
        if (AsyncSignalScanner::nextPulseTimeoutTime_us > 0 && AsyncSignalScanner::nextPulseTimeoutTime_us < micros())
        { // may be current pulse has now timedout so we have a signal?

          AsyncSignalScanner::onPulseTimerTimeout(); // refresh signal properties

          if (!RawSignal.readyForDecoder) // still dont have a valid signal?
            return false;
        }
        else
          return false;
      }

      counters::receivedSignalsCount++; // we have a signal, let's increment counters

      byte signalWasDecoded = PluginRXCall(0, 0); // Check all plugins to see which plugin can handle the received signal.
      if (signalWasDecoded)
      { // Check all plugins to see which plugin can handle the received signal.
        counters::successfullyDecodedSignalsCount++;
        RepeatingTimer = millis() + params::signal_repeat_time;
      }
      AsyncSignalScanner::startScanning();
      return (signalWasDecoded != 0);
    }

    namespace AsyncSignalScanner
    {
      unsigned long int lastChangedState_us = 0;
      unsigned long int nextPulseTimeoutTime_us = 0;
      bool scanningStopped = true;


      void startScanning()
      {
        scanningStopped = false;
        RawSignal.readyForDecoder = false;
        RawSignal.Number = 0;
        RawSignal.Time = 0;
        RawSignal.Multiply = params::sample_rate;
        lastChangedState_us = 0;
        nextPulseTimeoutTime_us = 0;
        attachInterrupt(digitalPinToInterrupt(Radio::pins::RX_DATA), RX_pin_changed_state, CHANGE);
      }

      void stopScanning()
      {
        scanningStopped = true;
        detachInterrupt(Radio::pins::RX_DATA);
      }

      void IRAM_ATTR RX_pin_changed_state()
      {
        static unsigned long lastChangedState_us = 0;
        unsigned long changeTime_us = micros();

        if (RawSignal.readyForDecoder) // it means previous packet has not been decoded yet, let's forget about it
          return;

        unsigned long pulseLength_us = changeTime_us - lastChangedState_us;
        lastChangedState_us = changeTime_us;

        if (pulseLength_us < params::min_pulse_len)
        {                              // this is too short, noise?
          nextPulseTimeoutTime_us = 0; // stop watching for a timeout
          RawSignal.Number = 0;
          RawSignal.Time = 0;
        }

        int pinState = digitalRead(Radio::pins::RX_DATA);

        if (RawSignal.Time == 0)
        {                    // this is potentially the beginning of a new signal
          if (pinState != 1) // if we get 0 here it means that we are in the middle of a signal, let's forget about it
            return;

          RawSignal.Time = millis(); // record when this signal started
          RawSignal.Multiply = Signal::params::sample_rate;
          nextPulseTimeoutTime_us = changeTime_us + SIGNAL_END_TIMEOUT_US;

          return;
        }

        if (pulseLength_us > SIGNAL_END_TIMEOUT_US)
        { // signal timedout but was not caught by main loop! We will do its job
          onPulseTimerTimeout();
          return;
        }

        RawSignal.Number++;

        if (RawSignal.Number >= RAW_BUFFER_SIZE)
        {                              // this signal has too many pulses and will be discarded
          nextPulseTimeoutTime_us = 0; // stop watching for a timeout
          RawSignal.Number = 0;
          RawSignal.Time = 0;
          //Serial.println("this signal has too many pulses and will be discarded");
          return;
        }

        if (RawSignal.Number == 0 && pulseLength_us < SIGNAL_MIN_PREAMBLE_US)
        {                              // too short preamble, let's drop it
          nextPulseTimeoutTime_us = 0; // stop watching for a timeout
          RawSignal.Number = 0;
          RawSignal.Time = 0;
          //Serial.print("too short preamble, let's drop it:");Serial.println(pulseLength_us);
          return;
        }

        //Serial.print("found pulse #");Serial.println(RawSignal.Number);
        RawSignal.Pulses[RawSignal.Number] = pulseLength_us / Signal::params::sample_rate;
        nextPulseTimeoutTime_us = changeTime_us + SIGNAL_END_TIMEOUT_US;
      }

      void onPulseTimerTimeout()
      {
        if (RawSignal.readyForDecoder)
        { // it means previous packet has not been decoded yet, let's forget about it
          //Serial.println("previous signal not decoded yet, discarding this one");
          nextPulseTimeoutTime_us = 0;
          return;
        }

        /*if (digitalRead(RX_DATA) == HIGH) {   // We have a corrupted packet here
        Serial.println("corrupted signal ends with HIGH");
        RawSignal.Number = 0;
        RawSignal.Time = 0;
        nextPulseTimeoutTime_us = 0;
        return;
      }*/

        if (RawSignal.Number == 0)
        { // timeout on preamble!
          //Serial.println("timeout on preamble");
          nextPulseTimeoutTime_us = 0;
          RawSignal.Number = 0;
          RawSignal.Time = 0;
          return;
        }

        if (RawSignal.Number < MIN_RAW_PULSES)
        { // not enough pulses, we ignore it
          nextPulseTimeoutTime_us = 0;
          RawSignal.Number = 0;
          RawSignal.Time = 0;
          return;
        }

        // finally we have one!
        stopScanning();
        nextPulseTimeoutTime_us = 0;
        RawSignal.Number++;
        RawSignal.Pulses[RawSignal.Number] = SIGNAL_END_TIMEOUT_US / Signal::params::sample_rate;
        //Serial.print("found one packet, marking now for decoding. Pulses = ");Serial.println(RawSignal.Number);
        RawSignal.readyForDecoder = true;
      }
    };

    /*********************************************************************************************\
   Send bitstream to RF - Plugin 004 (Newkaku) special version
\*********************************************************************************************/
    void AC_Send(unsigned long data, byte cmd)
    {
#define AC_FPULSE 260 // Pulse width in microseconds
#define AC_FRETRANS 5 // Number of code retransmissions

      // Serial.println("Send AC");
      // Serial.println(data, HEX);
      // Serial.println(cmd, HEX);

      unsigned long bitstream = 0L;
      byte command = 0;
      // prepare data to send
      for (unsigned short i = 0; i < 32; i++)
      { // reverse data bits
        bitstream <<= 1;
        bitstream |= (data & B1);
        data >>= 1;
      }
      if (cmd != 0xff)
      { // reverse dim bits
        for (unsigned short i = 0; i < 4; i++)
        {
          command <<= 1;
          command |= (cmd & B1);
          cmd >>= 1;
        }
      }

      Radio::set_Radio_mode(Radio::States::Radio_TX);
      noInterrupts();
      // send bits
      for (byte nRepeat = 0; nRepeat < AC_FRETRANS; nRepeat++)
      {
        data = bitstream;
        if (cmd != 0xff)
          cmd = command;
        digitalWrite(Radio::pins::TX_DATA, HIGH);
        //delayMicroseconds(fpulse);  //335
        delayMicroseconds(335);
        digitalWrite(Radio::pins::TX_DATA, LOW);
        delayMicroseconds(AC_FPULSE * 10 + (AC_FPULSE >> 1)); //335*9=3015 //260*10=2600
        for (unsigned short i = 0; i < 32; i++)
        {
          if (i == 27 && cmd != 0xff)
          { // DIM command, send special DIM sequence TTTT replacing on/off bit
            digitalWrite(Radio::pins::TX_DATA, HIGH);
            delayMicroseconds(AC_FPULSE);
            digitalWrite(Radio::pins::TX_DATA, LOW);
            delayMicroseconds(AC_FPULSE);
            digitalWrite(Radio::pins::TX_DATA, HIGH);
            delayMicroseconds(AC_FPULSE);
            digitalWrite(Radio::pins::TX_DATA, LOW);
            delayMicroseconds(AC_FPULSE);
          }
          else
            switch (data & B1)
            {
              case 0:
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE * 5); // 335*3=1005 260*5=1300  260*4=1040
                break;
              case 1:
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE * 5);
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE);
                break;
            }
          //Next bit
          data >>= 1;
        }
        // send dim bits when needed
        if (cmd != 0xff)
        { // need to send DIM command bits
          for (unsigned short i = 0; i < 4; i++)
          { // 4 bits
            switch (cmd & B1)
            {
              case 0:
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE * 5); // 335*3=1005 260*5=1300
                break;
              case 1:
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE * 5);
                digitalWrite(Radio::pins::TX_DATA, HIGH);
                delayMicroseconds(AC_FPULSE);
                digitalWrite(Radio::pins::TX_DATA, LOW);
                delayMicroseconds(AC_FPULSE);
                break;
            }
            //Next bit
            cmd >>= 1;
          }
        }
        //Send termination/synchronisation-signal. Total length: 32 periods
        digitalWrite(Radio::pins::TX_DATA, HIGH);
        delayMicroseconds(AC_FPULSE);
        digitalWrite(Radio::pins::TX_DATA, LOW);
        delayMicroseconds(AC_FPULSE * 40); //31*335=10385 40*260=10400
      }
      // End transmit
      Radio::set_Radio_mode(Radio::States::Radio_RX);
      interrupts();
    }

    void RawSendRF(RawSignalStruct *signal)
    {
      int x;

      Radio::set_Radio_mode(Radio::States::Radio_TX);

      //RawSignal.Pulses[RawSignal.Number]=1;                                   // due to a bug in Arduino 1.0.1

      for (byte y = 0; y <= signal->Repeats; y++)
      { // herhaal verzenden RF code
        x = 1;
        noInterrupts();
        while (x < signal->Number)
        {
          digitalWrite(Radio::pins::TX_DATA, HIGH);
          delayMicroseconds(signal->Pulses[x++] * signal->Multiply); // min een kleine correctie
          digitalWrite(Radio::pins::TX_DATA, LOW);
          delayMicroseconds(signal->Pulses[x++] * signal->Multiply); // min een kleine correctie
        }
        interrupts();
        if (y != signal->Repeats)
          delay(signal->Delay); // Delay buiten het gebied waar de interrupts zijn uitgeschakeld! Anders werkt deze funktie niet.
      }

      Radio::set_Radio_mode(Radio::States::Radio_RX);
    }

    bool getSignalFromJson(RawSignalStruct &signal, const char *json_str)
    {
      int jsonSize = strlen(json_str) * 6;
      DynamicJsonDocument json(jsonSize);

      if (deserializeJson(json, json_str) != DeserializationError::Ok)
      {
        Serial.println(F("An error occured while reading json"));
        return false;
      }

      signal.Number = 0;
      signal.Delay = 0;
      signal.Multiply = params::sample_rate;
      signal.Time = 0UL;

      auto &&root = json.as<JsonObject>();
      JsonArrayConst pulsesJson = root.getMember("pulses");


      signal.Number = pulsesJson.size();

      if(signal.Number < 2) {
        Serial.println(F("error, your signal has 0 pulse defined!"));
        return false;
      }

      if(signal.Number > RAW_BUFFER_SIZE) {
        Serial.printf_P(PSTR("error, your Signal has %i pulses while this supports only %i\r\n"), signal.Number, RAW_BUFFER_SIZE);
        return false;
      }

      int index = 0;
      for (JsonVariantConst pulse : pulsesJson)
      {
        index++;
        signal.Pulses[index] = pulse.as<signed long int>() / params::sample_rate;
        //Serial.printf("Pulse=%i\r\n",signal.Pulses[index]);
      }

      signal.Repeats = root.getMember("repeat").as<signed int>();
      signal.Delay = root.getMember("delay").as<signed int>();

      return true;

    }

    void executeCliCommand(char *cmd)
    {
      static const char error_command_aborted[] PROGMEM = "An error occurred, invalid signal was given. Command aborted!";

      RawSignalStruct signal{};

      char *commaIndex = strchr(cmd, ';');

      if (commaIndex == nullptr)
      {
        Serial.println(F("Error : failed to find ending ';' for the command"));
        return;
      }

      int commandSize = commaIndex - cmd;

      *commaIndex = 0; // replace ';' with null termination

      if (strncasecmp_P(cmd, commands::sendRF, commandSize) == 0)
      {
        if(!getSignalFromJson(signal, commaIndex + 1)) {
          RFLink::sendRawPrint(FPSTR(error_command_aborted), true);
          return;
        }

        Serial.printf_P(PSTR("** sending RF signal with the following properties: pulses=%i, repeat=%i, delay=%i, multiply=%i... "), signal.Number, signal.Repeats, signal.Delay, signal.Multiply);
        RawSendRF(&signal);
        Serial.println(F("done"));
      }
      else if (strncasecmp_P(cmd, commands::testRF, commandSize) == 0)
      {
        RawSignal.readyForDecoder = true;

        if(!getSignalFromJson(RawSignal, commaIndex+1)) {
          Serial.println(FPSTR(error_command_aborted));
          RawSignal.readyForDecoder = false;
          return;
        }

        Serial.printf_P(PSTR("Sending your signal to Plugins (%i pulses)\r\n"), RawSignal.Number);

        if (!PluginRXCall(0, 0)){
          Serial.println(F("No plugin has matched your signal"));
        }
        else
          RFLink::sendMsgFromBuffer();

        RawSignal.readyForDecoder = false;
      }
      else if (strncasecmp_P(cmd, commands::testRFMoveForward, commandSize) == 0)
      {
        if(!getSignalFromJson(RawSignal, commaIndex+1)) {
          Serial.println(FPSTR(error_command_aborted));
          RawSignal.readyForDecoder = false;
          return;
        }

        while(RawSignal.Number >= (int)params::min_raw_pulses) {

          Serial.printf_P(PSTR("Sending your signal to Plugins (%i pulses)\r\n"), RawSignal.Number);
          displaySignal(RawSignal);
          RawSignal.readyForDecoder = true;

          if (!PluginRXCall(0, 0)) {
            Serial.println(F("No plugin has matched your signal"));
          }
          else {
            RFLink::sendMsgFromBuffer();
            break;
          }

          RawSignal.Number -= 2;
          memcpy((void *)&RawSignal.Pulses[1], (void *)&RawSignal.Pulses[3], sizeof(uint16_t)*RawSignal.Number);
          yield();
        }

      }
      else if (strncasecmp_P(cmd, commands::enableVerboseSignalFetchLoop, commandSize) == 0) {
        runtime::verboseSignalFetchLoop = true;
        sendRawPrint(PSTR("30;verboseSignalFetchLoop"));
        sendRawPrint(PSTR(" enabled;"),true);
      }
      else if (strncasecmp_P(cmd, commands::disableVerboseSignalFetchLoop, commandSize) == 0) {
        runtime::verboseSignalFetchLoop = false;
        sendRawPrint(PSTR("30;verboseSignalFetchLoop"));
        sendRawPrint(PSTR(" disabled;"),true);
      }
      else
      {
        Serial.printf_P(PSTR("Error : unknown command '%s'\r\n"), cmd);
      }

      RawSignal.readyForDecoder = false;
    }

    void getStatusJsonString(JsonObject &output)
    {
      auto &&signal = output.createNestedObject("signal");
      signal[F("received_signal_count")] = counters::receivedSignalsCount;
      signal[F("successfully_decoded_count")] = counters::successfullyDecodedSignalsCount;
    }

    void displaySignal(RawSignalStruct &signal) {
      RFLink::sendRawPrint(F("20;XX;DEBUG;Pulses=")); // debug data
      RFLink::sendRawPrint(signal.Number);         // print number of pulses
      RFLink::sendRawPrint(F(";Pulses(uSec)="));      // print pulse durations
      // ----------------------------------
      char dbuffer[10];

      for (int i = 1; i < signal.Number + 1; i++)
      {
        if (QRFDebug == true)
        {
          sprintf(dbuffer, "%02x", signal.Pulses[i]);
          RFLink::sendRawPrint(dbuffer);
        }
        else
        {
          RFLink::sendRawPrint(signal.Pulses[i] * signal.Multiply);
          if (i < signal.Number)
            RFLink::sendRawPrint(',');
        }
      }
      RFLink::sendRawPrint(F(";RSSI="));
      sprintf_P(dbuffer, PSTR("%i;"), (int)signal.rssi);
      RFLink::sendRawPrint(dbuffer);
      RFLink::sendRawPrint(F("\r\n"));
    }

    const char * const EndReasonsStrings[] PROGMEM = {
      "Unknown",
      "ReachedLongPulseTimeOut",
      "AttemptedNoiseFilter",
      "DynamicGapLengthReached",
      "SignalEndTimeout",
      "TooLong",
      "REASONS_EOF"
    };
    static_assert(sizeof(EndReasonsStrings)/sizeof(char *) == EndReasons::REASONS_EOF+1, "EndReasonsStrings has missing/extra names, please compare with EndReasons enum declarations");

    const char * endReasonToString(EndReasons reason) {
      return EndReasonsStrings[(int) reason];
    }

    const char * const SlicerNamesStrings[] PROGMEM = {
            "Legacy",
            "RSSI_advanced"
    };
    static_assert(sizeof(SlicerNamesStrings)/sizeof(char *) == Slicer_enum::SLICERS_EOF, "SlicerNamesStrings has missing/extra names, please compare with Slicer_enum enum declarations");

    const char * slicerIdToString(Slicer_enum slicer) {
      return  SlicerNamesStrings[(int) slicer];
    }

    bool updateSlicer(Slicer_enum newSlicer) {

      runtime::appliedSlicer = Slicer_enum::RSSI_Advanced;

      //sprintf_P(printBuf, PSTR("requested Slicer ID '%i'"), (int) runtime::appliedSlicer);
      //sendRawPrint(printBuf, true);

      if(newSlicer == Slicer_enum::Default){
        #ifndef RFLINK_NO_RADIOLIB_SUPPORT
        if(Radio::hardware == Radio::HardwareType::HW_SX1278_t)
          runtime::appliedSlicer = SLICER_DEFAULT_SX1278;
        else if(Radio::hardware == Radio::HardwareType::HW_RFM69HCW_t || Radio::hardware == Radio::HardwareType::HW_RFM69CW_t )
          runtime::appliedSlicer = SLICER_DEFAULT_RFM69;
        else if(Radio::hardware == Radio::HardwareType::HW_CC1101_t )
          runtime::appliedSlicer = SLICER_DEFAULT_CC1101;
        #endif
      }
      else {
        runtime::appliedSlicer = newSlicer;
      }

      sprintf_P(printBuf, PSTR("Applied slicer '%s'"), slicerIdToString(runtime::appliedSlicer));
      sendRawPrint(printBuf, true);

      return true;
    }

  } // end of ns Signal
} // end of ns RFLink

/*********************************************************************************************/