// ble_handler.h

#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "config.h"

extern BLECharacteristic* pCharacteristic;
extern bool deviceConnected;

void initBLE();
void handleIncomingBLEMessage();