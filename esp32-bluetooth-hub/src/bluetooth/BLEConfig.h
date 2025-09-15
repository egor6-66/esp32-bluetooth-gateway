#pragma once

#define BLE_DEVICE_NAME_DEFAULT "MySensorHub_02"

// 🔹 UUID сервиса — короткий, стандартный, работает ВСЕГДА
#define BLE_CUSTOM_SERVICE_UUID           "0000ffe0-0000-1000-8000-00805f9b34fb"
#define BLE_CUSTOM_CHARACTERISTIC_UUID    "0000ffe1-0000-1000-8000-00805f9b34fb"

// 🔹 Стандартные параметры
#define BLE_ADVERTISING_INTERVAL_MIN      20
#define BLE_ADVERTISING_INTERVAL_MAX      20
#define BLE_ADVERTISING_SCAN_RESPONSE     true
#define BLE_TX_POWER                      ESP_PWR_LVL_P9
#define BLE_LED_PIN                       2

#define BLE_MAX_JSON_SIZE                 512

#define BLE_ADVERTISE_CUSTOM_SERVICE      true
#define BLE_ADVERTISE_DEVICE_INFO_SERVICE false // 👈 НЕ НУЖЕН — мы используем стандартный UUID