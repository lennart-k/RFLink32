//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                 Plugin-073 Deltronic Doorbell                                     ##
//#######################################################################################################
/*********************************************************************************************\
 * This Plugin takes care of reception And sending of the Deltronic doorbell
 *
 * Author  (present)  : StormTeam 2018..2020 - Marc RIVES (aka Couin3)
 * Support (present)  : https://github.com/couin3/RFLink 
 * Author  (original) : StuntTeam & Jonas Jespersen 2015..2016
 * Support (original) : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Technical information:
 *
 * The doorbell uses the UM3750 circuit which sends out a 12 bit signal:
 *
 * AAAAAAAA BBBB
 *
 * A = Always 1
 * B = Address (Can be either 1, 5 or 9)
 *
 * Address 1
 * 20;0D;DEBUG;Pulses=26;Pulses(uSec)=600,1150,525,1175,500,1175,475,1200,500,1175,500,1200,475,1175,475,1200,475,575,1075,575,1075,575,1075,1225,450;
 * 000000001110 
 * Address 5
 * 20;17;DEBUG;Pulses=26;Pulses(uSec)=550,1075,425,1100,400,1125,425,1100,400,1125,400,1150,375,1125,400,1125,375,550,900,1125,375,550,900,1150,375;
 * 000000001010
 * Address 9
 * 20;1B;DEBUG;Pulses=26;Pulses(uSec)=600,1150,500,1175,525,1175,500,1175,500,1175,500,1175,500,1175,475,1200,500,1200,475,575,1075,600,1075,1200,475;
 * 000000000110
 \*********************************************************************************************/
#define DELTRONIC_PLUGIN_ID 073
#define PLUGIN_DESC_073 "Deltronic"

#define DELTRONIC_PULSECOUNT 26

#define LENGTH_DEVIATION 300

#ifdef PLUGIN_073
#include "../4_Display.h"

boolean Plugin_073(byte function, const char *string)
{
    if (RawSignal.Number != DELTRONIC_PULSECOUNT)
        return false;

    unsigned long bitstream = 0L;
    unsigned long checksum = 0L;
    //==================================================================================
    if (RawSignal.Pulses[1] * RawSignal.Multiply > 675)
        return false; // First pulse is start bit and should be short!
                      //==================================================================================
                      // Get all 12 bits
                      //==================================================================================
    for (byte x = 2; x < DELTRONIC_PULSECOUNT; x = x + 2)
    {
        bitstream <<= 1; // Always shift
        if (RawSignal.Pulses[x] * RawSignal.Multiply > 800)
        { // long pulse  (800-1275)
            if (RawSignal.Pulses[x] * RawSignal.Multiply > 1275)
                return false; // pulse too long to be valid
            if (RawSignal.Pulses[x + 1] * RawSignal.Multiply > 675)
                return false; // invalid manchestercode (10 01)

            bitstream |= 0x1; // 10 => 1 bit
        }
        else
        { // short pulse
            if (RawSignal.Pulses[x] * RawSignal.Multiply < 250)
                return false; // too short
            if (RawSignal.Pulses[x + 1] * RawSignal.Multiply < 700)
                return false; // invalid manchestercode (10 01)

            // bitstream |= 0x0; // 01 => 0 bit
        }
    }
    //==================================================================================
    // Perform a quick sanity check
    //==================================================================================
    if (bitstream == 0)
        return false;
    checksum = (bitstream)&0x00000FF0L; // First 8 bits should always be 1
    if (checksum != 0x00000FF0L)
        return false;
    //==================================================================================
    // Prevent repeating signals from showing up
    //==================================================================================
    if ((SignalHash != SignalHashPrevious) || ((RepeatingTimer + 1000) < millis()) || (SignalCRC != bitstream))
        SignalCRC = bitstream; // not seen the RF packet recently
    else
        return true; // already seen the RF packet recently
    //==================================================================================
    // Output
    //==================================================================================
    display_Header();
    display_Name(PSTR("Deltronic"));
    display_IDn((bitstream & 0x0000000FL), 4);
    display_SWITCH(1);
    display_CMD(CMD_Single, CMD_On); // #ALL #ON
    display_CHIME(1);
    display_Footer();
    //==================================================================================
    RawSignal.Repeats = true; // suppress repeats of the same RF packet
    RawSignal.Number = 0;     // do not process the packet any further
    return true;
}
#endif // PLUGIN_073

#ifdef PLUGIN_TX_073
void Deltronic_Send(unsigned long address);

boolean  PluginTX_073(byte function, const char *string)
{
    boolean success = false;
    unsigned long bitstream = 0L;
    //10;DELTRONIC;001c33;1;OFF;
    //012345678901234567890123456
    if (strncasecmp(InputBuffer_Serial + 3, "DELTRONIC;", 10) == 0)
    {
        InputBuffer_Serial[11] = 0x30;
        InputBuffer_Serial[12] = 0x78;
        InputBuffer_Serial[19] = 0;
        bitstream = str2int(InputBuffer_Serial + 11);
        bitstream = (bitstream) | 0x00000FF0L;
        Deltronic_Send(bitstream); // Send RF packet
        success = true;
    }
    return success;
}

void Deltronic_Send(unsigned long address)
{
    byte repeatTimes = 16;
    byte repeat, index;
    int periodLong, periodSync;
    unsigned long bitmask;
    int period = 640;

    periodLong = 2 * period;
    periodSync = 36 * period;

    // Send seperator
    digitalWrite(TX_DATA, HIGH);
    delayMicroseconds(period);

    // Send sync
    digitalWrite(TX_DATA, LOW);
    delayMicroseconds(periodSync);
    digitalWrite(TX_DATA, HIGH);
    delayMicroseconds(period);

    for (repeat = 0; repeat < repeatTimes; repeat++)
    {
        bitmask = 0x00000800L;
        for (index = 0; index < 12; index++)
        {
            if (address & bitmask)
            {
                // Send 1
                digitalWrite(TX_DATA, LOW);
                delayMicroseconds(periodLong);
                digitalWrite(TX_DATA, HIGH);
                delayMicroseconds(period);
            }
            else
            {
                // Send 0
                digitalWrite(TX_DATA, LOW);
                delayMicroseconds(period);
                digitalWrite(TX_DATA, HIGH);
                delayMicroseconds(periodLong);
            }
            bitmask >>= 1;
        }
        // Send sync
        digitalWrite(TX_DATA, LOW);
        delayMicroseconds(periodSync);
        digitalWrite(TX_DATA, HIGH);
        delayMicroseconds(period);
    }
    digitalWrite(TX_DATA, LOW);
}
#endif // PLUGIN_TX_073
