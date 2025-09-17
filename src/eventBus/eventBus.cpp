#include "./eventBus.h"

void EventBus::begin() {
    BUS_SERIAL.begin(BUS_BAUD_RATE, SERIAL_8N1, BUS_RX_PIN, BUS_TX_PIN);
    DEBUG_PRINTLN("🚌 EventBus initialized on Serial2 (RX:" + String(BUS_RX_PIN) + ", TX:" + String(BUS_TX_PIN) + ")");
}

bool EventBus::available() {
    return BUS_SERIAL.available();
}

String EventBus::readLine() {
    return BUS_SERIAL.readStringUntil('\n');
}

void EventBus::sendEvent(const char* source, const char* targetModule, const char* eventName, JsonObject& payload, const char* requestId) {
    JsonDocument doc;
    doc["type"] = "event";
    doc["source"] = source;
    doc["target"] = targetModule;
    doc["eventName"] = eventName;
    doc["payload"] = payload;
    if (requestId) doc["requestId"] = requestId;

    String json;
    serializeJson(doc, json);
    BUS_SERIAL.println(json);
    DEBUG_PRINT("📤 EventBus Sent Event: ");
    DEBUG_PRINTLN(json);
}

void EventBus::sendResponse(const char* requestId, const char* target, const char* status, const char* message, JsonDocument& additional) {
    if (!requestId || strlen(requestId) == 0) return;

    JsonDocument doc;
    doc["type"] = "response";
    doc["requestId"] = requestId;
    doc["target"] = target;
    doc["status"] = status;
    doc["message"] = message;

    for (JsonPair kv : additional.as<JsonObject>()) {
        doc[kv.key()] = kv.value();
    }

    String json;
    serializeJson(doc, json);
    BUS_SERIAL.println(json);
    DEBUG_PRINT("📤 EventBus Sent Response: ");
    DEBUG_PRINTLN(json);
}