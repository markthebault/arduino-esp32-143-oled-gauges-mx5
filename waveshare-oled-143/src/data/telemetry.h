#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

/* --- Telemetry Structure --- */
typedef struct __attribute__((packed)) {
  float oilTemp;
  float waterTemp;
  uint32_t engineRPM;
  float oilPressure;
  float brakePressure;
  int brakePercent;
  float throttlePos;
  float speed;
  float accelPos;
} TelemetryData;

#endif // TELEMETRY_H
