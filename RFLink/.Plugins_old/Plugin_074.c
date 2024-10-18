//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                 Plugin-074: RL-02 Digital Doorbell                   			   ##
//#######################################################################################################
/*********************************************************************************************\
 * This plugin takes care of sending and receiving the RL-02 Digital Doorbell protocol. 
 * 
 * Author  (present)  : StormTeam 2018..2020 - Marc RIVES (aka Couin3)
 * Support (present)  : https://github.com/couin3/RFLink 
 * Author  (original) : Jonas Jespersen 2015..2016
 * Support (original) : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 ***********************************************************************************************
 * Technical information:
 *
 * The RL-02 Digital Doorbell uses a protocol similar to PT2262 which send 12 bits that can be either 
 * 1, 0 or f (float). 
 *
 * The doorbell transmitter has 2 buttons; One for ringing the doorbell and a button inside the transmitter
 * for changing the chime. Everytime the change chime button is pressed the doorbell toggles through a number
 * of available chimes, and when ring button is pressed the chime last selected with the change chime button
 * will sound.
 *
 * AAAAAAAAAAA B
 * 
 * A = Always f0ff0f0ffff
 * B = f or 1 (f = Change chime button, 1 = Ring button)
 *
 * Ring:
 * 20;6D;DEBUG;Pulses=50;Pulses(uSec)=175,400,450,50,100,400,100,400,100,400,450,50,100,400,450,50,100,425,100,400,100,400,450,50,100,400,100,400,100,400,450,50,100,400,425,75,100,400,425,75,100,400,450,75,425,75,425,75,75;
 *
 * Change chime:
 * 20;0B;DEBUG;Pulses=50;Pulses(uSec)=175,400,450,50,100,400,100,400,100,400,450,50,100,400,450,50,100,400,100,400,100,400,425,50,100,400,100,400,100,400,450,50,100,400,425,50,100,400,425,50,100,400,425,75,100,400,425,75,100;
 \*********************************************************************************************/
#define RL02_PLUGIN_ID 074
#define PLUGIN_DESC_074 "Byron MP"

#define RL02_CodeLength 12

#define RL02_T 125 // 175 uS

#ifdef PLUGIN_074
#include "../4_Display.h"

