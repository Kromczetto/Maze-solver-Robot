#include <Arduino.h>
#include "ultrasonic.h"

#define TRIG_FRONT 9
#define ECHO_FRONT 8

#define TRIG_LEFT 11
#define ECHO_LEFT 10

#define TRIG_RIGHT 7
#define ECHO_RIGHT 6

// 0.0 - brak filtracji
// 1.0 - bardzo wolna reakcja
#define EMA_ALPHA 0.5

static float frontFiltered = 0;
static float leftFiltered = 0;
static float rightFiltered = 0;

void initSensors() {
    pinMode(TRIG_FRONT, OUTPUT);
    pinMode(ECHO_FRONT, INPUT);

    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);
    
    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);
}

static float measureRaw(int trigPin, int echoPin) {

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
}

// EMA: new = alpha * prev + (1 - alpha) * measurement
static float applyEMA(float prev, float measurement) {
    return EMA_ALPHA * prev + (1.0 - EMA_ALPHA) * measurement;
}

float getFrontDistance() {
    float raw = measureRaw(TRIG_FRONT, ECHO_FRONT);

    if (frontFiltered == 0) {
        frontFiltered = raw; // inicjalizacja
    } else {
        frontFiltered = applyEMA(frontFiltered, raw);
    }

    return frontFiltered;
}

float getLeftDistance() {
    float raw = measureRaw(TRIG_LEFT, ECHO_LEFT);

    if (leftFiltered == 0) {
        leftFiltered = raw;
    } else {
        leftFiltered = applyEMA(leftFiltered, raw);
    }

    return leftFiltered;
}

float getRightDistance() {
    float raw = measureRaw(TRIG_RIGHT, ECHO_RIGHT);

    if (rightFiltered == 0) {
        rightFiltered = raw;
    } else {
        rightFiltered = applyEMA(rightFiltered, raw);
    }

    return rightFiltered;
}