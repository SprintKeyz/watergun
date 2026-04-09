#pragma once

#include <freertos/FreeRTOS.h>
#include <string>

#include "packet.hpp"

// UUIDs
#define SERVICE_UUID     "25df45d5-1beb-4223-a13a-7eb995bcf2a5"
#define NOTIFY_CHAR_UUID "1dee4855-33ba-4142-9735-04005383e1e7"
#define WRITE_CHAR_UUID  "4ea1e3e6-7446-493d-9ea7-8c89b6a1cfd2"

// forward declare
class BLEServer;
class BLECharacteristic;
class BLEService;
class BLEAdvertising;

class TelemetryManager {
public:
    TelemetryManager();
    ~TelemetryManager();

    void init(std::string name);
    void start(uint8_t coreID);
    void updateSensors(float batteryV,
                       int batteryP,
                       float waterCM,
                       int waterP,
                       int shots,
                       float currentPSI,
                       int targetPSI);
    void handleConnect();
    void handleDisconnect();
    void setPSITarget(int target);
    TelemetryPacket getData();

private:
    void run();
    void sendPacket();
    bool packetChanged(TelemetryPacket a, TelemetryPacket b);

    // allow freeRTOS to call our task and pin a core
    static void taskWrapper_(void *pvParameters);

    TelemetryPacket _packet;
    TelemetryPacket _prevPacket;
    SemaphoreHandle_t _mutex;
    TaskHandle_t _taskHandle;

    std::string _devname;
    BLEServer *_pServer;
    BLEService *_pService;

    // advertising
    BLEAdvertising *_pAdvertising;

    // BLE characteristics
    BLECharacteristic *_pNotifyChar;
    BLECharacteristic *_pWriteChar;

    bool _deviceConnected = false;
};