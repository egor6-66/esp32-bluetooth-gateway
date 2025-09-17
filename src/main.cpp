#include <Arduino.h>
#include "./config.h"
#include "./eventBus/eventBus.h"
#include "./handler/handler.h"
#define IS_GATEWAY

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    DEBUG_PRINTLN("🚀 Starting ESP32 BLE Gateway...");

    EventBus::begin();
    initBLE();
}

void loop() {
    // Обработка входящих BLE сообщений
    handleIncomingBLEMessage();

    // Обработка ответов от модулей
    if (EventBus::available()) {
        String line = EventBus::readLine();
        if (line.length() > 0) {
            handleModuleResponse(line);
        }
    }

    delay(LOOP_DELAY_MS);
}