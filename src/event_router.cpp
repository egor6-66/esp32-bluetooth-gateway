// event_router.cpp

#include "event_router.h"
#include "ble_handler.h"
#include <ArduinoJson.h>
#include "config.h"

void initEventBus() {
    BUS_SERIAL.begin(BUS_BAUD_RATE, SERIAL_8N1, 16, 17); // ✅ 115200 будет применена
    DEBUG_PRINTLN("🚌 Event bus initialized (Serial2 @ 115200, GPIO16/17)");
}

void sendResponse(const char* requestId, const char* status, const char* message, JsonDocument& additional) {
    if (!pCharacteristic || !deviceConnected) return;

    StaticJsonDocument<JSON_BUFFER_SIZE> response;
    response["id"] = requestId;
    response["status"] = status;
    response["message"] = message;

    // Копируем дополнительные поля, если есть
    for (JsonPair kv : additional.as<JsonObject>()) {
        response[kv.key()] = kv.value();
    }

    String jsonResponse;
    serializeJson(response, jsonResponse);

    pCharacteristic->setValue(jsonResponse.c_str());
    pCharacteristic->notify();

    DEBUG_PRINT("📤 Sent BLE Response: ");
    DEBUG_PRINTLN(jsonResponse);
}

void routeEventToBus(const char* requestId, const char* moduleName, const char* eventName, JsonObject& payload) {
    StaticJsonDocument<JSON_BUFFER_SIZE> busMessage;
    busMessage["source"] = "BLE_GATEWAY";
    busMessage["moduleName"] = moduleName;
    busMessage["eventName"] = eventName;
    busMessage["payload"] = payload;
    busMessage["requestId"] = requestId;

    String serialized;
    serializeJson(busMessage, serialized);

    BUS_SERIAL.println(serialized); // ✅ Только это уходит на шину

    // Логируй в отладку, если нужно
    // DEBUG_PRINT("✅ Routed: "); DEBUG_PRINTLN(serialized);

    sendResponse(requestId, "success", "Event routed to bus");
}