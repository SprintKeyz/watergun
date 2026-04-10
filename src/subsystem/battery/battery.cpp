#include "subsystem/battery/battery.hpp"
#include "util/ema.hpp"

#include <Arduino.h>

BatteryManager::BatteryManager(uint8_t pin, float c) {
    _pin = pin;
    _dividerConst = c;

    _ema = new EMAFilter(0.1);
}

float BatteryManager::getVoltage(bool ema) {
    return ema ? _vcc : _vccNoEMA;
}

float BatteryManager::getCutoffV() {
    return _cutoff;
}

float BatteryManager::getMaxV() {
    return _full;
}

float BatteryManager::getPct() {
    return _pct;
}

void BatteryManager::update() {
    float adcV = analogReadMilliVolts(_pin) / 1000.0f;
    float rawV = adcV / _dividerConst;
    _vccNoEMA = rawV;

    float emaV = _ema->update(rawV);

    // now, we can update our voltage and pct
    _vcc = emaV;
    _pct = vccToPct(emaV);
}

float BatteryManager::vccToPct(float vTot) {
    // get cell voltage
    float v = vTot / 4.0f;
    if (v >= _full / 4.0f) return 100;
    if (v <= _cutoff / 4.0f) return 0;

    // approximate pct
    return 123 - (123 / pow(1 + pow(v / 3.7, 80), 0.165));
}