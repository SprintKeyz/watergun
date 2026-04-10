#pragma once

#include <cstdint>

class EMAFilter;

// this uses a voltage divider to read battery voltage!
// power subsystem config + voltage rails
// - RAW_VCC -> raw from battery, goes to pump via PWM
// - 12V -> to pump
// - 5V -> to sensors
// - 3.3v -> regulated from ESP, low max I

class BatteryManager {
public:
    BatteryManager(uint8_t pin, float c);
    ~BatteryManager();

    // getters
    float getVoltage(bool ema = true);
    float getCutoffV();
    float getMaxV();
    float getPct();

    // setters
    void setCutoff(float v);
    void setMaxV(float v);

    // misc
    void update();

private:
    // attributes
    float _vcc;
    float _vccNoEMA;
    float _pct;
    float _cutoff = 13.5; // 1.5 LDO (12v rail)
    float _full = 16.8; // 4s1p

    // hardware
    float _dividerConst;
    uint8_t _pin;

    // EMA
    EMAFilter* _ema;

    // functions
    float vccToPct(float vTot);
};