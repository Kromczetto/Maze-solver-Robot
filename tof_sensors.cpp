#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "tof_sensors.h"

#define XSHUT_RIGHT 8
#define XSHUT_FRONT 9
#define XSHUT_LEFT  10

VL53L0X sensorRight;
VL53L0X sensorFront;
VL53L0X sensorLeft;

float median3(float a, float b, float c) {
    if (a > b) { float t = a; a = b; b = t; }
    if (b > c) { float t = b; b = c; c = t; }
    if (a > b) { float t = a; a = b; b = t; }
    return b;
}

float filterDistance(uint16_t d) {
    if (d == 0 || d == 65535 || d > 2000) {
        return 200; 
    }
    return d / 10.0;
}

float readSensor(VL53L0X &sensor) {

    float d1 = filterDistance(sensor.readRangeContinuousMillimeters());
    float d2 = filterDistance(sensor.readRangeContinuousMillimeters());
    float d3 = filterDistance(sensor.readRangeContinuousMillimeters());

    return median3(d1, d2, d3);
}

void initSensors() {

    Wire.begin();

    pinMode(XSHUT_RIGHT, OUTPUT);
    pinMode(XSHUT_FRONT, OUTPUT);
    pinMode(XSHUT_LEFT, OUTPUT);

    digitalWrite(XSHUT_RIGHT, LOW);
    digitalWrite(XSHUT_FRONT, LOW);
    digitalWrite(XSHUT_LEFT, LOW);
    delay(100);

    digitalWrite(XSHUT_RIGHT, HIGH);
    delay(100);

    sensorRight.init();
    sensorRight.setAddress(0x30);
    sensorRight.startContinuous();

    digitalWrite(XSHUT_FRONT, HIGH);
    delay(100);

    sensorFront.init();
    sensorFront.setAddress(0x31);
    sensorFront.startContinuous();

    digitalWrite(XSHUT_LEFT, HIGH);
    delay(100);

    sensorLeft.init();
    sensorLeft.setAddress(0x32);
    sensorLeft.startContinuous();

    Serial.println("ToF INIT DONE");
}

float getFrontDistance() { return readSensor(sensorFront); }
float getLeftDistance()  { return readSensor(sensorLeft); }
float getRightDistance() { return readSensor(sensorRight); }