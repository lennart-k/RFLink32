//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                      Plugin-032: AlectoV4                                         ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin takes care of decoding the protocol used for outdoor sensors of the Alecto weather stations 
 * following protocol version 4
 * This Plugin works at least with: Banggood SKU174397, Sako CH113, Homemart/Onemall FD030 and Blokker (Dake) 1730796 outdoor sensors
 * But probably with many others as the OEM sensor is sold under many brand names
 * 
 * Author  (present)  : StormTeam 2018..2020 - Marc RIVES (aka Couin3)
 * Support (present)  : https://github.com/couin3/RFLink 
 * Author  (original) : StuntTeam 2015..2016
 * Support (original) : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Technische informatie:
 * Message Format: (9 nibbles, 36 bits):
 *
 * Format for Temperature Humidity
 *   AAAAAAAA AAAA BCDD EEEE EEEE EEEE FFFFFFFF 
 *   01011100 0001 1000 1111 0111 1011 00001110
     01110000 0000 1111 1011 0000 0000 00000101
     10110101 0000 1x00                01001001

     01000101 1000 0110 1111 0000 1100 00100110
     01011111 1101 1000 0000 1111 0001 00001110
     01000101 1000 0010
 *
 *   A = Rolling Code      --- In rtl_433 the first 4 bits are type
 *   B = 1 (fixed value) --- This is the battery level ( 1 = Ok, 0 = low battery)
 *   C = 0=scheduled transmission, 1=requested transmission (button press)
 *   D = Channel number (00=ch1 01=ch2 10=ch3)
 *   E = Temperature (two's complement)
 *   F = Humidity BCD format
 *
 * 20;3F;DEBUG;Pulses=74;Pulses(uSec)=525,1725,425,3600,425,1725,425,3600,425,3625,425,1725,425,3600,425,1725,425,1725,425,1700,425,3600,425,3600,425,3600,425,1725,425,1725,425,1725,425,1725,425,1725,400,1725,425,3600,425,1725,425,1725,425,1725,425,3600,400,1725,425,1725,425,3625,400,1725,425,1725,425,1750,400,3600,425,1725,400,1750,400,3625,425,1725,400,1725,425;
 * 20;C2;DEBUG;Pulses=76;Pulses(uSec)=325,500,250,1800,375,3650,375,1775,375,3650,375,3650,375,1775,375,3650,375,1800,350,1800,375,3650,375,3650,375,3650,375,3650,375,1775,375,1775,375,1775,375,1775,375,1775,375,1775,375,1775,375,3650,375,3650,375,3650,375,1775,375,3650,375,3650,375,1775,375,1775,375,1775,375,1775,375,1775,375,1775,375,3650,375,3650,375,3650,375,3650,375;
 * 20;3E;DEBUG;Pulses=78;Pulses(uSec)=525,250,500,375,600,1650,450,3550,475,1675,450,3550,475,3550,450,1675,450,3575,450,1675,450,1700,450,1700,450,3575,425,3600,450,3575,475,1700,425,1725,425,1725,425,1725,400,1725,425,1725,425,3625,425,1725,425,1725,425,1725,425,3600,425,1725,400,1725,425,3600,425,1725,425,1725,400,1725,425,3600,400,1725,425,1725,400,3600,425,1725,425,1725,400;
 \*********************************************************************************************/
#define ALECTOV4_PLUGIN_ID 032
#define PLUGIN_DESC_032 "Alecto V4"
#define ALECTOV4_PULSECOUNT 74

#define ALECTOV4_MIDHI_D 550
#define ALECTOV4_PULSEMIN_D 1500
#define ALECTOV4_PULSEMINMAX_D 2500
#define ALECTOV4_PULSEMAXMIN_D 3000

#ifdef PLUGIN_032
#include "../4_Display.h"

boolean Plugin_032(byte function, const char *string)
{
   if (RawSignal.Number < ALECTOV4_PULSECOUNT || RawSignal.Number > (ALECTOV4_PULSECOUNT + 4))
      return false;

   const long ALECTOV4_MIDHI = ALECTOV4_MIDHI_D / RawSignal.Multiply;
   const long ALECTOV4_PULSEMIN = ALECTOV4_PULSEMIN_D / RawSignal.Multiply;
   const long ALECTOV4_PULSEMINMAX = ALECTOV4_PULSEMINMAX_D / RawSignal.Multiply;
   const long ALECTOV4_PULSEMAXMIN = ALECTOV4_PULSEMAXMIN_D / RawSignal.Multiply;


   unsigned long bitstream = 0L;
   int temperature = 0;
   byte humidity = 0; //bitstream2 !
   byte rc = 0;
   byte rc2 = 0;
   byte battery = 0;
   //==================================================================================
   // Get all 36 bits
   //==================================================================================
   byte start = 0;
   if (RawSignal.Number == (ALECTOV4_PULSECOUNT + 4))
      start = 4;
   if (RawSignal.Number == (ALECTOV4_PULSECOUNT + 2))
      start = 2;

   for (byte x = 2 + start; x <= 56 + start; x += 2)
   { // Get first 28 bits
      if (RawSignal.Pulses[x + 1] > ALECTOV4_MIDHI)
         return false;

      bitstream <<= 1; // Always shift
      if (RawSignal.Pulses[x] > ALECTOV4_PULSEMAXMIN)
         bitstream |= 0x1;
      else
      {
         if (RawSignal.Pulses[x] < ALECTOV4_PULSEMIN)
            return false;
         if (RawSignal.Pulses[x] > ALECTOV4_PULSEMINMAX)
            return false;
         // bitstream |= 0x0;
      }
   }

   for (byte x = 58 + start; x <= 72 + start; x = x + 2)
   { // Get remaining 8 bits
      if (RawSignal.Pulses[x + 1] > ALECTOV4_MIDHI)
         return false;

      humidity <<= 1; // Always shift
      if (RawSignal.Pulses[x] > ALECTOV4_PULSEMAXMIN)
         humidity |= 0x1;
      else
      {
         if (RawSignal.Pulses[x] < ALECTOV4_PULSEMIN)
            return false;
         if (RawSignal.Pulses[x] > ALECTOV4_PULSEMINMAX)
            return false;
         // humidity |= 0x0;
      }
   }
   //==================================================================================
   // Perform a quick sanity check
   //==================================================================================
   if (bitstream == 0)
      return false;

   if (humidity == 0)
      return false; // Sanity check

   //==================================================================================
   // Now process the various sensor types
   //==================================================================================
   rc = (bitstream >> 20) & 0xFF;
   rc2 = (bitstream >> 12) & 0xF3; //  Remove battery and button from ID
   /* 
   if (((rc2)&0x08) != 0x08)
      return false; // needs to be 1
   */
   temperature = (bitstream & 0xFFF);
   //fix 12 bit signed number conversion
   if ((temperature & 0x800) == 0x800)
   {
      temperature = 4096 - temperature; // fix for minus temperatures
      if (temperature > 0x258)
         return false;                    // temperature out of range ( > 60.0 degrees)
      temperature = temperature | 0x8000; // turn highest bit on for minus values
   }
   else
   {
      if (temperature > 0x258)
         return false; // temperature out of range ( > 60.0 degrees)
   }
   if (humidity > 99)
      return false;                       // Humidity out of range
   battery = ((bitstream >> 12) & B1000); // get battery indicator
   //==================================================================================
   // Prevent repeating signals from showing up
   //==================================================================================
   unsigned long tmpval = (((bitstream << 8) & 0xFFF0) | humidity); // All but 8 1st ID bits ...

   if ((SignalHash != SignalHashPrevious) || ((RepeatingTimer + 500) < millis()) || (SignalCRC != tmpval))
      SignalCRC = tmpval; // not seen this RF packet recently
   else
      return true; // already seen the RF packet recently
   //==================================================================================
   // Output
   //==================================================================================
   display_Header();
   display_Name(PSTR("Alecto V4"));
   char c_ID[5];
   sprintf(c_ID, "%02x%02x", rc, rc2);
   display_IDc(c_ID);
   display_TEMP(temperature);
   if (humidity < 99) // Only report valid humidty values
      display_HUM(humidity);
   display_BAT(battery);
   display_Footer();
   //==================================================================================
   RawSignal.Repeats = true; // suppress repeats of the same RF packet
   RawSignal.Number = 0;
   return true;
}
#endif // PLUGIN_032
