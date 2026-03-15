#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "ultrasonic.h"
#include "left_hand.h"

SoftwareSerial SUART(12, 13);

unsigned long lastTelemetry = 0;
const int TELEMETRY_INTERVAL = 200;

const char* getStateString() {

    if (isRobotIdle()) return "IDLE";

    return "MOVING";
}

void sendTelemetry() {

    int front = (int)getFrontDistance();
    int left  = (int)getLeftDistance();
    int right = (int)getRightDistance();

    const char* state = getStateString();

    SUART.print(front);
    SUART.print(",");
    SUART.print(left);
    SUART.print(",");
    SUART.print(right);
    SUART.print(",");
    SUART.println(state);

    Serial.print(front);
    Serial.print(",");
    Serial.print(left);
    Serial.print(",");
    Serial.print(right);
    Serial.print(",");
    Serial.println(state);
}

void setup() {

    Serial.begin(9600);
    SUART.begin(9600);

    initMotors();
    initSensors();

    Serial.println("Maze robot started");
}

void loop() {

    updateMotion();

    if (isRobotIdle()) {

        leftHandStep();
    }

    if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {

        sendTelemetry();
        lastTelemetry = millis();
    }
}