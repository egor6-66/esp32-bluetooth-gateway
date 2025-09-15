// config.h

#pragma once

// ———— BLE настройки ————
#define BLE_DEVICE_NAME           "ESP32_BLE_ReactNative"
#define BLE_SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_CHARACTERISTIC_UUID   "beb54836-36ac-4d87-9f91-ba699084199d"

// ———— JSON буфер ————
#define JSON_BUFFER_SIZE          512   // увеличил для payload

// ———— Пины ————
#define STATUS_LED_PIN            LED_BUILTIN  // можно заменить на 2, 25 и т.д.

// ———— Системные настройки ————
#define SERIAL_BAUD_RATE          115200
#define LOOP_DELAY_MS             100

// ———— Шина (Serial для примера) ————
#define BUS_SERIAL                Serial2   // или Serial, или SoftwareSerial
#define BUS_BAUD_RATE             115200

// ———— Отладка ————
#define DEBUG_ENABLED             true
#if DEBUG_ENABLED
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif