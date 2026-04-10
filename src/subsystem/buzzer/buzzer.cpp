#include "subsystem/buzzer/buzzer.hpp"

#include <Arduino.h>

BuzzerManager::BuzzerManager(uint8_t pin,
                             BuzzerTone waterLow,
                             BuzzerTone waterFull,
                             BuzzerTone batteryLow) {
    _pin = pin;
    _waterLow = waterLow;
    _batteryLow = batteryLow;
}

void BuzzerManager::init() { pinMode(_pin, OUTPUT); }

void BuzzerManager::update(float batteryPct, float waterLvlPct) {
    BuzzerTone* nextTone = nullptr;

    if (batteryPct == 0) {
        // takes priority
        nextTone = &_batteryLow;
    }

    else if (waterLvlPct == 0) {
        nextTone = &_waterLow;
    }

    else if (waterLvlPct == 100) {
        nextTone = &_waterFull;
    }

    if (nextTone != _currentTone) {
        _currentTone = nextTone;
        _currentRepeat = 0;
        _lastStateChange = millis();
        _isBeeping = false;
        digitalWrite(_pin, LOW); // Reset pin
        
        if (_currentTone != nullptr) {
             _isBeeping = true;
             digitalWrite(_pin, HIGH);
        }
    }

    if (_currentTone != nullptr) {
        handleBuzzerState();
    }
}

void BuzzerManager::handleBuzzerState() {
    uint32_t now = millis();
    uint32_t elapsed = now - _lastStateChange;

    if (_isBeeping) {
        // 1. End the beep pulse
        if (elapsed >= (uint32_t)_currentTone->duration) {
            digitalWrite(_pin, LOW);
            _isBeeping = false;
            _lastStateChange = now;
        }
    } 
    else if (_isCoolingDown) {
        // 2. Wait for the long rest between sets
        if (elapsed >= (uint32_t)_currentTone->setDelay) {
            _isCoolingDown = false;
            _currentRepeat = 0;
            _lastStateChange = now;
            // Trigger the first beep of the new set immediately
            digitalWrite(_pin, HIGH);
            _isBeeping = true;
        }
    }
    else {
        // 3. Short rest between individual beeps
        if (elapsed >= (uint32_t)_currentTone->rest) {
            if (_currentRepeat < _currentTone->repeats - 1) {
                // More beeps left in the current set
                digitalWrite(_pin, HIGH);
                _isBeeping = true;
                _currentRepeat++;
                _lastStateChange = now;
            } 
            else if (_currentTone->setDelay > 0) {
                // No beeps left, start the long cooling down period
                _isCoolingDown = true;
                _lastStateChange = now;
            } 
            else {
                // No beeps left and no setDelay: loop immediately
                _currentRepeat = 0;
                digitalWrite(_pin, HIGH);
                _isBeeping = true;
                _lastStateChange = now;
            }
        }
    }
}