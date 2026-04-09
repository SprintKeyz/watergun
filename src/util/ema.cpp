#include "util/ema.hpp"

EMAFilter::EMAFilter(float alpha) {
    _new = alpha;
    _old = 1.0f - alpha;
}

void EMAFilter::setWeight(float alpha) {
    _new = alpha;
    _old = 1.0f - alpha;
}

void EMAFilter::reset() {
    _val = 0.0f;
    _firstReading = true;
}

float EMAFilter::update(float raw) {
    if (_firstReading) {
        _val = raw;
        _firstReading = false;
        return _val;
    }

    _val = (_val * _old) + (raw * _new);
    return _val;
}