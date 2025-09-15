// main.cpp

#include <Arduino.h>
#include "config.h"
#include "ble_handler.h"
#include "event_router.h"

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    DEBUG_PRINTLN("🚀 Starting ESP32 BLE Gateway...");

    initEventBus();   // Инициализация шины
    initBLE();        // Инициализация BLE сервера
}

void loop() {
    handleIncomingBLEMessage();  // Проверка и обработка входящих BLE сообщений
    delay(LOOP_DELAY_MS);
}