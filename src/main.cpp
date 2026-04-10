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

const int freq = 10000;
const int resolution = 10;
const float targetV = 12.0;

void waterLevelTask(void *pvParameters) {
    for (;;) {
        waterLevel->update();
        // Run this slower than the main loop to save resources
        // 100ms (10Hz) is plenty for water level
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}


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
    //pinMode(2, OUTPUT);

    ledcAttach(2, freq, resolution);

    xTaskCreatePinnedToCore(waterLevelTask, "WaterLevelTask", 4096, NULL, 1, NULL, 1);
}

int calculateSafeDuty(float vBatt, float vTarget) {
    if (vBatt <= vTarget) return 1023;

    float ratio = vTarget / vBatt;
    int duty = ratio * 1023;

    if (duty > 1023) duty = 1023;
    if (duty < 0) duty = 0;

    return duty;
}

void loop() {
    battery->update();
    //waterLevel->update();
    waterPressure->update();
    //buzzer->update(battery->getPct(), waterLevel->getPct());

    bool triggered = digitalRead(3);
    Serial.println(triggered);

    int targetDuty = calculateSafeDuty(battery->getVoltage(false), targetV);

    //ledcWrite(2, targetDuty);


    if (!triggered) {
        //digitalWrite(1, HIGH);
        ledcWrite(2, targetDuty);
    }

    else {
        ledcWrite(2, 0);
        //digitalWrite(1, LOW);
    }

    Serial.printf("Pressure: %.2f", waterPressure->getPSI());

    /*telemetry->updateSensors(battery->getVoltage(),           // Battery Volts
                             battery->getPct(),               // Battery %
                             waterLevel->getLevel(),          // Water Level CM
                             waterLevel->getPct(),            // Water Level %
                             waterLevel->getShotsRemaining(), // Shots
                             waterPressure->getPSI(),  // Our oscillating PSI
                             telemetry->getPSITarget() // Target PSI
    );*/

    delay(100);
}
