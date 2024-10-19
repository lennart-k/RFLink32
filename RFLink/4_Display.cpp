// ************************************* //
// * Arduino Project RFLink32        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#include <Arduino.h>
#include "RFLink.h"
#include "3_Serial.h"
#include "4_Display.h"

byte PKSequenceNumber = 0;       // 1 byte packet counter
char dbuffer[60];                // Buffer for message chunk data
char pbuffer[PRINT_BUFFER_SIZE]; // Buffer for complete message data

// ------------------- //
// Display shared func //
// ------------------- //

// Common Header
void display_Header(void)
{
  sprintf_P(dbuffer, PSTR("%s%02X"), PSTR("20;"), PKSequenceNumber++);
  strcat(pbuffer, dbuffer);
}

// Plugin Name
void display_Name(const char *input)
{
  sprintf_P(dbuffer, PSTR(";%s"), input);
  strcat(pbuffer, dbuffer);
}

void display_NameEx(const char *name, unsigned int id)
{
  sprintf_P(dbuffer, PSTR(";%s-%04X"), name, id);
  strcat(pbuffer, dbuffer);
}

// Common Footer
void display_Footer(void)
{
  sprintf_P(dbuffer, PSTR("%s"), PSTR(";\r\n"));
  strcat(pbuffer, dbuffer);
}

// Start message
void display_Splash(void)
{
  sprintf_P(dbuffer, PSTR("%s%d.%d;BUILD=%s"), PSTR(";RFLink_ESP;VER="), BUILDNR, REVNR, PSTR(RFLINK_BUILDNAME));
  strcat(pbuffer, dbuffer);
}

// ID=9999 => device ID (often a rolling code and/or device channel number) (Hexadecimal)
void display_IDn(unsigned long input, byte n)
{
  switch (n)
  {
  case 2:
    sprintf_P(dbuffer, PSTR("%s%02lx"), PSTR(";ID="), input);
    break;
  case 4:
    sprintf_P(dbuffer, PSTR("%s%04lx"), PSTR(";ID="), input);
    break;
  case 6:
    sprintf_P(dbuffer, PSTR("%s%06lx"), PSTR(";ID="), input);
    break;
  case 8:
  default:
    sprintf_P(dbuffer, PSTR("%s%08lx"), PSTR(";ID="), input);
  }
  strcat(pbuffer, dbuffer);
}

void display_IDc(const char *input)
{
  sprintf_P(dbuffer, PSTR("%s"), PSTR(";ID="));
  strcat(pbuffer, dbuffer);
  strcat(pbuffer, input);
}

// SWITCH=A16 => House/Unit code like A1, P2, B16 or a button number etc.
void display_SWITCH(byte input)
{
  sprintf_P(dbuffer, PSTR("%s%02x"), PSTR(";SWITCH="), input);
  strcat(pbuffer, dbuffer);
}

// SWITCH=A16 => House/Unit code like A1, P2, B16 or a button number etc.
void display_SWITCHc(const char *input)
{
  sprintf_P(dbuffer, PSTR("%s"), PSTR(";SWITCH="));
  strcat(pbuffer, dbuffer);
  strcat(pbuffer, input);
}

// CMD=ON => Command (ON/OFF/ALLON/ALLOFF) Additional for Milight: DISCO+/DISCO-/MODE0 - MODE8
void display_CMD(boolean all, byte on)
{
  sprintf_P(dbuffer, PSTR("%s"), PSTR(";CMD="));
  strcat(pbuffer, dbuffer);

  if (all == CMD_All)
  {
    sprintf_P(dbuffer, PSTR("%s"), PSTR("ALL"));
    strcat(pbuffer, dbuffer);
  }

  switch (on)
  {
  case CMD_On:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("ON"));
    break;
  case CMD_Off:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("OFF"));
    break;
  case CMD_Bright:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("BRIGHT"));
    break;
  case CMD_Dim:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("DIM"));
    break;
  case CMD_Up:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("UP"));
    break;
  case CMD_Down:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("DOWN"));
    break;
  case CMD_Stop:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("STOP"));
    break;
  case CMD_Pair:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("PAIR"));
    break;
  case CMD_Unknown:
  default:
    sprintf_P(dbuffer, PSTR("%s"), PSTR("UNKNOWN"));
  }
  strcat(pbuffer, dbuffer);
}

