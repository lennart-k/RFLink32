// ************************************* //
// * Arduino Project RFLink32        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //
#pragma once

#include <Arduino.h>
#include "RFLink.h"

#include <WiFi.h>
#include "10_Wifi.h"
#include "4_Display.h"
#include "11_Config.h"

#include <time.h>
#include <sys/time.h>

extern char MQTTbuffer[PRINT_BUFFER_SIZE]; // Buffer for MQTT message


namespace RFLink { namespace Mqtt {

    extern const char *mqtt_ca_cert_filename;

    extern Config::ConfigItem configItems[];
    extern struct timeval lastMqttConnectionAttemptTime;

    namespace params {
        extern bool enabled;
        extern String server;
        extern int port;
        extern String id;
        extern String user;
        extern String password;

        extern String topic_out;
        extern String topic_in;

        extern bool lwt_enabled;
        extern String topic_lwt;

        extern bool ssl_enabled;
        extern bool ssl_insecure;
        extern String ca_cert;
    }

void setup_MQTT();
void reconnect(int retryCount=-1, bool force=false);
void publishMsg();
void checkMQTTloop();

void paramsUpdatedCallback();
void refreshParametersFromConfig(bool triggerChanges=true);

void getStatusJsonString(JsonObject &output);

void triggerParamsHaveChanged();

}} // end of MQTT namespace
