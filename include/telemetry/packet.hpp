#pragma once

struct __attribute__((packed)) TelemetryPacket {
    float batteryVoltage;
    int batteryPct;
    float waterLevel;
    int waterLevelPct;
    int shotsRemaining;
    float currentPSI;
    int targetPSI;
};