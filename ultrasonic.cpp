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

static float readDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);

    float distance = duration * 0.0343 / 2.0;

    return distance;
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