#include "telemetry/callbacks.hpp"
#include "telemetry/manager.h"

#define PSI_MIN 5
#define PSI_MAX 100

void ServerCallbacks::onConnect(BLEServer* pServer) {
    Serial.println("[BLE] Device connected!");
    _pManager->handleConnect();
}

void ServerCallbacks::onDisconnect(BLEServer* pServer) {
    Serial.println("[BLE] Device disconnected!");
    _pManager->handleDisconnect();
    pServer->startAdvertising(); // begin advertising again
}

void WriteCallbacks::onWrite(BLECharacteristic* pChar) {
    // get raw data as string
    String raw = pChar->getValue().c_str();
    raw.trim();

    // validate data
    int received = raw.toInt();
    if (received >= PSI_MIN && received <= PSI_MAX) {
        Serial.printf("[BLE] Received PSI %d\n", received);
        _pManager->setPSITarget(received);
    }
}
