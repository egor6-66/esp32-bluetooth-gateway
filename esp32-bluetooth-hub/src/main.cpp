#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <ArduinoJson.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb54836-36ac-4d87-9f91-ba699084199d"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Client connected");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("Client disconnected");
    }
};
void handleEvent(const char* requestId, const char* action, StaticJsonDocument<256>& payload) {
    Serial.print("🔧 Handling event: ");
    Serial.println(action);

    // 👇 Ответ по умолчанию (можно переопределить в каждом case)
    StaticJsonDocument<256> response;
    response["id"] = requestId;
    response["status"] = "error";
    response["message"] = "Unknown action";

    // 🔄 Обработка по типу события
    if (strcmp(action, "toggle_led") == 0) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        Serial.println("✅ LED toggled");

        response["status"] = "success";
        response["action"] = "toggle_led";
        response["message"] = "LED state changed";

    } else if (strcmp(action, "get_device_info") == 0) {
        // Пример: отправка MAC-адреса и статуса
        BLEAddress mac = BLEDevice::getAddress();
        response["status"] = "success";
        response["action"] = "get_device_info";
        response["message"] = "Device info retrieved";
        response["mac_address"] = mac.toString().c_str();
        response["connected"] = deviceConnected;

    } else if (strcmp(action, "ping") == 0) {
        response["status"] = "success";
        response["message"] = "pong";
    }

    // 📤 Отправляем ответ
    String jsonResponse;
    serializeJson(response, jsonResponse);
    pCharacteristic->setValue(jsonResponse.c_str());
    pCharacteristic->notify();

    Serial.print("📤 Sent response: ");
    Serial.println(jsonResponse);
}
void setup() {
    Serial.begin(115200);

    BLEDevice::init("ESP32_BLE_ReactNative");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // 👇 Получаем и выводим MAC-адрес ESP32
    BLEAddress macAddress = BLEDevice::getAddress();
    Serial.print("📱 ESP32 BLE MAC Address: ");
    Serial.println(macAddress.toString().c_str());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    Serial.println("Waiting for a client connection...");
}

void loop() {
    if (deviceConnected) {
        // Проверяем, есть ли новые данные
        if (pCharacteristic->getValue().length() > 0) {
            String value = pCharacteristic->getValue().c_str();
            Serial.print("📥 Received: ");
            Serial.println(value);

            // 👇 Парсим JSON
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, value);
            if (error) {
                Serial.print("❌ JSON parse error: ");
                Serial.println(error.c_str());
                pCharacteristic->setValue(""); // Очищаем, чтобы не зациклиться
                return;
            }

            // 👇 Обязательное поле "id"
            const char* requestId = doc["id"];
            if (!requestId) {
                Serial.println("⚠️ Ignoring message without 'id'");
                pCharacteristic->setValue(""); // Очищаем
                return;
            }

            // 👇 Получаем тип события (action) и payload (весь doc)
            const char* action = doc["action"] | ""; // По умолчанию пустая строка

            // 🎯 Вызываем обработчик события с payload
            handleEvent(requestId, action, doc);

            // 👇 ОЧИЩАЕМ значение характеристики — обработано один раз!
            pCharacteristic->setValue("");
        }
    }
    delay(100);
}