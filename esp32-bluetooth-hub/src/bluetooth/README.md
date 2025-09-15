
# 📱 ESP32 + BLE JSON Communication (PlatformIO)

Простой и надёжный шаблон для обмена **JSON-сообщениями между ESP32 и веб-приложением/мобильным устройством** через **Bluetooth Low Energy (BLE GATT)**.

---

## ✅ Возможности

- ✅ Отправка и приём **JSON-сообщений**
- ✅ Поддержка **веб-браузера (Chrome/Edge)** через Web Bluetooth API
- ✅ Чистая архитектура: **отдельный модуль `BLEManager`**
- ✅ Расширяемая структура: легко добавлять новые команды
- ✅ Совместим с PlatformIO и CLion

---

## 🔧 Как использовать

### 1. Загрузите прошивку

Убедитесь, что в `platformio.ini` указано:
```ini
lib_deps =
    bblanchon/ArduinoJson@^7.0.0