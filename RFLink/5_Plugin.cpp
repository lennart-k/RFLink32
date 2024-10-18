// ************************************* //
// * Arduino Project RFLink32        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#include <Arduino.h>
#include "RFLink.h"
#include "2_Signal.h"
#include "5_Plugin.h"
#include "7_Utils.h"

using namespace RFLink::Utils;
using namespace RFLink::Signal;
using namespace RFLink;

boolean (*Plugin_ptr[PLUGIN_MAX])(byte, const char *); // Receive plugins
byte Plugin_State[PLUGIN_MAX];

boolean (*PluginTX_ptr[PLUGIN_TX_MAX])(byte, const char *); // Trasmit plugins
byte PluginTX_State[PLUGIN_TX_MAX];

boolean RFDebug = RFDebug_0;     // debug RF signals with plugin 001 (no decode)
boolean QRFDebug = QRFDebug_0;   // debug RF signals with plugin 001 but no multiplication (faster?, compact)
boolean RFUDebug = RFUDebug_0;   // debug RF signals with plugin 254 (decode 1st)
boolean QRFUDebug = QRFUDebug_0; // debug RF signals with plugin 254 but no multiplication (faster?, compact)

/**********************************************************************************************\
 * Load plugins
\*********************************************************************************************/
#include "./Plugins/_Plugin_Config_01.h"

#ifdef PLUGIN_000
#include "./Plugins/Plugin_000.cpp"
#endif

#ifdef PLUGIN_001
#include "./Plugins/Plugin_001.c"
#endif

#ifdef PLUGIN_002
#include "./Plugins/Plugin_002.c"
#endif

#ifdef PLUGIN_003
#include "./Plugins/Plugin_003.cpp"
#endif

#ifdef PLUGIN_004
#include "./Plugins/Plugin_004.c"
#endif

#ifdef PLUGIN_005
#include "./Plugins/Plugin_005.c"
#endif

#ifdef PLUGIN_006
#include "./Plugins/Plugin_006.c"
#endif

#ifdef PLUGIN_007
#include "./Plugins/Plugin_007.c"
#endif

#ifdef PLUGIN_008
#include "./Plugins/Plugin_008.c"
#endif

#ifdef PLUGIN_009
#include "./Plugins/Plugin_009.c"
#endif

#ifdef PLUGIN_010
#include "./Plugins/Plugin_010.c"
#endif

#ifdef PLUGIN_011
#include "./Plugins/Plugin_011.c"
#endif

#ifdef PLUGIN_012
#include "./Plugins/Plugin_012.c"
#endif

#ifdef PLUGIN_013
#include "./Plugins/Plugin_013.c"
#endif

#ifdef PLUGIN_014
#include "./Plugins/Plugin_014.c"
#endif

#ifdef PLUGIN_015
#include "./Plugins/Plugin_015.c"
#endif

#ifdef PLUGIN_016
#include "./Plugins/Plugin_016.c"
#endif

#ifdef PLUGIN_017
#include "./Plugins/Plugin_017.c"
#endif

#ifdef PLUGIN_018
#include "./Plugins/Plugin_018.c"
#endif

#ifdef PLUGIN_019
#include "./Plugins/Plugin_019.c"
#endif

#ifdef PLUGIN_020
#include "./Plugins/Plugin_020.c"
#endif

#ifdef PLUGIN_021
#include "./Plugins/Plugin_021.c"
#endif

#ifdef PLUGIN_022
#include "./Plugins/Plugin_022.c"
#endif

#ifdef PLUGIN_023
#include "./Plugins/Plugin_023.c"
#endif

#ifdef PLUGIN_024
#include "./Plugins/Plugin_024.c"
#endif

#ifdef PLUGIN_025
#include "./Plugins/Plugin_025.c"
#endif

#ifdef PLUGIN_026
#include "./Plugins/Plugin_026.c"
#endif

#ifdef PLUGIN_027
#include "./Plugins/Plugin_027.c"
#endif

#ifdef PLUGIN_028
#include "./Plugins/Plugin_028.c"
#endif

#ifdef PLUGIN_029
#include "./Plugins/Plugin_029.c"
#endif

#ifdef PLUGIN_030
#include "./Plugins/Plugin_030.c"
#endif

