#include <Arduino.h>
#include "BLEManager.h"
#include "BLEApi.h"
#include "BLEConfig.h"
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>
#include <BLEAdvertising.h>

static void (*globalCallback)(const BLEMessage&) = nullptr;
static BLECharacteristic* pCharacteristic = nullptr;
static BLEServer* pServer = nullptr;
static bool deviceConnected = false;
static String deviceName = "";

class BLECallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("✅ BLE: Подключено");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("❌ BLE: Отключено");
    }
};

class BLEManager : public AbstractBLE {
public:
    void begin(const char* deviceNameOverride = nullptr) override {
        if (deviceNameOverride && strlen(deviceNameOverride) > 0) {
            deviceName = deviceNameOverride;
        } else {
            deviceName = BLE_DEVICE_NAME_DEFAULT;
        }

        Serial.printf("🔧 Инициализация BLE с именем: %s\n", deviceName.c_str());

        BLEDevice::setPower(BLE_TX_POWER);
        BLEDevice::init(deviceName.c_str());

        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new BLECallbacks());

        // Создаём стандартный сервис FFE0 + характеристика FFE1
        BLEService* service = pServer->createService(BLE_CUSTOM_SERVICE_UUID);
        pCharacteristic = service->createCharacteristic(
            BLE_CUSTOM_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
        );
        pCharacteristic->setValue("Ready");
        service->start();

        // Реклама
        BLEAdvertising* advertising = pServer->getAdvertising();
        advertising->addServiceUUID(BLE_CUSTOM_SERVICE_UUID);
        advertising->setMinInterval(BLE_ADVERTISING_INTERVAL_MIN);
        advertising->setMaxInterval(BLE_ADVERTISING_INTERVAL_MAX);
        advertising->setScanResponse(BLE_ADVERTISING_SCAN_RESPONSE);
        advertising->start();

        pinMode(BLE_LED_PIN, OUTPUT);
        digitalWrite(BLE_LED_PIN, HIGH);
        delay(100);
        digitalWrite(BLE_LED_PIN, LOW);

        Serial.println("📡 BLE запущен. Жду подключения...");
        Serial.println("📡 Advertising with name: " + deviceName);
    }

    bool sendJson(const JsonDocument& doc) override {
        if (!deviceConnected || !pCharacteristic) return false;

        String json;
        serializeJson(doc, json);

        if (json.length() > BLE_MAX_JSON_SIZE) {
            Serial.println("⚠️ JSON слишком большой (>512 байт)");
            return false;
        }

        pCharacteristic->setValue(json.c_str());
        pCharacteristic->notify();
        Serial.print("📤 Отправлено: ");
        Serial.println(json);
        return true;
    }

    void onReceive(void (*callback)(const BLEMessage&)) override {
        globalCallback = callback;
    }

    bool isConnected() const override {
        return deviceConnected;
    }

    void loop() override {
        if (!deviceConnected || !pCharacteristic) return;

        std::string valueStr = pCharacteristic->getValue();
        if (valueStr.empty()) return;

        String received(valueStr.c_str(), valueStr.length());
        pCharacteristic->setValue(""); // Очищаем после чтения

        if (received.length() == 0 || received.length() > BLE_MAX_JSON_SIZE) return;

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, received);

        if (error || !doc.is<JsonObject>()) return;

        BLEMessage msg;
        msg.rawPayload = received;
        msg.content = doc.as<JsonObject>();
        msg.type = doc["type"] | "";

        if (globalCallback) {
            globalCallback(msg);
        }
    }

    const char* getName() const override {
        return deviceName.c_str();
    }
};

static BLEManager bleInstance;

void bleBegin(const char* deviceName) { bleInstance.begin(deviceName); }
bool bleSendJson(const JsonDocument& doc) { return bleInstance.sendJson(doc); }
void bleOnReceive(void (*callback)(const BLEMessage&)) { bleInstance.onReceive(callback); }
bool bleIsConnected() { return bleInstance.isConnected(); }
void bleLoop() { bleInstance.loop(); }
const char* bleGetName() { return bleInstance.getName(); }