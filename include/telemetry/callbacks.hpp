#pragma once

#include <BLEServer.h>

// forward declare
class TelemetryManager;

class ServerCallbacks : public BLEServerCallbacks {
public:
    ServerCallbacks(TelemetryManager* manager) : _pManager(manager) {}

    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;

private:
    TelemetryManager* _pManager;
};

// the watch will write a target PSI!
class WriteCallbacks : public BLECharacteristicCallbacks {
public:
    WriteCallbacks(TelemetryManager* manager) : _pManager(manager) {}

    void onWrite(BLECharacteristic* pChar) override;

private:
    TelemetryManager* _pManager;
};