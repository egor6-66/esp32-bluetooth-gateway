// src/bluetooth/BLEApi.h
#pragma once
#include "BLEMessage.h"
#include <ArduinoJson.h>

// Инициализация BLE
void bleBegin(const char* deviceName = nullptr);

// Отправить JSON-сообщение
bool bleSendJson(const JsonDocument& doc);

// Зарегистрировать обработчик входящих сообщений
void bleOnReceive(void (*callback)(const BLEMessage&));

// Проверить подключение
bool bleIsConnected();

// Обязательно вызывать в loop()
void bleLoop();

// Получить имя устройства
const char* bleGetName();