boolean Plugin_074(byte function, const char *string)
{
    if (RawSignal.Number != (RL02_CodeLength * 4) + 2)
        return false;

    unsigned long bitstream = 0L;
    unsigned long checksum = 0L;
    //==================================================================================
    // Get all 12 bits
    //==================================================================================
    byte j = (RL02_T * 2) / RawSignal.Multiply;
    for (byte i = 0; i < RL02_CodeLength; i++)
    {

        if (RawSignal.Pulses[4 * i + 1] < j && RawSignal.Pulses[4 * i + 2] > j && RawSignal.Pulses[4 * i + 3] < j && RawSignal.Pulses[4 * i + 4] > j)
        {                    // 0101
            bitstream >>= 1; // 0
        }
        else if (RawSignal.Pulses[4 * i + 1] < j && RawSignal.Pulses[4 * i + 2] > j && RawSignal.Pulses[4 * i + 3] > j && RawSignal.Pulses[4 * i + 4] < j)
        {
            bitstream >>= 1;                           // 0110
            bitstream |= (1 << (RL02_CodeLength - 1)); // 1
        }
        else if (RawSignal.Pulses[4 * i + 1] > j && RawSignal.Pulses[4 * i + 2] < j && RawSignal.Pulses[4 * i + 3] > j && RawSignal.Pulses[4 * i + 4] < j)
        {                    // 1010
            bitstream >>= 1; // 0
        }
        else
        {
            if (i == 0)
            {
                if (RawSignal.Pulses[4 * i + 1] > j && RawSignal.Pulses[4 * i + 2] > j && RawSignal.Pulses[4 * i + 3] < j && RawSignal.Pulses[4 * i + 4] > j)
                {
                    bitstream >>= 1;                           // 1101
                    bitstream |= (1 << (RL02_CodeLength - 1)); // 1
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
    //==================================================================================
    // Perform a quick sanity check
    //==================================================================================
    if (bitstream == 0)
        return false; // sanity check
    checksum = (bitstream & 0x000007FFL);
    if (checksum != 0x000007ADL)
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
    display_Name(PSTR("Byron MP"));
    display_IDn(((bitstream & 0x00000800L) ? 1 : 0), 4); // ID: 0 = Ring button, 1 = Change chime button
    display_SWITCH(1);
    display_CMD(CMD_Single, CMD_On);
    display_CHIME(1);
    display_Footer();
    // ----------------------------------
    RawSignal.Repeats = true;
    RawSignal.Number = 0;
    return true;
}
#endif //PLUGIN_074

#ifdef PLUGIN_TX_074
void RL02_Send(unsigned long address);

boolean  PluginTX_074(byte function, const char *string)
{
    boolean success = false;
    unsigned long bitstream = 0;
    //10;Byron MP;001c33;1;OFF;
    //012345678901234567890123456
    if (strncasecmp(InputBuffer_Serial + 3, "BYRON MP;", 9) == 0)
    {
        InputBuffer_Serial[10] = 0x30;
        InputBuffer_Serial[11] = 0x78;
        InputBuffer_Serial[18] = 0;
        bitstream = str2int(InputBuffer_Serial + 10);
        bitstream = ((bitstream) << 11) | 0x000007ADL;
        RL02_Send(bitstream); // Send RF packet
        success = true;
    }
    return success;
}

void RL02_Send(unsigned long address)
{
    int fpulse = 175; // Pulse witdh in microseconds
    int fretrans = 7; // Number of code retransmissions
    uint32_t fdatabit;
    uint32_t fdatamask = 0x00000001;
    uint32_t fsendbuff;

    for (int nRepeat = 0; nRepeat <= fretrans; nRepeat++)
    {
        fsendbuff = address;
        // Send command

        for (int i = 0; i < 11; i++)
        { // RL-02 packet is 12 bits
            // read data bit
            fdatabit = fsendbuff & fdatamask; // Get most right bit
            fsendbuff = (fsendbuff >> 1);     // Shift right

            // PT2262 data can be 0, 1 or float.
            if (fdatabit != fdatamask)
            { // Write 0
                digitalWrite(TX_DATA, HIGH);
                delayMicroseconds(fpulse);
                digitalWrite(TX_DATA, LOW);
                delayMicroseconds(fpulse * 3);
                digitalWrite(TX_DATA, HIGH);
                delayMicroseconds(fpulse);
                digitalWrite(TX_DATA, LOW);
                delayMicroseconds(fpulse * 3);
            }
            else
            { // Write float
                digitalWrite(TX_DATA, HIGH);
                delayMicroseconds(fpulse * 1);
                digitalWrite(TX_DATA, LOW);
                delayMicroseconds(fpulse * 3);
                digitalWrite(TX_DATA, HIGH);
                delayMicroseconds(fpulse * 3);
                digitalWrite(TX_DATA, LOW);
                delayMicroseconds(fpulse * 1);
            }
        }

        fdatabit = fsendbuff & fdatamask; // Get most right bit
        // Send last bit. Can be either 1 or float
        if (fdatabit != fdatamask)
        { // Write 1
            digitalWrite(TX_DATA, HIGH);
            delayMicroseconds(fpulse * 3);
            digitalWrite(TX_DATA, LOW);
            delayMicroseconds(fpulse);
            digitalWrite(TX_DATA, HIGH);
            delayMicroseconds(fpulse * 3);
            digitalWrite(TX_DATA, LOW);
            delayMicroseconds(fpulse);
        }
        else
        { // Write float
            digitalWrite(TX_DATA, HIGH);
            delayMicroseconds(fpulse * 1);
            digitalWrite(TX_DATA, LOW);
            delayMicroseconds(fpulse * 3);
            digitalWrite(TX_DATA, HIGH);
            delayMicroseconds(fpulse * 3);
            digitalWrite(TX_DATA, LOW);
            delayMicroseconds(fpulse * 1);
        }

        // Send sync bit
        digitalWrite(TX_DATA, HIGH);
        delayMicroseconds(fpulse * 1);
        digitalWrite(TX_DATA, LOW); // and lower the signal
        delayMicroseconds(fpulse * 31);
    }
}
#endif // PLUGIN_TX_074
