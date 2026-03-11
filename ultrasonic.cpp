#include <Arduino.h>
#include "ultrasonic.h"

#define TRIG_FRONT 9
#define ECHO_FRONT 8

#define TRIG_LEFT 11
#define ECHO_LEFT 10

#define TRIG_RIGHT 7
#define ECHO_RIGHT 6

#define TRIG_BACK 13
#define ECHO_BACK 12

void initSensors() {
    pinMode(TRIG_FRONT, OUTPUT);
    pinMode(ECHO_FRONT, INPUT);

    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);
    
    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);
    
    pinMode(TRIG_BACK, OUTPUT);
    pinMode(ECHO_BACK, INPUT);
}

static float median3(float a, float b, float c) {

    if (a > b) { float t = a; a = b; b = t; }
    if (b > c) { float t = b; b = c; c = t; }
    if (a > b) { float t = a; a = b; b = t; }

    return b;
}

static float readDistance(int trigPin, int echoPin) {
    float d1, d2, d3;

    auto measure = [&](void) {

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);

        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        long duration = pulseIn(echoPin, HIGH, 20000);

        float distance = duration * 0.0343 / 2.0;

        if (distance == 0 || distance > 200) {
            distance = 200;
        }

        return distance;
    };

    d1 = measure();
    delay(2);

    d2 = measure();
    delay(2);

    d3 = measure();

    return median3(d1, d2, d3);
}

float getFrontDistance() {
    return readDistance(TRIG_FRONT, ECHO_FRONT);
}

float getLeftDistance() {
    return readDistance(TRIG_LEFT, ECHO_LEFT);
}

float getRightDistance() {
    return readDistance(TRIG_RIGHT, ECHO_RIGHT);
}

float getBackDistance() {
    return readDistance(TRIG_BACK, ECHO_BACK);
}