// SET_LEVEL=15 => Direct dimming level setting value (decimal value: 0-15)
void display_SET_LEVEL(byte input)
{
  sprintf_P(dbuffer, PSTR("%s%02d"), PSTR(";SET_LEVEL="), input);
  strcat(pbuffer, dbuffer);
}

// TEMP=9999 => Temperature celcius (hexadecimal), high bit contains negative sign, needs division by 10
void display_TEMP(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";TEMP="), input);
  strcat(pbuffer, dbuffer);
}

// HUM=99 => Humidity (decimal value: 0-100 to indicate relative humidity in %)
void display_HUM(byte input)
{
  sprintf_P(dbuffer, PSTR("%s%02d"), PSTR(";HUM="), input);
  strcat(pbuffer, dbuffer);
}

// BARO=9999 => Barometric pressure (hexadecimal)
void display_BARO(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";BARO="), input);
  strcat(pbuffer, dbuffer);
}

// UV=9999 => UV intensity (hexadecimal)
void display_UV(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";UV="), input);
  strcat(pbuffer, dbuffer);
}

// BAT=OK => Battery status indicator (OK/LOW)
void display_BAT(boolean input)
{
  if (input == true)
    sprintf_P(dbuffer, PSTR("%s"), PSTR(";BAT=OK"));
  else
    sprintf_P(dbuffer, PSTR("%s"), PSTR(";BAT=LOW"));
  strcat(pbuffer, dbuffer);
}

// RAIN=1234 => Total rain in mm. (hexadecimal) 0x8d = 141 decimal = 14.1 mm (needs division by 10)
void display_RAIN(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";RAIN="), input);
  strcat(pbuffer, dbuffer);
}

// WINSP=9999 => Wind speed in km. p/h (hexadecimal) needs division by 10
void display_WINSP(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";WINSP="), input);
  strcat(pbuffer, dbuffer);
}

// AWINSP=9999 => Average Wind speed in km. p/h (hexadecimal) needs division by 10
void display_AWINSP(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";AWINSP="), input);
  strcat(pbuffer, dbuffer);
}

// WINGS=9999 => Wind Gust in km. p/h (hexadecimal)
void display_WINGS(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";WINGS="), input);
  strcat(pbuffer, dbuffer);
}

// WINDIR=123 => Wind direction (integer value from 0-15) reflecting 0-360 degrees in 22.5 degree steps
void display_WINDIR(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%03d"), PSTR(";WINDIR="), input);
  strcat(pbuffer, dbuffer);
}

// WINCHL => wind chill (hexadecimal, see TEMP)
void display_WINCHL(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";WINCHL="), input);
  strcat(pbuffer, dbuffer);
}

// WINTMP=1234 => Wind meter temperature reading (hexadecimal, see TEMP)
void display_WINTMP(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";WINTMP="), input);
  strcat(pbuffer, dbuffer);
}

// CHIME=123 => Chime/Doorbell melody number
void display_CHIME(unsigned int input)
{
  sprintf_P(dbuffer, PSTR("%s%03d"), PSTR(";CHIME="), input);
  strcat(pbuffer, dbuffer);
}

// SMOKEALERT=ON => ON/OFF
void display_SMOKEALERT(boolean input)
{
  if (input == SMOKE_On)
    sprintf_P(dbuffer, PSTR("%s"), PSTR(";SMOKEALERT=ON"));
  else
    sprintf_P(dbuffer, PSTR("%s"), PSTR(";SMOKEALERT=OFF"));
  strcat(pbuffer, dbuffer);
}

// Channel
void display_CHAN(byte channel)
{
  sprintf_P(dbuffer, PSTR("%s%04x"), PSTR(";CHN="), channel);
  strcat(pbuffer, dbuffer);
}

// DEBUG=..... => provide DEBUG Data
void display_DEBUG(byte data[], unsigned int size)
{
  sprintf_P(dbuffer, PSTR("%s"), PSTR(";DEBUG="));
  strcat(pbuffer, dbuffer);

  char buffer[size*2 + 1];
  for (unsigned int i = 0; i < size; i++)
  {
    sprintf_P(buffer+i*2, PSTR("%02x"), data[i]);
  }

  strcat(pbuffer, buffer);
}

