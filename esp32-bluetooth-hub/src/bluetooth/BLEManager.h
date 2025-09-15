// src/bluetooth/BLEManager.h
#pragma once
#include "BLEMessage.h"
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>
#include "BLEConfig.h"

class AbstractBLE {
public:
    virtual void begin(const char* deviceName = nullptr) = 0;
    virtual bool sendJson(const JsonDocument& doc) = 0;
    virtual void onReceive(void (*callback)(const BLEMessage&)) = 0;
    virtual bool isConnected() const = 0;
    virtual void loop() = 0;
    virtual const char* getName() const = 0;
    virtual ~AbstractBLE() = default;
};