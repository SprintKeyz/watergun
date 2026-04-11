#pragma once

#include <cstdint>

class Valve {
public:
    Valve(uint8_t pin);
    ~Valve();

    void init();

    void open();
    void close();

private:
    uint8_t _pin;
};