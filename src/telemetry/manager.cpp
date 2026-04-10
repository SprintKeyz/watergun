#include "telemetry/manager.h"
#include "telemetry/callbacks.hpp"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

TelemetryManager::TelemetryManager() {
    // use a mutex to prevent concurrent r/w
    _mutex = xSemaphoreCreateMutex();
    _packet = {0};
    _prevPacket = {0};
}

void TelemetryManager::setPSITarget(int target) {
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        _psiTarget = target;
        xSemaphoreGive(_mutex);
    }
}

int TelemetryManager::getPSITarget() {
    return _psiTarget;
}

void TelemetryManager::updateSensors(float batteryV,
                                     int batteryP,
                                     float waterCM,
                                     int waterP,
                                     int shots,
                                     float currentPSI,
                                     int targetPSI) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
        _packet.batteryVoltage = batteryV;
        _packet.batteryPct = batteryP;
        _packet.waterLevel = waterCM;
        _packet.waterLevelPct = waterP;
        _packet.shotsRemaining = shots;
        _packet.currentPSI = currentPSI;
        _packet.targetPSI = targetPSI;
        xSemaphoreGive(_mutex);
    }
}

void TelemetryManager::init(std::string name) { _devname = name; }

void TelemetryManager::handleDisconnect() {
    _prevPacket = {0};
    _deviceConnected = false;
}

void TelemetryManager::handleConnect() {
    _prevPacket = {0};
    _deviceConnected = true;
}

void TelemetryManager::start(uint8_t coreID) {
    // we should use core 0 for BLE stack on esp32-s3
    xTaskCreatePinnedToCore(this->taskWrapper_,
                            "TelemetryTask",
                            4096,
                            this,
                            1,
                            &_taskHandle,
                            coreID);
}

void TelemetryManager::taskWrapper_(void* pvParameters) {
    // get oinstance from parameter
    TelemetryManager* instance = (TelemetryManager*)pvParameters;
    instance->run();
}

void TelemetryManager::run() {
    // init server and comms
    BLEDevice::init(_devname.c_str());

    _pServer = BLEDevice::createServer();
    _pServer->setCallbacks(new ServerCallbacks(this));
    _pService = _pServer->createService(SERVICE_UUID);

    // notifications & writing
    _pNotifyChar =
        _pService->createCharacteristic(NOTIFY_CHAR_UUID,
                                        BLECharacteristic::PROPERTY_NOTIFY);

    _pWriteChar =
        _pService->createCharacteristic(WRITE_CHAR_UUID,
                                        BLECharacteristic::PROPERTY_WRITE);

    _pNotifyChar->addDescriptor(new BLE2902());
    _pWriteChar->setCallbacks(new WriteCallbacks(this));

    _pService->start();

    _pAdvertising = BLEDevice::getAdvertising();
    _pAdvertising->addServiceUUID(SERVICE_UUID);
    _pAdvertising->setScanResponse(true);
    _pAdvertising->setMinPreferred(0x06);

    BLEDevice::startAdvertising();

    uint32_t lastSendTime = 0;

    // now, our main loop
    while (true) {
        // we want a heartbeat!
        bool timeout = (millis() - lastSendTime > 5000);

        // send updated packets
        if (_deviceConnected &&
            (packetChanged(_prevPacket, _packet) || timeout)) {
            sendPacket();
            lastSendTime = millis();
        } else {
            // slow down loop
            vTaskDelay(pdMS_TO_TICKS(500));
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void TelemetryManager::sendPacket() {
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        _pNotifyChar->setValue((uint8_t*)&_packet, sizeof(_packet));
        _pNotifyChar->notify();

        _prevPacket = _packet;

        xSemaphoreGive(_mutex);
    }
}

bool TelemetryManager::packetChanged(TelemetryPacket a, TelemetryPacket b) {
    // check if b is sufficiently different from a
    if (fabs(b.batteryVoltage - a.batteryVoltage) > 0.2) {
        return true;
    }

    if (abs(b.batteryPct - a.batteryPct) > 0) {
        return true;
    }

    if (fabs(b.currentPSI - a.currentPSI) > 1) {
        return true;
    }

    if (fabs(b.waterLevel - a.waterLevel) > 1) {
        return true;
    }

    if (fabs(b.waterLevelPct - a.waterLevelPct) > 1) {
        return true;
    }

    return false;
}