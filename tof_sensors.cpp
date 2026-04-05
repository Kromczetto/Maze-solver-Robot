#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "tof_sensors.h"

#define XSHUT_LEFT  12
#define XSHUT_FRONT 13
#define XSHUT_RIGHT A0

#define FILTER_SIZE 5

VL53L0X sensorLeft;
VL53L0X sensorFront;
VL53L0X sensorRight;

float leftBuffer[FILTER_SIZE]  = {200,200,200,200,200};
float rightBuffer[FILTER_SIZE] = {200,200,200,200,200};
float frontBuffer[FILTER_SIZE] = {200,200,200,200,200};

float medianFilter(float newValue, float *buffer) {

    for (int i = FILTER_SIZE - 1; i > 0; i--) {
        buffer[i] = buffer[i - 1];
    }
    buffer[0] = newValue;

    float temp[FILTER_SIZE];
    for (int i = 0; i < FILTER_SIZE; i++) {
        temp[i] = buffer[i];
    }

    for (int i = 0; i < FILTER_SIZE - 1; i++) {
        for (int j = 0; j < FILTER_SIZE - i - 1; j++) {
            if (temp[j] > temp[j + 1]) {
                float t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    return temp[FILTER_SIZE / 2];
}

float filterDistance(uint16_t d) {

    if (d == 0 || d == 65535 || d > 2000) return 200;

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

    // RIGHT
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

    if (sensor.timeoutOccurred()) return 200;

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

float getLeftFiltered() {
    float raw = getLeftDistance();
    return medianFilter(raw, leftBuffer);
}

float getRightFiltered() {
    float raw = getRightDistance();
    return medianFilter(raw, rightBuffer);
}

float getFrontFiltered() {
    float raw = getFrontDistance();
    return medianFilter(raw, frontBuffer);
}