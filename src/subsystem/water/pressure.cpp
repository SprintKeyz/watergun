#include "subsystem/water/pressure.hpp"
#include "util/ema.hpp"

#include <Arduino.h>
#include <cmath>

WaterPressureManager::WaterPressureManager(uint8_t pin, float c, float maxPSI) {
    _pin = pin;
    _dividerConst = c;
    _maxPSI = maxPSI;

    _ema = new EMAFilter(0.25);
}

float WaterPressureManager::getPSI() {
    return _psi;
}

float WaterPressureManager::getMaxPSI() {
    return _maxPSI;
}

void WaterPressureManager::setMaxPSI(float PSI) {
    _maxPSI = PSI;
}

void WaterPressureManager::update() {
    float adcV = analogReadMilliVolts(_pin) / 1000.0f;
    float rawV = adcV / _dividerConst;
    float emaV = _ema->update(rawV);

    float psi = ((emaV - 0.5f) / (4.5f - 0.5f)) * _maxPSI;
    _psi = std::clamp(psi, 0.0f, _maxPSI);
}