#pragma once
#include "RFLink.h"

#include "11_Config.h"

namespace RFLink {
    namespace Portal {

        extern Config::ConfigItem configItems[];
        /**
         * puts things together but doesn't start server *yet*
         * */
        void init();
        void start();
        void stop();

        void paramsUpdatedCallback();
        void refreshParametersFromConfig(bool triggerChanges=true);
    }
}
