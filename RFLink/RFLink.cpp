// *********************************************************************************************************************************
// * Portions GPLv3 2021 Christophe Painchaud https://github.com/cpainchaud/RFLink32
// * Arduino Project RFLink for ESP architecture https://github.com/couin3/RFLink (Branch esp)
// * Portions Free Software 2018..2020 StormTeam - Marc RIVES
// * Portions Free Software 2015..2016 StuntTeam - (RFLink R29~R33)
// * Portions © Copyright 2010..2015 Paul Tonkes (original Nodo 3.7 code)
// *
// *                                       RFLink32
// *
// ********************************************************************************************************************************
// * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
// * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// * You received a copy of the GNU General Public License along with this program in file 'COPYING.TXT'.
// * For more information on GPL licensing: http://www.gnu.org/licenses
// ********************************************************************************************************************************

// ****************************************************************************
#include <Arduino.h>
#include "RFLink.h"
#include "1_Radio.h"
#include "2_Signal.h"
#include "3_Serial.h"
#include "4_Display.h"
#include "5_Plugin.h"
#include "6_MQTT.h"
#include "9_Serial2Net.h"
#include "10_Wifi.h"
#include "11_Config.h"
#include "12_Portal.h"
#include "13_OTA.h"

#if (defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__))
#include <avr/power.h>
#endif

//****************************************************************************************************************************************
void sendMsgFromBuffer(); // See at bottom

void CallReboot(void) {
  RFLink::sendMsgFromBuffer();
  delay(1);
  ESP.restart();
}

namespace RFLink {

    namespace params {
      const char *ntpServer = "pool.ntp.org";
    }

    struct timeval timeAtBoot;
    struct timeval scheduledRebootTime;
    char printBuf[300];

    void setup() {

      delay(250);         // Time needed to switch back from Upload to Console

      Serial.setRxBufferSize(512);
      Serial.setTimeout(1);

      Serial.begin(BAUD); // Initialise the serial port

      if (gettimeofday(&timeAtBoot, NULL) != 0) {
        Serial.println(F("Failed to obtain time"));
      }

      Serial.println(); // ESP "Garbage" message
      Serial.print(F("Arduino IDE Version :\t"));
      Serial.println(ARDUINO);
      Serial.print(F("ESP SDK version:\t"));
      Serial.println(ESP.getSdkVersion());
      Serial.print(F("Sketch File :\t\t"));
      Serial.println(F(__FILE__)); // "RFLink.ino" version is in 20;00 Message
      Serial.println(F("Compiled on :\t\t" __DATE__ " at " __TIME__));

      display_Header();
      display_Splash();
      display_Footer();

#ifdef SERIAL_ENABLED
      Serial.print(pbuffer);
#endif

      RFLink::Config::setup();
      RFLink::Radio::setup();
      RFLink::Signal::setup();

      RFLink::Wifi::setup();
      RFLink::Portal::init();
      #ifndef RFLINK_MQTT_DISABLED
      RFLink::Mqtt::setup_MQTT();
      #endif // RFLINK_MQTT_DISABLED
      RFLink::Serial2Net::setup();


      PluginInit();
      PluginTXInit();

      Radio::set_Radio_mode(Radio::Radio_OFF);
      Radio::show_Radio_Pin();


#ifdef MQTT_ENABLED
      //RFLink::Mqtt::publishMsg();
#endif

      pbuffer[0] = 0;
      Radio::set_Radio_mode(Radio::Radio_RX);

      RFLink::Portal::start();
      RFLink::Serial2Net::setup();
    }

    void mainLoop() {
      #ifndef RFLINK_MQTT_DISABLED
      RFLink::Mqtt::checkMQTTloop();
      #endif // RFLINK_MQTT_DISABLED
      RFLink::sendMsgFromBuffer();
      RFLink::Wifi::mainLoop();

      RFLink::Serial2Net::serverLoop();

#if defined(SERIAL_ENABLED)
      readSerialAndExecute();
#endif

      if (RFLink::Signal::ScanEvent()) {
        RFLink::sendMsgFromBuffer();
      }

      struct timeval now;
      gettimeofday(&now, nullptr);
      if (scheduledRebootTime.tv_sec != 0 && now.tv_sec > scheduledRebootTime.tv_sec) {
        Serial.println(F("***** Rebooting now for scheduled reboot !!! *****"));
        ESP.restart();
      }

      Radio::mainLoop();
      OTA::mainLoop();
    }

