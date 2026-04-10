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
BuzzerTone waterLow = {100, 100, 3, 2000};
BuzzerTone waterFull = {200, 0, 1, 0};
BuzzerTone batt = {2000, 300, 3, 0};

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

    // for trigger
    pinMode(3, INPUT_PULLUP);
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
}

void loop() {
    battery->update();
    waterLevel->update();
    waterPressure->update();
    //buzzer->update(battery->getPct(), waterLevel->getPct());

    bool triggered = digitalRead(3);
    Serial.println(triggered);

    /*if (!triggered) {
        digitalWrite(2, HIGH);
    }

    else {
        digitalWrite(2, LOW);
    }*/

    Serial.printf("Water: %.2f (%.1f lvl, %.0f pct)\n", waterPressure->getPSI(), waterLevel->getLevel(), waterLevel->getPct());

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