// --------------------- //
// get label shared func //
// --------------------- //

char retrieveBuffer[INPUT_COMMAND_SIZE];
char *ptr;
const char c_delim[2] = ";";
char c_label[12];

void retrieve_Init()
{
  memcpy(retrieveBuffer, InputBuffer_Serial, INPUT_COMMAND_SIZE);
  ptr = strtok(retrieveBuffer, c_delim);
}

boolean retrieve_Name(const char *c_Name)
{
  if (ptr != NULL)
  {
    if (strncasecmp(ptr, c_Name, strlen(c_Name)) != 0)
      return false;
    ptr = strtok(NULL, c_delim);
    return true;
  }
  else
    return false;
}

boolean retrieve_hasPrefix(const char* prefix)
{
  if (ptr != NULL && prefix != NULL)
  {
    int prefixLength = strlen(prefix);
    if (strncasecmp(ptr, prefix, prefixLength) != 0)
      return false;
    ptr += prefixLength;
    return true;
  }
  else
    return false;
}

boolean retrieve_decimalNumber(unsigned long &value, byte maxDigits, const char* prefix)
{
  if (ptr != NULL)
  {
    if ((prefix != NULL) && (strncasecmp(ptr, prefix, strlen(prefix)) == 0))
      ptr += strlen(prefix);

    //Make this length check more forgiving by dropping leading 0s
    size_t len = strlen(ptr);

    while(len > 1 && ptr[0] == '0')
    {
        ptr++;
        len--;
    }

    if (len > maxDigits)
      return false;

    for (byte i = 0; i < len; i++)
      if (!isdigit(ptr[i]))
        return false;

    value = strtoul(ptr, NULL, DEC);

    ptr = strtok(NULL, c_delim);
    return true;
  }

  return false;
}

boolean retrieve_hexNumber(unsigned long &value, byte maxNibbles, const char* prefix)
{
  if (ptr != NULL)
  {
    if ((prefix != NULL) && (strncasecmp(ptr, prefix, strlen(prefix)) == 0))
      ptr += strlen(prefix);

    //Make this length check more forgiving by dropping leading 0s
    size_t len = strlen(ptr);

    while(len > 1 && ptr[0] == '0')
    {
        ptr++;
        len--;
    }

    if (len > maxNibbles)
      return false;

    for (byte i = 0; i < len; i++)
      if (!isxdigit(ptr[i]))
        return false;

    value = strtoul(ptr, NULL, HEX);

    ptr = strtok(NULL, c_delim);
    return true;
  }

  return false;
}

boolean retrieve_Command(byte &value, const char* prefix)
{
  if (ptr != NULL)
  {
    if ((prefix != NULL) && (strncasecmp(ptr, prefix, strlen(prefix)) == 0))
      ptr += strlen(prefix);

    if (strlen(ptr) > 7)
      return false;

    for (byte i = 0; i < strlen(ptr); i++)
      if (!isalnum(ptr[i]))
        return false;

    value = str2cmd(ptr); // Get ON/OFF etc. command
    if (value != false)
        ptr = strtok(NULL, c_delim);

    return (value != false);
  }
  
  return false;
}

boolean retrieve_long(unsigned long &value, const char* prefix)
{
    return retrieve_hexNumber(value, 8, prefix);
}

boolean retrieve_word(uint16_t &value, const char* prefix)
{
    unsigned long retrievedValue;
    bool result = retrieve_hexNumber(retrievedValue, 4, prefix);
    if (result)
        value = (uint16_t)retrievedValue;
    return result;
}

boolean retrieve_byte(byte &value, const char* prefix)
{
    unsigned long retrievedValue;
    bool result = retrieve_hexNumber(retrievedValue, 2, prefix);
    if (result)
        value = (byte)retrievedValue;
    return result;
}

boolean retrieve_nibble(byte &value, const char* prefix)
{
    unsigned long retrievedValue;
    bool result = retrieve_hexNumber(retrievedValue, 1, prefix);
    if (result)
        value = (byte)retrievedValue;
    return result;
}

boolean retrieve_ID(unsigned long &ul_ID)
{
    boolean result = retrieve_long(ul_ID, "ID=");
    if (result)
        ul_ID &= 0x03FFFFFF; 
    return result;
}

