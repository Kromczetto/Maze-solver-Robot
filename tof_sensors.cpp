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

    if (!sensorRight.init()) {
        Serial.println("RIGHT FAIL");
    } else {
        sensorRight.setAddress(0x30);
        sensorRight.setTimeout(50);
        sensorRight.startContinuous();
        Serial.println("RIGHT OK");
    }

    digitalWrite(XSHUT_FRONT, HIGH);
    delay(100);

    if (!sensorFront.init()) {
        Serial.println("FRONT FAIL");
    } else {
        sensorFront.setAddress(0x31);
        sensorFront.setTimeout(50);
        sensorFront.startContinuous();
        Serial.println("FRONT OK");
    }

    digitalWrite(XSHUT_LEFT, HIGH);
    delay(100);

    if (!sensorLeft.init()) {
        Serial.println("LEFT FAIL");
    } else {
        sensorLeft.setAddress(0x32);
        sensorLeft.setTimeout(50);
        sensorLeft.startContinuous();
        Serial.println("LEFT OK");
    }

    Serial.println("ToF INIT DONE");
}

float readSensor(VL53L0X &sensor) {
    uint16_t d = sensor.readRangeContinuousMillimeters();

    if (sensor.timeoutOccurred() || d == 65535 || d == 0) {
        return 200; 
    }

    return d / 10.0; 
}

float getFrontDistance() {
    return readSensor(sensorFront);
}

float getLeftDistance() {
    return readSensor(sensorLeft);
}

float getRightDistance() {
    return readSensor(sensorRight);
}