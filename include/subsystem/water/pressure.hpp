#pragma once

#include <cstdint>

class EMAFilter;

class WaterPressureManager {
public:
    WaterPressureManager(uint8_t pin, float c, float maxPSI);
    ~WaterPressureManager();

    float getPSI();
    float getMaxPSI();
    void setMaxPSI(float PSI);

    void update();

private:
    // atributes
    float _psi;
    float _maxPSI;

    // hardware
    uint8_t _pin;
    float _dividerConst;

    // EMA
    EMAFilter* _ema;
};