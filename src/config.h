#pragma once

// ———— Общие настройки ————
#define SERIAL_BAUD_RATE          115200
#define DEBUG_ENABLED             true

// ———— DEBUG ————
#if DEBUG_ENABLED
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// ———— Настройки шины (UART) ————
#define BUS_SERIAL                Serial2
#define BUS_RX_PIN                16   // RX пин для Serial2
#define BUS_TX_PIN                17   // TX пин для Serial2
#define BUS_BAUD_RATE             115200

// ———— Настройки JSON ————
#define JSON_BUFFER_SIZE          512

// ———— GATEWAY ————
#ifdef IS_GATEWAY
  #define BLE_DEVICE_NAME           "ESP32_BLE_GATEWAY"
  #define BLE_SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
  #define BLE_CHARACTERISTIC_UUID   "beb54836-36ac-4d87-9f91-ba699084199d"
  #define STATUS_LED_PIN            LED_BUILTIN
  #define LOOP_DELAY_MS             100
#endif
