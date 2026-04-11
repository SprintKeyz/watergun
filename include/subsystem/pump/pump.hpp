#pragma once

#include <cstdint>

struct PumpParams {
    float softStart;
    int pwmFreq;
};

class Pump {
public:
    Pump(uint8_t pin, PumpParams p);
    ~Pump();

    void init();
    void update(float vBatt);

    void start();
    void stop();

private:
    int calcDuty();

    PumpParams _params;

    uint8_t _pin;
    float _vBatt;

    float _currentDuty = 0; 
    bool _isRunning = false;
    uint32_t _lastUpdateTime = 0;

    float _minDuty = 900;
    float _maxDuty = 1023;
};