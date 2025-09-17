#pragma once

#include <ArduinoJson.h>
#include "../config.h"

class EventBus {
public:
    static void begin();
    static bool available();
    static String readLine();
    static void sendEvent(const char* source, const char* targetModule, const char* eventName, JsonObject& payload, const char* requestId = nullptr);
    static void sendResponse(const char* requestId, const char* target, const char* status, const char* message, JsonDocument& additional = *(new JsonDocument()));
};