#ifdef PLUGIN_031
#include "./Plugins/Plugin_031.c"
#endif

#ifdef PLUGIN_032
#include "./Plugins/Plugin_032.c"
#endif

#ifdef PLUGIN_033
#include "./Plugins/Plugin_033.c"
#endif

#ifdef PLUGIN_034
#include "./Plugins/Plugin_034.c"
#endif

#ifdef PLUGIN_035
#include "./Plugins/Plugin_035.c"
#endif

#ifdef PLUGIN_036
#include "./Plugins/Plugin_036.c"
#endif

#ifdef PLUGIN_037
#include "./Plugins/Plugin_037.c"
#endif

#ifdef PLUGIN_038
#include "./Plugins/Plugin_038.c"
#endif

#ifdef PLUGIN_039
#include "./Plugins/Plugin_039.c"
#endif

#ifdef PLUGIN_040
#include "./Plugins/Plugin_040.c"
#endif

#ifdef PLUGIN_041
#include "./Plugins/Plugin_041.c"
#endif

#ifdef PLUGIN_042
#include "./Plugins/Plugin_042.c"
#endif

#ifdef PLUGIN_043
#include "./Plugins/Plugin_043.c"
#endif

#ifdef PLUGIN_044
#include "./Plugins/Plugin_044.c"
#endif

#ifdef PLUGIN_045
#include "./Plugins/Plugin_045.c"
#endif

#ifdef PLUGIN_046
#include "./Plugins/Plugin_046.c"
#endif

#ifdef PLUGIN_047
#include "./Plugins/Plugin_047.c"
#endif

#ifdef PLUGIN_048
#include "./Plugins/Plugin_048.c"
#endif

#ifdef PLUGIN_049
#include "./Plugins/Plugin_049.c"
#endif

#ifdef PLUGIN_050
#include "./Plugins/Plugin_050.c"
#endif

#ifdef PLUGIN_051
#include "./Plugins/Plugin_051.c"
#endif

#ifdef PLUGIN_052
#include "./Plugins/Plugin_052.c"
#endif

#ifdef PLUGIN_053
#include "./Plugins/Plugin_053.c"
#endif

#ifdef PLUGIN_054
#include "./Plugins/Plugin_054.c"
#endif

#ifdef PLUGIN_055
#include "./Plugins/Plugin_055.c"
#endif

#ifdef PLUGIN_056
#include "./Plugins/Plugin_056.c"
#endif

#ifdef PLUGIN_057
#include "./Plugins/Plugin_057.c"
#endif

#ifdef PLUGIN_058
#include "./Plugins/Plugin_058.c"
#endif

#ifdef PLUGIN_059
#include "./Plugins/Plugin_059.c"
#endif

#ifdef PLUGIN_060
#include "./Plugins/Plugin_060.c"
#endif

#ifdef PLUGIN_061
#include "./Plugins/Plugin_061.c"
#endif

#ifdef PLUGIN_062
#include "./Plugins/Plugin_062.c"
#endif

#ifdef PLUGIN_063
#include "./Plugins/Plugin_063.c"
#endif

#ifdef PLUGIN_064
#include "./Plugins/Plugin_064.c"
#endif

#ifdef PLUGIN_065
#include "./Plugins/Plugin_065.c"
#endif

#ifdef PLUGIN_066
#include "./Plugins/Plugin_066.c"
#endif

#ifdef PLUGIN_067
#include "./Plugins/Plugin_067.c"
#endif

#ifdef PLUGIN_068
#include "./Plugins/Plugin_068.c"
#endif

#ifdef PLUGIN_069
#include "./Plugins/Plugin_069.c"
#endif

#ifdef PLUGIN_070
#include "./Plugins/Plugin_070.c"
#endif

#ifdef PLUGIN_071
#include "./Plugins/Plugin_071.c"
#endif

#ifdef PLUGIN_072
#include "./Plugins/Plugin_072.c"
#endif

#ifdef PLUGIN_073
#include "./Plugins/Plugin_073.c"
#endif

#ifdef PLUGIN_074
#include "./Plugins/Plugin_074.c"
#endif

