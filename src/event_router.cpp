// event_router.cpp

#include "event_router.h"
#include "ble_handler.h"
#include <ArduinoJson.h>
#include "config.h"

void initEventBus() {
    BUS_SERIAL.begin(BUS_BAUD_RATE);
    DEBUG_PRINTLN("🚌 Event bus initialized (Serial1 @ 9600)");
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
    // 📦 Формируем сообщение для шины
    StaticJsonDocument<JSON_BUFFER_SIZE> busMessage;
    busMessage["source"] = "BLE_GATEWAY";
    busMessage["moduleName"] = moduleName;
    busMessage["eventName"] = eventName;
    busMessage["payload"] = payload;

    // 🧷 Добавляем requestId для возможного ответа
    busMessage["requestId"] = requestId;

    String serialized;
    serializeJson(busMessage, serialized);

    // 🚌 Отправляем на шину (в данном случае — Serial1)
    BUS_SERIAL.println(serialized);
    DEBUG_PRINT("🚌 Routed to bus: ");
    DEBUG_PRINTLN(serialized);

    // ✅ Отправляем подтверждение по BLE
    sendResponse(requestId, "success", "Event routed to bus");
}