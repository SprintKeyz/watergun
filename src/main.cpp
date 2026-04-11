#include "pins.h"
#include "subsystem/battery/battery.hpp"
#include "subsystem/buzzer/buzzer.hpp"
#include "subsystem/pump/pump.hpp"
#include "subsystem/valve/valve.hpp"
#include "subsystem/water/level.hpp"
#include "subsystem/water/pressure.hpp"
#include "telemetry/manager.h"

#include <Arduino.h>

// --- Objects ---
TelemetryManager* telemetry = new TelemetryManager();
BatteryManager* battery = new BatteryManager(BATTERY_PIN, BATTERY_DC);
WaterLevelManager* waterLevel = new WaterLevelManager(TRIG_PIN, ECHO_PIN, WATER_LEVEL_EMPTY, WATER_LEVEL_FULL, MAX_SHOTS);
WaterPressureManager* waterPressure = new WaterPressureManager(PRESSURE_PIN, LEVEL_5V_DC, 150.0f);
BuzzerManager* buzzer = new BuzzerManager(BUZZER_PIN, {100, 100, 3, 2000}, {200, 0, 1, 0}, {2000, 300, 3, 0});
Valve* valve = new Valve(1);
Pump* pump = new Pump(2, {100, 10000}); // 100ms soft start

// --- Timing/State ---
unsigned long valveOpenStartTime = 0;
bool isFiring = false;
bool isRecharging = false;
const float cutInOffset = -20.0f; 

// --- Core 1 Task: Dedicated to Pump & Audio Stability ---
void pumpTask(void* pvParameters) {
    for (;;) {
        // We update the pump here so the PWM ramp stays smooth 
        // regardless of what the main loop is doing.
        pump->update(battery->getVoltage(false));
        buzzer->update(battery->getPct(), waterLevel->getPct());
        
        vTaskDelay(pdMS_TO_TICKS(10)); // 100Hz 
    }
}

// --- Core 0 Task: Dedicated to Water Level (The "Slow" Sensor) ---
void waterLevelTask(void* pvParameters) {
    for (;;) {
        waterLevel->update(); // This is the only slow part
        vTaskDelay(pdMS_TO_TICKS(500)); // Update level 10 times a second
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize all hardware
    telemetry->init("WaterGun");
    telemetry->start(0); // This usually starts its own task on Core 0

    analogReadResolution(10);
    analogSetAttenuation(ADC_11db);

    waterLevel->init();
    buzzer->init();
    pump->init();
    valve->init();
    pinMode(3, INPUT_PULLUP);

    // Create the tasks
    xTaskCreatePinnedToCore(waterLevelTask, "LevelTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(pumpTask, "PumpTask", 2048, NULL, 2, NULL, 1);
}

void loop() {
    // 1. Update Fast Sensors (Analog reads take microseconds)
    battery->update();
    waterPressure->update();

    float currentPSI = waterPressure->getPSI();
    float targetPSI = telemetry->getPSITarget();
    bool hasWater = waterLevel->getPct() > 0;
    bool triggerPressed = !digitalRead(3);

    // 2. Trigger Logic (Non-blocking)
    if (triggerPressed && !isFiring && hasWater && (currentPSI >= (targetPSI + cutInOffset))) {
        valve->open();
        isFiring = true;
        valveOpenStartTime = millis();
    }

    if (isFiring) {
        if (millis() - valveOpenStartTime >= 1000 || !hasWater) {
            valve->close();
            isFiring = false;
        }
    }

    // 3. Pump Hysteresis State
    if (!hasWater) {
        isRecharging = false;
    } else if (currentPSI <= (targetPSI + cutInOffset)) {
        isRecharging = true;
    } else if (currentPSI >= targetPSI) {
        isRecharging = false;
    }

    if (isRecharging) pump->start();
    else pump->stop();

    // 4. Telemetry Update
    telemetry->updateSensors(
        battery->getVoltage(), battery->getPct(),
        waterLevel->getLevel(), waterLevel->getPct(),
        waterLevel->getShotsRemaining(), currentPSI, targetPSI
    );

    // Loop can now run as fast as possible
    delay(6); 
}