#ifdef PLUGIN_075
#include "./Plugins/Plugin_075.c"
#endif

#ifdef PLUGIN_076
#include "./Plugins/Plugin_076.cpp"
#endif

#ifdef PLUGIN_077
#include "./Plugins/Plugin_077.c"
#endif

#ifdef PLUGIN_078
#include "./Plugins/Plugin_078.c"
#endif

#ifdef PLUGIN_079
#include "./Plugins/Plugin_079.c"
#endif

#ifdef PLUGIN_080
#include "./Plugins/Plugin_080.c"
#endif

#ifdef PLUGIN_081
#include "./Plugins/Plugin_081.c"
#endif

#ifdef PLUGIN_082
#include "./Plugins/Plugin_082.c"
#endif

#ifdef PLUGIN_254
#include "./Plugins/Plugin_254.c"
#endif

/*********************************************************************************************/
void PluginInit(void)
{
  byte x;

  // Wis de pointertabel voor de plugins.
  for (x = 0; x < PLUGIN_MAX; x++)
  {
    Plugin_ptr[x] = 0;
    Plugin_State[x] = P_Disabled;
  }

  x = 0;

#ifdef PLUGIN_000
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_000;
#endif

#ifdef PLUGIN_001
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_001;
#endif

#ifdef PLUGIN_002
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_002;
#endif

#ifdef PLUGIN_003
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_003;
#endif

#ifdef PLUGIN_004
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_004;
#endif

#ifdef PLUGIN_005
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_005;
#endif

#ifdef PLUGIN_006
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_006;
#endif

#ifdef PLUGIN_007
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_007;
#endif

#ifdef PLUGIN_008
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_008;
#endif

#ifdef PLUGIN_009
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_009;
#endif

#ifdef PLUGIN_010
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_010;
#endif

#ifdef PLUGIN_011
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_011;
#endif

#ifdef PLUGIN_012
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_012;
#endif

#ifdef PLUGIN_013
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_013;
#endif

#ifdef PLUGIN_014
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_014;
#endif

#ifdef PLUGIN_015
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_015;
#endif

#ifdef PLUGIN_016
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_016;
#endif

#ifdef PLUGIN_017
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_017;
#endif

#ifdef PLUGIN_018
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_018;
#endif

#ifdef PLUGIN_019
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_019;
#endif

#ifdef PLUGIN_020
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_020;
#endif

#ifdef PLUGIN_021
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_021;
#endif

#ifdef PLUGIN_022
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_022;
#endif

#ifdef PLUGIN_023
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_023;
#endif

#ifdef PLUGIN_024
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_024;
#endif

#ifdef PLUGIN_025
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_025;
#endif

#ifdef PLUGIN_026
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_026;
#endif

#ifdef PLUGIN_027

  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_027;
#endif

#ifdef PLUGIN_028

  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_028;
#endif

#ifdef PLUGIN_029
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_029;
#endif

#ifdef PLUGIN_030
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_030;
#endif

#ifdef PLUGIN_031
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_031;
#endif

#ifdef PLUGIN_032
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_032;
#endif

#ifdef PLUGIN_033
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_033;
#endif

#ifdef PLUGIN_034
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_034;
#endif

#ifdef PLUGIN_035
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_035;
#endif

#ifdef PLUGIN_036
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_036;
#endif

#ifdef PLUGIN_037
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_037;
#endif

#ifdef PLUGIN_038
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_038;
#endif

#ifdef PLUGIN_039
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_039;
#endif

#ifdef PLUGIN_040
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_040;
#endif

#ifdef PLUGIN_041
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_041;
#endif

#ifdef PLUGIN_042
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_042;
#endif

#ifdef PLUGIN_043
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_043;
#endif

#ifdef PLUGIN_044
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_044;
#endif

#ifdef PLUGIN_045
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_045;
#endif

#ifdef PLUGIN_046
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_046;
#endif

#ifdef PLUGIN_047
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_047;
#endif

#ifdef PLUGIN_048
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_048;
#endif

#ifdef PLUGIN_049
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_049;
#endif

#ifdef PLUGIN_050
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_050;
#endif

#ifdef PLUGIN_051
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_051;
#endif

#ifdef PLUGIN_052
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_052;
#endif

#ifdef PLUGIN_053
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_053;
#endif

#ifdef PLUGIN_054
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_054;
#endif

#ifdef PLUGIN_055
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_055;
#endif

#ifdef PLUGIN_056
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_056;
#endif

#ifdef PLUGIN_057
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_057;
#endif

#ifdef PLUGIN_058
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_058;
#endif

#ifdef PLUGIN_059
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_059;
#endif

#ifdef PLUGIN_065
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_065;
#endif

#ifdef PLUGIN_066
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_066;
#endif

#ifdef PLUGIN_067
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_067;
#endif

#ifdef PLUGIN_068
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_068;
#endif

#ifdef PLUGIN_069
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_069;
#endif

#ifdef PLUGIN_076
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_076;
#endif

#ifdef PLUGIN_077
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_077;
#endif

#ifdef PLUGIN_078
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_078;
#endif

#ifdef PLUGIN_079
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_079;
#endif

#ifdef PLUGIN_080
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_080;
#endif

#ifdef PLUGIN_081
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_081;
#endif

#ifdef PLUGIN_082
  Plugin_State[x] = P_Enabled;
  Plugin_ptr[x++] = &Plugin_082;
#endif

  // Initialiseer alle plugins door aanroep met verwerkingsparameter PLUGIN_INIT
  PluginInitCall(0, 0);
}
/*********************************************************************************************/
void PluginTXInit(void)
{
  byte x;

  // Wis de pointertabel voor de plugins.
  for (x = 0; x < PLUGIN_TX_MAX; x++) PluginTX_ptr[x] = 0;

  x = 0;

#ifdef PLUGIN_TX_000
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_001;
#endif

#ifdef PLUGIN_TX_001
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_001;
#endif

#ifdef PLUGIN_TX_002
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_002;
#endif

#ifdef PLUGIN_TX_003
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_003;
#endif

#ifdef PLUGIN_TX_004
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_004;
#endif

#ifdef PLUGIN_TX_005
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_005;
#endif

#ifdef PLUGIN_TX_006
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_006;
#endif

#ifdef PLUGIN_TX_007
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_007;
#endif

#ifdef PLUGIN_TX_008
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_008;
#endif

#ifdef PLUGIN_TX_009
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_009;
#endif

#ifdef PLUGIN_TX_010
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_010;
#endif

#ifdef PLUGIN_TX_011
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_011;
#endif

#ifdef PLUGIN_TX_012
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_012;
#endif

#ifdef PLUGIN_TX_013
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_013;
#endif

#ifdef PLUGIN_TX_014
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_014;
#endif

#ifdef PLUGIN_TX_015
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_015;
#endif

#ifdef PLUGIN_TX_016
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_016;
#endif

#ifdef PLUGIN_TX_017
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_017;
#endif

#ifdef PLUGIN_TX_018
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_018;
#endif

#ifdef PLUGIN_TX_019
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_019;
#endif

#ifdef PLUGIN_TX_020
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_020;
#endif

#ifdef PLUGIN_TX_021
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_021;
#endif

#ifdef PLUGIN_TX_022
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_022;
#endif

#ifdef PLUGIN_TX_023
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_023;
#endif

#ifdef PLUGIN_TX_024
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_024;
#endif

#ifdef PLUGIN_TX_025
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_025;
#endif

#ifdef PLUGIN_TX_026
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_026;
#endif

#ifdef PLUGIN_TX_027
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_027;
#endif

#ifdef PLUGIN_TX_028
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_028;
#endif

#ifdef PLUGIN_TX_029
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_029;
#endif

#ifdef PLUGIN_TX_030
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_030;
#endif

#ifdef PLUGIN_TX_031
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_031;
#endif

#ifdef PLUGIN_TX_032
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_032;
#endif

#ifdef PLUGIN_TX_033
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_033;
#endif

#ifdef PLUGIN_TX_034
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_034;
#endif

#ifdef PLUGIN_TX_035
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_035;
#endif

#ifdef PLUGIN_TX_036
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_036;
#endif

#ifdef PLUGIN_TX_037
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_037;
#endif

#ifdef PLUGIN_TX_038
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_038;
#endif

#ifdef PLUGIN_TX_039
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_039;
#endif

#ifdef PLUGIN_TX_040
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_040;
#endif

#ifdef PLUGIN_TX_041
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_041;
#endif

#ifdef PLUGIN_TX_042
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_042;
#endif

#ifdef PLUGIN_TX_043
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_043;
#endif

#ifdef PLUGIN_TX_044
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_044;
#endif

#ifdef PLUGIN_TX_045
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_045;
#endif

#ifdef PLUGIN_TX_046
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_046;
#endif

#ifdef PLUGIN_TX_047
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_047;
#endif

#ifdef PLUGIN_TX_048
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_048;
#endif

#ifdef PLUGIN_TX_049
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_049;
#endif

#ifdef PLUGIN_TX_050
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_050;
#endif

#ifdef PLUGIN_TX_051
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_051;
#endif

#ifdef PLUGIN_TX_052
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_052;
#endif

#ifdef PLUGIN_TX_053
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_053;
#endif

#ifdef PLUGIN_TX_054
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_054;
#endif

#ifdef PLUGIN_TX_055
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_055;
#endif

#ifdef PLUGIN_TX_056
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_056;
#endif

#ifdef PLUGIN_TX_057
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_057;
#endif

#ifdef PLUGIN_TX_058
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_058;
#endif

#ifdef PLUGIN_TX_059
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_059;
#endif

#ifdef PLUGIN_TX_065
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_065;
#endif

#ifdef PLUGIN_TX_066
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_066;
#endif

#ifdef PLUGIN_TX_067
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_067;
#endif

#ifdef PLUGIN_TX_068
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_068;
#endif

#ifdef PLUGIN_TX_069
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_069;
#endif

#ifdef PLUGIN_TX_076
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_076;
#endif

#ifdef PLUGIN_TX_077
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_077;
#endif

#ifdef PLUGIN_TX_078
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_078;
#endif

#ifdef PLUGIN_TX_079
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_079;
#endif

#ifdef PLUGIN_TX_080
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_080;
#endif

#ifdef PLUGIN_TX_081
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_081;
#endif

#ifdef PLUGIN_TX_082
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_082;
#endif

#ifdef PLUGIN_TX_254
  PluginTX_State[x] = P_Enabled;
  PluginTX_ptr[x++] = &PluginTX_254;
#endif

  // Initialiseer alle plugins door aanroep met verwerkingsparameter PLUGINTX_INIT
  PluginTXInitCall(0, 0);
}
/*********************************************************************************************\
 * This function initializes the Receive plugin function table
 \*********************************************************************************************/
