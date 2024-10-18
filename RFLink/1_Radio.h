// ************************************* //
// * Arduino Project RFLink32        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#ifndef Radio_h
#define Radio_h

#include <Arduino.h>
#include "11_Config.h"

#define TRANSMITTER_STABLE_DELAY_US 500 // 500        // Delay to let the transmitter become stable (Note: Aurel RTX MID needs 500µS/0,5ms).
#define PULLUP_RF_RX_DATA false       // false      // Sometimes a pullup in needed on RX data pin

#undef BUILTIN_LED
#define BUILTIN_LED 9

// PIN Definition
//


#ifndef PIN_RF_RX_PMOS
  #define PIN_RF_RX_PMOS NOT_A_PIN // High Side P-MOSFET, active on LOW level
#endif
#ifndef PIN_RF_RX_NMOS
  #define PIN_RF_RX_NMOS NOT_A_PIN // Low Side N-MOSFET, active on HIGH level
#endif
#ifndef PIN_RF_RX_VCC
  #define PIN_RF_RX_VCC NOT_A_PIN  // Power to the receiver on this pin
#endif
#ifndef PIN_RF_RX_GND
  #define PIN_RF_RX_GND NOT_A_PIN  // Ground to the receiver on this pin
#endif
#ifndef PIN_RF_RX_NA
  #define PIN_RF_RX_NA NOT_A_PIN   // Alt. RX_DATA. Forced as input
#endif
#ifndef PIN_RF_RX_DATA
  #define PIN_RF_RX_DATA 21 // On this input, the 433Mhz-RF signal is received. LOW when no signal.
#endif
#ifndef PIN_RF_RX_RESET
  #define PIN_RF_RX_RESET NOT_A_PIN // pin to reset transceiver
#endif
#ifndef PIN_RF_RX_CS
  #define PIN_RF_RX_CS NOT_A_PIN    // Used for SPI "Slave Select"
#endif

#ifndef PIN_RF_TX_PMOS
  #define PIN_RF_TX_PMOS NOT_A_PIN // High Side P-MOSFET, active on LOW level
#endif
#ifndef PPIN_RF_TX_NMOS
  #define PPIN_RF_TX_NMOS NOT_A_PIN // Low Side N-MOSFET, active on HIGH level
#endif
#ifndef PIN_RF_TX_VCC
  #define PIN_RF_TX_VCC NOT_A_PIN  // +5 volt / Vcc power to the transmitter on this pin
#endif
#ifndef PIN_RF_TX_GND
  #define PIN_RF_TX_GND NOT_A_PIN  // Ground power to the transmitter on this pin
#endif
#ifndef PIN_RF_TX_NA
  #define PIN_RF_TX_NA NOT_A_PIN   // Spare RX pin. Forced as input
#endif
#ifndef PIN_RF_TX_DATA
  #define PIN_RF_TX_DATA 2 // Data to the 433Mhz transmitter on this pin
#endif


namespace RFLink { namespace Radio {

    enum States
    {
        Radio_OFF,
        Radio_RX,
        Radio_TX,
        Radio_NA
    };
    extern States current_State;

    enum HardwareType
    {
        HW_basic_t=0,
        #ifndef RFLINK_NO_RADIOLIB_SUPPORT
        HW_RFM69CW_t,
        HW_RFM69HCW_t,
        HW_SX1278_t,
        HW_SX1276_t,
        HW_CC1101_t,
        #endif
        HW_EOF_t,
    };
    #ifndef RFLink_default_Radio_HardwareType
      #define RFLink_default_Radio_HardwareType HardwareType::HW_basic_t
    #endif

    extern HardwareType hardware;

    extern bool hardwareProperlyInitialized;


    extern Config::ConfigItem configItems[];

    namespace pins {
        extern int8_t RX_PMOS;
        extern int8_t RX_NMOS;
        extern int8_t RX_VCC;
        extern int8_t RX_GND;
        extern int8_t RX_NA;
        extern int8_t RX_DATA;
        extern int8_t RX_RESET;
        extern int8_t RX_CS;
        extern boolean PULLUP_RX_DATA;

        extern int8_t TX_PMOS;
        extern int8_t TX_NMOS;
        extern int8_t TX_VCC;
        extern int8_t TX_GND;
        extern int8_t TX_NA;
        extern int8_t TX_DATA;
    }
    

    
    void setup();
    void mainLoop();
    void paramsUpdatedCallback();
    void refreshParametersFromConfig();

    /// Returns the current frequency of the transceiver in Hertz
    int32_t getFrequency();

    /// Sets the frequency of the transceiver in Hertz, and returns the previously set frequency, or 0 if it does not support setting the frequency
    int32_t setFrequency(int32_t newFrequency);


    /**
     * return HardwareType::HW_EOF_t when not found
     * */
    HardwareType hardwareIDFromString(const char *);

    void set_Radio_mode(States new_state, bool force=false);
    void show_Radio_Pin();

    /**
     * don't use directly unless you know what you are doing.
     * */
    void set_Radio_mode_generic(States new_state, bool force=false);
    /**
     * don't use directly unless you know what you are doing.
     * */
    void set_Radio_mode_RFM69(States new_state, bool force=false);
    /**
    * don't use directly unless you know what you are doing.
    * */
    void set_Radio_mode_SX1278(States new_state, bool force=false);
    /**
     * don't use directly unless you know what you are doing.
     * */
    void set_Radio_mode_SX1276(States new_state, bool force=false);
    /**
     * don't use directly unless you know what you are doing.
     * */
    void set_Radio_mode_CC1101(States new_state, bool force=false);
    
    /**
     * don't use directly unless you know what you are doing.
     * */
    void enableRX_generic();
    /**
     * don't use directly unless you know what you are doing.
     * */
    void disableRX_generic();
    /**
     * don't use directly unless you know what you are doing.
     * */
    void enableTX_generic();
    /**
     * don't use directly unless you know what you are doing.
     * */
    void disableTX_generic();

    /**
     * Takes 19-32us to execute
     * @return
     */
    float getCurrentRssi();

    void initializeHardware(HardwareType newHardware, bool force = false);

    bool initialize_SX1278();
    bool initialize_SX1276();
    bool initialize_RFM69();
    bool initialize_CC1101();
}}


#endif // Radio_h