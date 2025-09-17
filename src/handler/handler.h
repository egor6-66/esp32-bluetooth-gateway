#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

extern BLECharacteristic* pCharacteristic;
extern bool deviceConnected;

void initBLE();
void handleIncomingBLEMessage();
void handleModuleResponse(const String& json);