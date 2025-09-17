#include <Arduino.h>
#include <ArduinoJson.h>
#include "./handler.h"
#include "../eventBus/eventBus.h"
#include "../config.h"

BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        DEBUG_PRINTLN("📱 Client connected");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        DEBUG_PRINTLN("📴 Client disconnected");
    }
};

void initBLE() {
    BLEDevice::init(BLE_DEVICE_NAME);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEAddress macAddress = BLEDevice::getAddress();
    DEBUG_PRINT("📡 ESP32 BLE MAC: ");
    DEBUG_PRINTLN(macAddress.toString().c_str());

    BLEService* pService = pServer->createService(BLE_SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        BLE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->start();

    DEBUG_PRINTLN("✅ BLE Advertising started. Waiting for connections...");
}

void handleIncomingBLEMessage() {
    if (!deviceConnected || !pCharacteristic) return;

    std::string rawValue = pCharacteristic->getValue();
    if (rawValue.length() == 0) return;

    String value(rawValue.c_str());
    DEBUG_PRINT("📥 BLE Received: ");
    DEBUG_PRINTLN(value);

    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    DeserializationError error = deserializeJson(doc, value);

    if (error) {
        DEBUG_PRINT("❌ JSON parse error: ");
        DEBUG_PRINTLN(error.c_str());
        pCharacteristic->setValue("");
        return;
    }

    const char* requestId = doc["id"];
    const char* targetModule = doc["targetModule"];  // ⬅️ Кому адресовано
    const char* eventName = doc["eventName"];
    JsonObject payload = doc["payload"];

    if (!requestId || !targetModule || !eventName) {
        DEBUG_PRINTLN("⚠️ Missing required fields: id, targetModule, or eventName");
        pCharacteristic->setValue("");
        return;
    }

    // 🚀 Отправляем событие на шину
    EventBus::sendEvent("BLE_GATEWAY", targetModule, eventName, payload, requestId);

    // 🧹 Очищаем характеристику
    pCharacteristic->setValue("");
}

void handleModuleResponse(const String& json) {
    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        DEBUG_PRINT("❌ Failed to parse module response: ");
        DEBUG_PRINTLN(error.c_str());
        return;
    }

    const char* type = doc["type"];
    if (strcmp(type, "response") != 0) return;

    const char* requestId = doc["requestId"];
    const char* status = doc["status"];
    const char* message = doc["message"];

    if (!requestId || !status || !message) {
        DEBUG_PRINTLN("⚠️ Incomplete response from module");
        return;
    }

    // Отправляем ответ обратно клиенту по BLE
    if (deviceConnected && pCharacteristic) {
        StaticJsonDocument<JSON_BUFFER_SIZE> response;
        response["id"] = requestId;
        response["status"] = status;
        response["message"] = message;

        JsonObject additional = doc["additional"];
        if (additional) {
            response["data"] = additional;
        }

        String jsonResponse;
        serializeJson(response, jsonResponse);

        pCharacteristic->setValue(jsonResponse.c_str());
        pCharacteristic->notify();

        DEBUG_PRINT("📲 Forwarded to BLE Client: ");
        DEBUG_PRINTLN(jsonResponse);
    }
}