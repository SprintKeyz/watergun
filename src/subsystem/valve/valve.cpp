#include "subsystem/valve/valve.hpp"

#include <Arduino.h>

Valve::Valve(uint8_t pin) {
    _pin = pin;
}

void Valve::init() {
    pinMode(_pin, OUTPUT);
}

void Valve::open() {
    digitalWrite(_pin, HIGH);
}

void Valve::close() {
    digitalWrite(_pin, LOW);
}