//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                    Plugin-254: Signal Analyzer                                    ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin shows pulse lengths that have been received on RF and have not been decoded by
 * one of the other plugins. The primary use of this plugin is to provide an easy way to debug and 
 * analyse currently unsupported RF signals
 *
 * Author  (present)  : StormTeam 2018..2020 - Marc RIVES (aka Couin3)
 * Support (present)  : https://github.com/couin3/RFLink 
 * Author  (original) : StuntTeam 2015..2016
 * Support (original) : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 ***********************************************************************************************
 * Technical description:
 * This plugin just outputs unsupported  RF packets, use this plugin to find signals from new devices
 * Even if you do not know what to do with the data yourself you might want to share your data so
 * others can analyse it.
 \*********************************************************************************************/
#define P254_PLUGIN_ID 254
#define PLUGIN_DESC_254 "Unidentified Packet debugging"

#ifdef PLUGIN_254
#include "../4_Display.h"

boolean Plugin_254(byte function, const char *string)
{
   int i;

   if ((RFUDebug == false) && (QRFUDebug == false)) // debug is on?
      return false;

   if (RawSignal.Number < 24) // make sure the packet is long enough to have a meaning
      return false;

   // ----------------------------------
   // Output
   // ----------------------------------
   //display_Header();
   //display_Name(PSTR("DEBUG"));
   //display_Footer();
   // ----------------------------------
   RFLink::sendRawPrint(F("20;XX;DEBUG;Pulses=")); // debug data
   RFLink::sendRawPrint(RawSignal.Number);         // print number of pulses
   RFLink::sendRawPrint(F(";Pulses(uSec)="));      // print pulse durations
   // ----------------------------------
   char dbuffer[5];

   for (i = 1; i < RawSignal.Number + 1; i++)
   {
      if (QRFUDebug == true)
      {
         sprintf(dbuffer, "%02x", RawSignal.Pulses[i]);
         RFLink::sendRawPrint(dbuffer);
      }
      else
      {
         RFLink::sendRawPrint(RawSignal.Pulses[i] * RawSignal.Multiply);
         if (i < RawSignal.Number)
            RFLink::sendRawPrint(',');
      }
   }
   RFLink::sendRawPrint(F(";\r\n"));
   // ----------------------------------
   RawSignal.Number = 0; // Last plugin, kill packet
   return true;          // stop processing
}
#endif // PLUGIN_254
