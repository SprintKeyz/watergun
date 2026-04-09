#include "subsystem/water/waterlevel.hpp"
#include "util/ema.hpp"

#include <Arduino.h>

WaterLevelManager::WaterLevelManager(uint8_t trig, uint8_t echo, float empty, float full) {
    _trig = trig;
    _echo = echo;
    _empty = empty;
    _full = full;

    _ema = new EMAFilter(0.1);
}

float WaterLevelManager::getLevel() {
    return _lvl;
}

float WaterLevelManager::getPct() {
    return _pct;
}

float WaterLevelManager::getEmpty() {
    return _empty;
}

float WaterLevelManager::getFull() {
    return _full;
}

void WaterLevelManager::setEmpty(float cm) {
    _empty = cm;
}

void WaterLevelManager::setFull(float cm) {
    _full = cm;
}

void WaterLevelManager::init() {
    pinMode(_trig, OUTPUT);
    pinMode(_echo, INPUT);
}

void WaterLevelManager::update() {
    digitalWrite(_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trig, LOW);

    int duration = pulseIn(_echo, HIGH);
    float rawCM = duration / 58.0f;
    float emaCM = _ema->update(rawCM);

    _lvl = emaCM;
    _pct = lvlToPct(emaCM);
}

float WaterLevelManager::lvlToPct(float lvl) {
    return (_empty - lvl) / (_empty - _full);
}