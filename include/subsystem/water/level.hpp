#pragma once

#include <cstdint>

class EMAFilter;

class WaterLevelManager {
public:
    WaterLevelManager(uint8_t trig, uint8_t echo, float empty, float full, int shots);
    ~WaterLevelManager();

    // getters
    float getLevel();
    float getPct();
    float getEmpty();
    float getFull();
    int getShotsRemaining();
    int getShotsTotal();

    // setters
    void setEmpty(float cm);
    void setFull(float cm);
    void setShots(int shots);

    // misc
    void init();
    void update();

private:
    // attributes
    float _lvl;
    float _pct;
    float _empty;
    float _full;
    int _shotsTotal;
    int _shotsRemain;

    // hardware
    uint8_t _trig;
    uint8_t _echo;

    // EMA
    EMAFilter* _ema;

    // functions
    float lvlToPct(float lvl);
    int pctToShots(float pct);
};