    void sendMsgFromBuffer() {
      if (pbuffer[0] != 0) {

#ifdef SERIAL_ENABLED
        Serial.print(pbuffer);
#endif

#ifndef RFLINK_MQTT_DISABLED
        RFLink::Mqtt::publishMsg();
#endif // !RFLINK_MQTT_DISABLED

        RFLink::Serial2Net::broadcastMessage(pbuffer);

        pbuffer[0] = 0;
      }
    }

    void sendRawPrint(const char *buf, bool end_of_line) {
      if (buf[0] != 0) {

#ifdef SERIAL_ENABLED
        Serial.print(buf);
        if(end_of_line)
          Serial.println();
#endif
        RFLink::Serial2Net::broadcastMessage(buf);
        if(end_of_line)
          RFLink::Serial2Net::broadcastMessage(F("\r\n"));

      }
    }

    void sendRawPrint(long n)
    {
#ifdef SERIAL_ENABLED
      Serial.print(n);
#endif
      RFLink::Serial2Net::broadcastMessage(String(n).c_str());
    }

    void sendRawPrint(unsigned long n)
    {
#ifdef SERIAL_ENABLED
      Serial.print(n);
#endif
      RFLink::Serial2Net::broadcastMessage(String(n).c_str());
    }

    void sendRawPrint(int n)
    {
#ifdef SERIAL_ENABLED
      Serial.print(n);
#endif
      RFLink::Serial2Net::broadcastMessage(String(n).c_str());
    }

    void sendRawPrint(unsigned int n)
    {
#ifdef SERIAL_ENABLED
      Serial.print(n);
#endif
      RFLink::Serial2Net::broadcastMessage(String(n).c_str());
    }

  void sendRawPrint(float f)
  {
#ifdef SERIAL_ENABLED
    Serial.print(f);
#endif
    RFLink::Serial2Net::broadcastMessage(String(f).c_str());
  }

    void sendRawPrint(char c)
    {
#ifdef SERIAL_ENABLED
      Serial.write(c);
#endif
      RFLink::Serial2Net::broadcastMessage(c);
    }

    void sendRawPrint(const __FlashStringHelper *buf, bool end_of_line){
      #ifdef SERIAL_ENABLED
      Serial.print(buf);
      if(end_of_line)
        Serial.println();
      #endif
      RFLink::Serial2Net::broadcastMessage(buf);
      if(end_of_line)
        RFLink::Serial2Net::broadcastMessage(F("\r\n"));
    };

