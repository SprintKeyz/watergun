#include "pins.h"
#include "subsystem/battery/battery.hpp"
#include "telemetry/manager.h"

#include <Arduino.h>

TelemetryManager* telemetry = new TelemetryManager();
BatteryManager* battery = new BatteryManager(BATTERY_PIN, DIVIDER_CONST);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    telemetry->init("ESP-WATER-GUN");
    telemetry->start(0);

    analogReadResolution(10);
    analogSetAttenuation(ADC_11db);

    // TODO: make sure we set analogreadresolution and ADC_11db attenuation
    // somewhere else!
}

void loop() {
    battery->update();

    telemetry->updateSensors(battery->getVoltage(), // Battery Volts
                             battery->getPct(),     // Battery %
                             20.0,                  // Water Level CM
                             80,                    // Water Level %
                             20,                    // Shots
                             50,                    // Our oscillating PSI
                             100                    // Target PSI
    );

    delay(50);
}
