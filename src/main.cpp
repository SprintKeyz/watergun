#include "pins.h"
#include "subsystem/battery/battery.hpp"
#include "subsystem/buzzer/buzzer.hpp"
#include "subsystem/water/level.hpp"
#include "subsystem/water/pressure.hpp"
#include "telemetry/manager.h"

#include <Arduino.h>

// BLE stack
TelemetryManager* telemetry = new TelemetryManager();

// battery
BatteryManager* battery = new BatteryManager(BATTERY_PIN, BATTERY_DC);

// tank level
WaterLevelManager* waterLevel = new WaterLevelManager(
    TRIG_PIN, ECHO_PIN, WATER_LEVEL_EMPTY, WATER_LEVEL_FULL, MAX_SHOTS);

// water pressure
WaterPressureManager* waterPressure =
    new WaterPressureManager(PRESSURE_PIN, LEVEL_5V_DC, 150.0f);

// buzzer
BuzzerTone waterLow = {200, 200, 3, 2000};
BuzzerTone waterFull = {200, 0, 1, 0};
BuzzerTone batt = {1000, 1000, 3, 0};

BuzzerManager* buzzer = new BuzzerManager(BUZZER_PIN, waterLow, waterFull, batt);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    telemetry->init("WaterGun");
    telemetry->start(0);

    analogReadResolution(10);
    analogSetAttenuation(ADC_11db);

    waterLevel->init();
    buzzer->init();
}

void loop() {
    battery->update();
    waterLevel->update();
    waterPressure->update();
    buzzer->update(battery->getPct(), waterLevel->getPct());

    telemetry->updateSensors(battery->getVoltage(),           // Battery Volts
                             battery->getPct(),               // Battery %
                             waterLevel->getLevel(),          // Water Level CM
                             waterLevel->getPct(),            // Water Level %
                             waterLevel->getShotsRemaining(), // Shots
                             waterPressure->getPSI(),  // Our oscillating PSI
                             telemetry->getPSITarget() // Target PSI
    );

    delay(50);
}
