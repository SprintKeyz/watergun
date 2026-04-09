#pragma once

class EMAFilter {
public:
    EMAFilter(float alpha);

    void reset();
    float update(float raw);
    void setWeight(float alpha);

private:
    float _old;
    float _new;
    float _val;
    
    bool _firstReading = true;
};