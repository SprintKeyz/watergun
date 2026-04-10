#pragma once

#include <cstdint>

struct BuzzerTone {
    int duration;
    int rest;
    int repeats;
    int setDelay;
};

class BuzzerManager {
public:
    BuzzerManager(uint8_t pin, BuzzerTone waterLow, BuzzerTone waterFull, BuzzerTone batteryLow);
    ~BuzzerManager();

    void init();
    void update(float batteryPct, float waterLvlPct);

private:
    // hardware
    uint8_t _pin;
    BuzzerTone _waterLow;
    BuzzerTone _batteryLow;
    BuzzerTone _waterFull;

    uint32_t _lastStateChange = 0;
    uint8_t _currentRepeat = 0;
    bool _isBeeping = false;
    bool _isActive = false;
    bool _isCoolingDown = false;
    BuzzerTone* _currentTone = nullptr;

    void handleBuzzerState();
};