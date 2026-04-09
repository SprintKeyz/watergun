#pragma once

struct TelemetryPacket {
    float batteryVoltage;
    int batteryPct;
    float waterLevel;
    int waterLevelPct;
    int shotsRemaining;
    float currentPSI;
    float targetPSI;
};