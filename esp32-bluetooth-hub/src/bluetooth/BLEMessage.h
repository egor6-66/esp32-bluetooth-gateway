// src/bluetooth/BLEMessage.h
#pragma once
#include <ArduinoJson.h>
#include <String.h>

struct BLEMessage {
    String type;
    JsonObject content;
    String rawPayload;

    BLEMessage() : rawPayload("") {}

    template<typename T>
    T get(const char* key, const T& defaultValue = T{}) const {
        return content[key] | defaultValue;
    }

    bool has(const char* key) const {
        return content.containsKey(key); // 👈 Работает в ArduinoJson v7+
    }
};