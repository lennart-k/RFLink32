// ************************************* //
// * Arduino Project RFLink32        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //
#pragma once
#include <Arduino.h>

#define BAUD 115200            // 57600      // Baudrate for serial communication.
#ifndef INPUT_COMMAND_SIZE
    #define INPUT_COMMAND_SIZE 2000 // 60         // Maximum number of characters that a command via serial can be.
#endif
#define FOCUS_TIME_MS 50      // 50         // Duration in mSec. that, after receiving serial data from USB only the serial port is checked.

extern char InputBuffer_Serial[INPUT_COMMAND_SIZE];
extern int serialBufferCursor;

void resetSerialBuffer();

boolean readSerialAndExecute();
boolean CheckMQTT(byte *);