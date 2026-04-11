#include "subsystem/water/level.hpp"
#include "util/ema.hpp"

#include <Arduino.h>

WaterLevelManager::WaterLevelManager(uint8_t trig, uint8_t echo, float empty, float full, int shots) {
    _trig = trig;
    _echo = echo;
    _empty = empty;
    _full = full;
    _shotsTotal = shots;

    _ema = new EMAFilter(0.15);
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

int WaterLevelManager::getShotsTotal() {
    return _shotsTotal;
}

int WaterLevelManager::getShotsRemaining() {
    return _shotsRemain;
}

void WaterLevelManager::setEmpty(float cm) {
    _empty = cm;
}

void WaterLevelManager::setFull(float cm) {
    _full = cm;
}

void WaterLevelManager::setShots(int shots) {
    _shotsTotal = shots;
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
    printf("EMA CM: %.2f\n", emaCM);
    _pct = lvlToPct(emaCM);
    _shotsRemain = pctToShots(_pct);
}

float WaterLevelManager::lvlToPct(float lvl) {
    printf("lvl: %.2f\n", lvl);
    float pct = (_empty - lvl) / (_empty - _full);
    printf("pct: %.2f (empty: %.2f, full: %.2f)\n", pct, _empty, _full);

    if (pct < 0.0) return 0.0;
    if (pct > 1.0) return 1.0;

    return pct;
}

int WaterLevelManager::pctToShots(float pct) {
    return static_cast<int>(std::round(pct * _shotsTotal));
}