    /**
    */
    bool executeCliCommand(char *cmd) {
      static byte ValidCommand = 0;

      // Copy input command to InputBuffer_Serial, because many plugins are based on it !
      if(cmd != InputBuffer_Serial) { // sometimes we already have the command in the right buffer
        memcpy(InputBuffer_Serial, cmd, INPUT_COMMAND_SIZE);;
      }

      if (strlen(cmd) > 7) { // need to see minimal 8 characters on the serial port
        // 10;....;..;ON;
        if (strncmp(cmd, "10;", 3) == 0) { // Command from Master to RFLink
          // -------------------------------------------------------
          // Handle Device Management Commands
          // -------------------------------------------------------
          if (strncasecmp(cmd + 3, "PING;", 5) == 0) {
            display_Header();
            display_Name(PSTR("PONG"));
            display_Footer();
          } else if (strncasecmp(cmd + 3, "REBOOT;", 7) == 0) {
            display_Header();
            display_Name(PSTR("REBOOT"));
            display_Footer();
            CallReboot();
          } else if (strncasecmp(cmd + 3, "RFDEBUG=O", 9) == 0) {
            if (cmd[12] == 'N' || cmd[12] == 'n') {
              RFDebug = true;    // full debug on
              QRFDebug = false;  // q full debug off
              RFUDebug = false;  // undecoded debug off
              QRFUDebug = false; // q undecoded debug off
              display_Header();
              display_Name(PSTR("RFDEBUG=ON"));
              display_Footer();
            } else {
              RFDebug = false; // full debug off
              display_Header();
              display_Name(PSTR("RFDEBUG=OFF"));
              display_Footer();
            }
          } else if (strncasecmp(cmd + 3, "RFUDEBUG=O", 10) == 0) {
            if (cmd[13] == 'N' || cmd[13] == 'n') {
              RFDebug = false;   // full debug off
              QRFDebug = false;  // q debug off
              RFUDebug = true;   // undecoded debug on
              QRFUDebug = false; // q undecoded debug off
              display_Header();
              display_Name(PSTR("RFUDEBUG=ON"));
              display_Footer();
            } else {
              RFUDebug = false; // undecoded debug off
              display_Header();
              display_Name(PSTR("RFUDEBUG=OFF"));
              display_Footer();
            }
          } else if (strncasecmp(cmd + 3, "QRFDEBUG=O", 10) == 0) {
            if (cmd[13] == 'N' || cmd[13] == 'n') {
              RFDebug = false;   // full debug off
              QRFDebug = true;   // q debug on
              RFUDebug = false;  // undecoded debug off
              QRFUDebug = false; // q undecoded debug off
              display_Header();
              display_Name(PSTR("QRFDEBUG=ON"));
              display_Footer();
            } else {
              QRFDebug = false; // q debug off
              display_Header();
              display_Name(PSTR("QRFDEBUG=OFF"));
              display_Footer();
            }
          } else if (strncasecmp(cmd + 3, "QRFUDEBUG=O", 11) == 0) {
            if (cmd[14] == 'N' || cmd[14] == 'n') {
              RFDebug = false;  // full debug off
              QRFDebug = false; // q debug off
              RFUDebug = false; // undecoded debug off
              QRFUDebug = true; // q undecoded debug on
              display_Header();
              display_Name(PSTR("QRFUDEBUG=ON"));
              display_Footer();
            } else {
              QRFUDebug = false; // q undecode debug off
              display_Header();
              display_Name(PSTR("QRFUDEBUG=OFF"));
              display_Footer();
            }
          } else if (strncasecmp(cmd + 3, "VERSION", 7) == 0) {
            display_Header();
            display_Splash();
            display_Footer();
          } else if (strncasecmp(cmd + 3, "signal", 6) == 0) {
            Signal::executeCliCommand(cmd + 3 + 6 + 1);
          } else if (strncasecmp(cmd + 3, "config", 6) == 0) {
            Config::executeCliCommand(cmd + 3 + 6 + 1);
          } else {
            // -------------------------------------------------------
            // Handle Generic Commands / Translate protocol data into Nodo text commands
            // -------------------------------------------------------
            if (Radio::pins::TX_DATA == NOT_A_PIN) {
              display_Header();
              display_Name(PSTR("ERROR: TX_DATA PIN is not defined!"));
              display_Footer();
            }
            else {
              Radio::set_Radio_mode(Radio::Radio_TX);
              if (PluginTXCall(0, cmd))
                ValidCommand = 1;
              else // Answer that an invalid command was received?
                ValidCommand = 2;

              Radio::set_Radio_mode(Radio::Radio_RX);
            }
          }
        }
      } // if > 7
      if (ValidCommand != 0) {
        display_Header();
        if (ValidCommand == 1)
          display_Name(PSTR("OK"));
        else
          display_Name(PSTR("CMD UNKNOWN"));
        display_Footer();
      }
      ValidCommand = 0;
      sendMsgFromBuffer(); // in case there is a response waiting to be sent
      resetSerialBuffer();
      return true;
    }

    void scheduleReboot(unsigned int seconds) {
      gettimeofday(&scheduledRebootTime, NULL);
      scheduledRebootTime.tv_sec += seconds;
    }

    void getStatusJsonString(JsonObject &output) {

      char buffer[90];
      struct timeval now;

      if (gettimeofday(&now, NULL) != 0) {
        RFLink::sendRawPrint(F("Failed to obtain time"));
      }

      output["uptime"] = now.tv_sec - timeAtBoot.tv_sec;

      output["heap_free"] = ESP.getFreeHeap();
      output["heap_min_free"] = ESP.getMinFreeHeap();
      output["heap_max_alloc"] = ESP.getMaxAllocHeap();

      sprintf_P(buffer, PSTR("RFLink_ESP_%d.%d-%s"), BUILDNR, REVNR, PSTR(RFLINK_BUILDNAME));
      output["sw_version"] = buffer;
    }

}

/*********************************************************************************************/