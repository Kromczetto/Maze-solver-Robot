#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "tof_sensors.h"

#define XSHUT_LEFT  12
#define XSHUT_FRONT 13
#define XSHUT_RIGHT A0

#define FILTER_SIZE 3
#define FRONT_OFFSET_CM 3.0

VL53L0X sensorLeft;
VL53L0X sensorFront;
VL53L0X sensorRight;

float leftFiltered = 200;
float rightFiltered = 200;
float frontFiltered = 200;

float lastLeft = 200;
float lastRight = 200;
float lastFront = 200;

float leftBuffer[FILTER_SIZE] = {200,200,200};
float rightBuffer[FILTER_SIZE] = {200,200,200};
float frontBuffer[FILTER_SIZE] = {200,200,200};

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

float readTOF(VL53L0X &sensor, float &last) {

    uint16_t d = sensor.readRangeContinuousMillimeters();
    float val;

    if (sensor.timeoutOccurred() || d == 0 || d == 65535) {
        val = last; 
    } else {
        val = d / 10.0;
        last = val;
    }

    return val;
}

void initSensors() {

    Wire.begin();
    Wire.setTimeout(5); 

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
    sensorLeft.setTimeout(100);
    sensorLeft.setMeasurementTimingBudget(50000);
    sensorLeft.startContinuous();

    digitalWrite(XSHUT_FRONT, HIGH);
    delay(100);
    sensorFront.init();
    sensorFront.setAddress(0x31);
    sensorFront.setTimeout(100);
    sensorFront.setMeasurementTimingBudget(50000);
    sensorFront.startContinuous();

    digitalWrite(XSHUT_RIGHT, HIGH);
    delay(100);
    sensorRight.init();
    sensorRight.setAddress(0x33);
    sensorRight.setTimeout(100);
    sensorRight.setMeasurementTimingBudget(50000);
    sensorRight.startContinuous();
}

void updateTOF() {

    float leftRaw = readTOF(sensorLeft, lastLeft);
    float rightRaw = readTOF(sensorRight, lastRight);
    float frontRaw = readTOF(sensorFront, lastFront);

    float leftMed = medianFilter(leftRaw, leftBuffer);
    float rightMed = medianFilter(rightRaw, rightBuffer);
    float frontMed = medianFilter(frontRaw, frontBuffer);

    leftFiltered  = 0.5 * leftFiltered  + 0.5 * leftMed;
    rightFiltered = 0.5 * rightFiltered + 0.5 * rightMed;
    frontFiltered = 0.5 * frontFiltered + 0.5 * frontMed;
}

float getLeftFiltered() {
    return leftFiltered;
}

float getRightFiltered() {
    return rightFiltered - 1.0;
}

float getFrontFiltered() {
    float v = frontFiltered - FRONT_OFFSET_CM;
    if (v < 0) v = 0;
    return v;
}

float getLeftDistance() { return lastLeft; }
float getRightDistance() { return lastRight; }
float getFrontDistance() { return lastFront; }