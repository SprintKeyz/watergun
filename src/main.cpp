#include "telemetry/manager.h"
#include <Arduino.h>

TelemetryManager* telemetry = new TelemetryManager();

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    telemetry->init("ESP-WATER-GUN");
    telemetry->start(0);
}

void loop() {
    unsigned long now = millis();

    float period = 14000.0;
    float angle = (2.0 * M_PI * now) / period;

    float sineValue = sin(angle);
    float generatedPSI = 52.5 + (47.5 * sineValue);
    
    telemetry->updateSensors(12.6,         // Battery Volts
                             95,           // Battery %
                             20.0,         // Water Level CM
                             80,           // Water Level %
                             50,           // Shots
                             generatedPSI, // Our oscillating PSI
                             100           // Target PSI
    );

    delay(50);
}
