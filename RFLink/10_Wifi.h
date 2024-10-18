#pragma once
#include "RFLink.h"
#include "WiFi.h"

#include "11_Config.h"

namespace RFLink {
    namespace Wifi {

        extern Config::ConfigItem configItems[];

        void setup();
        void mainLoop();
        
        void stop_WIFI();
        void start_WIFI();

        void resetClientWifi(); // to connect/reconnect client wifi after settings have changed

        void clientParamsUpdatedCallback();
        void accessPointParamsUpdatedCallback();
        void reconnectServices();

        bool clientNetworkIsUp();
        bool ntpIsSynchronized();

        void getStatusJsonString(JsonObject &output);
    }

    namespace AutoOTA {
        void checkForUpdateAndApply();
    }
}