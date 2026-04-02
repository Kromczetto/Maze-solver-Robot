#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "tof_sensors.h"

#define XSHUT_LEFT  10
#define XSHUT_FRONT 9
#define XSHUT_RIGHT 8

VL53L0X sensorLeft;
VL53L0X sensorFront;
VL53L0X sensorRight;

float filterDistance(uint16_t d) {

    if (d == 0 || d == 65535 || d > 2000) return 100;
    return d / 10.0;
}

void initSensors() {

    Wire.begin();

    pinMode(XSHUT_LEFT, OUTPUT);
    pinMode(XSHUT_FRONT, OUTPUT);
    pinMode(XSHUT_RIGHT, OUTPUT);

    digitalWrite(XSHUT_LEFT, LOW);
    digitalWrite(XSHUT_FRONT, LOW);
    digitalWrite(XSHUT_RIGHT, LOW);
    delay(100);

    digitalWrite(XSHUT_LEFT, HIGH);
    delay(100);

    sensorLeft.init();
    sensorLeft.setAddress(0x30);
    sensorLeft.startContinuous();
    sensorLeft.setTimeout(50);

    digitalWrite(XSHUT_FRONT, HIGH);
    delay(100);

    sensorFront.init();
    sensorFront.setAddress(0x31);
    sensorFront.startContinuous();
    sensorFront.setTimeout(50);

    digitalWrite(XSHUT_RIGHT, HIGH);
    delay(100);

    sensorRight.init();
    sensorRight.setAddress(0x33);
    sensorRight.startContinuous();
    sensorRight.setTimeout(50);

    sensorLeft.setMeasurementTimingBudget(50000);
    sensorFront.setMeasurementTimingBudget(50000);
    sensorRight.setMeasurementTimingBudget(50000);
}

float readTOF(VL53L0X &sensor) {

    uint16_t d = sensor.readRangeContinuousMillimeters();

    if (sensor.timeoutOccurred()) return 100;

    return filterDistance(d);
}

float getFrontDistance() {
    return readTOF(sensorFront);
}

float getLeftDistance() {
    return readTOF(sensorLeft);
}

float getRightDistance() {
    return readTOF(sensorRight);
}