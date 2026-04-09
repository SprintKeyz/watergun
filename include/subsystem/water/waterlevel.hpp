#pragma once

#include <cstdint>

class EMAFilter;

class WaterLevelManager {
public:
    WaterLevelManager(uint8_t trig, uint8_t echo, float empty, float full);
    ~WaterLevelManager();

    // getters
    float getLevel();
    float getPct();
    float getEmpty();
    float getFull();

    // setters
    void setEmpty(float cm);
    void setFull(float cm);

    // misc
    void init();
    void update();

private:
    // attributes
    float _lvl;
    float _pct;
    float _empty;
    float _full;

    // hardware
    uint8_t _trig;
    uint8_t _echo;

    // EMA
    EMAFilter* _ema;

    // functions
    float lvlToPct(float lvl);
};