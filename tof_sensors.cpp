#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "tof_sensors.h"

#define XSHUT_FRONT 9
VL53L0X sensorFront;

#define TRIG_LEFT  A0
#define ECHO_LEFT  A1

#define TRIG_RIGHT 8
#define ECHO_RIGHT 7

float filterDistance(uint16_t d) {

    if (d == 0 || d == 65535 || d > 2000) return 100;
    return d / 10.0;

}

float readFrontTOF() {

    uint16_t d = sensorFront.readRangeContinuousMillimeters();
    if (sensorFront.timeoutOccurred()) return 100;
    return filterDistance(d);

}

float readUltrasonic(int trig, int echo) {

    digitalWrite(trig, LOW);
    delayMicroseconds(2);

    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    long duration = pulseIn(echo, HIGH, 20000);

    if (duration == 0) return 200;

    float d = duration * 0.034 / 2.0;

    if (d < 5) d = 5;
    if (d > 200) d = 200;

    return d;
}

float filterUltrasonic(float raw, float prev) {

    if (abs(raw - prev) > 20) {
        return prev;
    }

    return 0.85 * prev + 0.15 * raw;
}


void initSensors() {

    Wire.begin();

    pinMode(XSHUT_FRONT, OUTPUT);
    digitalWrite(XSHUT_FRONT, LOW);
    delay(100);

    digitalWrite(XSHUT_FRONT, HIGH);
    delay(100);

    sensorFront.init();
    sensorFront.setAddress(0x30);
    sensorFront.startContinuous();
    sensorFront.setTimeout(50);

    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);

    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);

}

float getFrontDistance() {

    return readFrontTOF();

}

float getLeftDistance() {

    static float filtered = 10;

    float raw = readUltrasonic(TRIG_LEFT, ECHO_LEFT);
    delay(5); 

    filtered = filterUltrasonic(raw, filtered);

    return filtered;

}

float getRightDistance() {

    static float filtered = 10;

    float raw = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);
    delay(5);

    filtered = filterUltrasonic(raw, filtered);

    return filtered;
    
}