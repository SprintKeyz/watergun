#include "pins.h"
#include "subsystem/battery/battery.hpp"
#include "subsystem/water/waterlevel.hpp"
#include "telemetry/manager.h"

#include <Arduino.h>

TelemetryManager* telemetry = new TelemetryManager();
BatteryManager* battery = new BatteryManager(BATTERY_PIN, DIVIDER_CONST);
WaterLevelManager* waterLevel = new WaterLevelManager(
    TRIG_PIN, ECHO_PIN, WATER_LEVEL_EMPTY, WATER_LEVEL_FULL);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    telemetry->init("ESP-WATER-GUN");
    telemetry->start(0);

    analogReadResolution(10);
    analogSetAttenuation(ADC_11db);

    waterLevel->init();
}

void loop() {
    battery->update();
    waterLevel->init();

    telemetry->updateSensors(battery->getVoltage(),  // Battery Volts
                             battery->getPct(),      // Battery %
                             waterLevel->getLevel(), // Water Level CM
                             waterLevel->getPct(),   // Water Level %
                             20,                     // Shots
                             50,                     // Our oscillating PSI
                             100                     // Target PSI
    );

    delay(50);
}