byte PluginInitCall(byte Function, char *str)
{
  int x;

  for (x = 0; x < PLUGIN_MAX; x++)
  {
    if (Plugin_State[x] >= P_Enabled)
    {
      Plugin_ptr[x](Function, str);
    }
  }
  return true;
}
/*********************************************************************************************\
 * This function initializes the Transmit plugin function table
 \*********************************************************************************************/
byte PluginTXInitCall(byte Function, char *str)
{
  int x;

  for (x = 0; x < PLUGIN_TX_MAX; x++)
  {
    if (PluginTX_State[x] >= P_Enabled)
    {
      PluginTX_ptr[x](Function, str);
    }
  }
  return true;
}
/*********************************************************************************************\
 * With this function plugins are called that have Receive functionality.
 \*********************************************************************************************/
byte PluginRXCall(byte Function, const char *str)
{
  for (byte x = 0; x < PLUGIN_MAX; x++)
  {
    if (Plugin_State[x] >= P_Enabled)
    {
      SignalHash = x; // store plugin number
      if (Plugin_ptr[x](Function, str))
      {
        SignalHashPrevious = SignalHash; // store previous plugin number after success
        return true;
      }
    }
  }
  return false;
}
/*********************************************************************************************\
 * With this function plugins are called that have Transmit functionality.
 \*********************************************************************************************/
byte PluginTXCall(byte Function, const char *str)
{
  int x;

  for (x = 0; x < PLUGIN_TX_MAX; x++)
  {
    if (PluginTX_State[x] >= P_Enabled)
    {
      return PluginTX_ptr[x](Function, str);
    }
  }
  return false;
}
/*********************************************************************************************/