#include "_Plugin_Config_01.h"
#ifndef Config_h // To help with IDEs completions
using namespace RFLink;
using namespace RFLink::Signal;
using namespace RFLink::Radio;
#endif

#define PLUGIN_DESC_000 "Raw signal handling"
#ifdef PLUGIN_000
#include "../4_Display.h"

namespace PluginRaw
{
   void displaySignal(RawSignalStruct &signal)
   {
      RFLink::sendRawPrint(F("20;XX;RAW;Pulses=")); // debug data
      RFLink::sendRawPrint(signal.Number);          // print number of pulses
      RFLink::sendRawPrint(F(";Pulses(uSec)="));    // print pulse durations
      // ----------------------------------
      char dbuffer[10];

      for (int i = 1; i < signal.Number + 1; i++)
      {
         RFLink::sendRawPrint(signal.Pulses[i] * signal.Multiply);
         if (i < signal.Number)
            RFLink::sendRawPrint(',');
      }
      RFLink::sendRawPrint(F(";RSSI="));
      sprintf_P(dbuffer, PSTR("%i;"), (int)signal.rssi);
      RFLink::sendRawPrint(dbuffer);
      RFLink::sendRawPrint(F("\r\n"));
   }
}

boolean Plugin_000(byte function, const char *string)
{
   PluginRaw::displaySignal(RawSignal);
   return false; // Don't prevent other plugins
}

boolean PluginTX_000(byte function, const char *string)
{
   return false;
}

#endif
