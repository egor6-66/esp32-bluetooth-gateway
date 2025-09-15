// ble_handler.cpp

#include "ble_handler.h"
#include "event_router.h"
#include <ArduinoJson.h>
#include "config.h"

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

    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, value);

    if (error) {
        DEBUG_PRINT("❌ JSON parse error: ");
        DEBUG_PRINTLN(error.c_str());
        pCharacteristic->setValue(""); // очищаем
        return;
    }

    const char* requestId = doc["id"];
    if (!requestId) {
        DEBUG_PRINTLN("⚠️ No 'id' field — ignoring message");
        pCharacteristic->setValue("");
        return;
    }

    const char* moduleName = doc["moduleName"] | "";
    const char* eventName = doc["eventName"] | "";
    JsonObject payload = doc["payload"];

    if (strlen(moduleName) == 0 || strlen(eventName) == 0) {
        DEBUG_PRINTLN("⚠️ Missing moduleName or eventName");
        pCharacteristic->setValue("");
        return;
    }

    // 🚀 Передаем событие на шину
    routeEventToBus(requestId, moduleName, eventName, payload);

    // 🧹 Очищаем характеристику
    pCharacteristic->setValue("");
}