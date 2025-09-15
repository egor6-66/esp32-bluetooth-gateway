// event_router.h

#pragma once

#include <ArduinoJson.h>
#include "config.h"

void initEventBus();
void routeEventToBus(const char* requestId, const char* moduleName, const char* eventName, JsonObject& payload);
void sendResponse(const char* requestId, const char* status, const char* message, JsonDocument& additional = *(new StaticJsonDocument<64>()));