boolean retrieve_Switch(byte &b_Switch)
{
    return retrieve_byte(b_Switch, "SWITCH=");
}

boolean retrieve_Command(byte &b_Cmd)
{
    return retrieve_Command(b_Cmd, "CMD=");
}

boolean retrieve_Command(byte &b_Cmd, byte &b_Cmd2)
{
  // Command
  char c_Cmd[10];

  if (ptr != NULL)
  {
    strcpy(c_label, "SET_LEVEL=");
    if (strncasecmp(ptr, c_label, strlen(c_label)) == 0)
      ptr += strlen(c_label);

    strcpy(c_label, "CMD=");
    if (strncasecmp(ptr, c_label, strlen(c_label)) == 0)
      ptr += strlen(c_label);

    if (strlen(ptr) > 7)
      return false;

    for (byte i = 0; i < strlen(ptr); i++)
      if (!isalnum(ptr[i]))
        return false;

    strcpy(c_Cmd, ptr);

    b_Cmd2 = str2cmd(c_Cmd); // Get ON/OFF etc. command
    if (b_Cmd2 == false)     // Not a valid command received? ON/OFF/ALLON/ALLOFF
      b_Cmd2 = (byte)strtoul(c_Cmd, NULL, HEX);
    // ON
    switch (b_Cmd2)
    {
    case VALUE_ON:
    case VALUE_ALLON:
      b_Cmd |= B01;
      break;
    }
    // Group
    switch (b_Cmd2)
    {
    case VALUE_ALLON:
    case VALUE_ALLOFF:
      b_Cmd |= B10;
      break;
    }
    // Dimmer
    switch (b_Cmd2)
    {
    case VALUE_ON:
    case VALUE_OFF:
    case VALUE_ALLON:
    case VALUE_ALLOFF:
      b_Cmd2 = 0xFF;
      break;
    }

    ptr = strtok(NULL, c_delim);
    return true;
  }
  else
    return false;
}

boolean retrieve_End()
{
  // End
  if (ptr != NULL)
    return false;
  return true;
}

/*********************************************************************************************\
   Convert string to command code
\*********************************************************************************************/
int str2cmd(const char *command)
{
  if (strcasecmp(command, "ON") == 0)
    return VALUE_ON;
  if (strcasecmp(command, "OFF") == 0)
    return VALUE_OFF;
  if (strcasecmp(command, "ALLON") == 0)
    return VALUE_ALLON;
  if (strcasecmp(command, "ALLOFF") == 0)
    return VALUE_ALLOFF;
  if (strcasecmp(command, "PAIR") == 0)
    return VALUE_PAIR;
  if (strcasecmp(command, "DIM") == 0)
    return VALUE_DIM;
  if (strcasecmp(command, "BRIGHT") == 0)
    return VALUE_BRIGHT;
  if (strcasecmp(command, "UP") == 0)
    return VALUE_UP;
  if (strcasecmp(command, "DOWN") == 0)
    return VALUE_DOWN;
  if (strcasecmp(command, "STOP") == 0)
    return VALUE_STOP;
  if (strcasecmp(command, "CONFIRM") == 0)
    return VALUE_CONFIRM;
  if (strcasecmp(command, "LIMIT") == 0)
    return VALUE_LIMIT;
  return false;
}

void replacechar(char *str, char orig, char rep)
{
  char *ix = str;
  int n = 0;
  while ((ix = strchr(ix, orig)) != NULL)
  {
    *ix++ = rep;
    n++;
  }
}

uint8_t String2GPIO(String sGPIO)
{
  char cGPIO[4];

  sGPIO.trim();
  sGPIO.toCharArray(cGPIO, 4);

  switch (strlen(cGPIO))
  {
  case 1:
    if (isdigit(cGPIO[0]))
      return (cGPIO[0] - '0');
  case 2:
    if ((isdigit(cGPIO[0])) && (isdigit(cGPIO[1])))
      return ((cGPIO[0] - '0') * 10 + (cGPIO[1] - '0'));
  default:
    return NOT_A_PIN;
  }
}

String GPIO2String(uint8_t uGPIO)
{
  if (uGPIO < 40)
    return String(uGPIO);
  else
    return "NOT_A_PIN";
}