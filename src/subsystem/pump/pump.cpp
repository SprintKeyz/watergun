#include "subsystem/pump/pump.hpp"

#include <Arduino.h>

Pump::Pump(uint8_t pin, PumpParams p) {
    _pin = pin;
    _params = p;
}

void Pump::init() {
    ledcAttach(_pin, _params.pwmFreq, 10);
}

void Pump::update(float vBatt) {
    _vBatt = vBatt;

    if (!_isRunning) return;

    int targetDuty = calcDuty();
    uint32_t now = millis();
    float deltaTime = (now - _lastUpdateTime);
    _lastUpdateTime = now;

    // Handle 0ms soft start (instant on)
    if (_params.softStart <= 0) {
        _currentDuty = targetDuty;
    } 
    // Ramping logic
    else if (_currentDuty < targetDuty) {
        float increment = (targetDuty / _params.softStart) * deltaTime;
        _currentDuty += increment;
        if (_currentDuty > targetDuty) _currentDuty = targetDuty;
    } 
    // If battery voltage drops, targetDuty might become lower than current
    else {
        _currentDuty = targetDuty;
    }

    ledcWrite(_pin, (int)_currentDuty);
}

int Pump::calcDuty() {
    if (_vBatt <= 12) return 1023;

    float ratio = 12 / _vBatt;
    int duty = ratio * 1023;

    if (duty > _maxDuty) duty = _maxDuty;
    if (duty < _minDuty) duty = _minDuty;

    return duty;
}

void Pump::start() {
    _isRunning = true;
    _lastUpdateTime = millis();
}

void Pump::stop() {
    _isRunning = false;
    _currentDuty = 0; // Reset for next start
    ledcWrite(_pin, 0);
}