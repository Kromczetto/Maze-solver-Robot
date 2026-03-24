#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "tof_sensors.h"
#include "left_hand.h"

SoftwareSerial SUART(12, 13);

bool robotEnabled = true;

unsigned long lastTelemetry = 0;
const int TELEMETRY_INTERVAL = 200;

const char* getStateString() {
    switch (getRobotState()) {
        case IDLE: return "IDLE";
        case MOVING_FORWARD: return "FORWARD";
        case TURNING_LEFT: return "LEFT";
        case TURNING_RIGHT: return "RIGHT";
        case TURNING_AROUND: return "AROUND";
    }
    return "UNKNOWN";
}

void readBLECommands() {
    while (SUART.available()) {
        char c = SUART.read();

        if (c == 'S') {
            robotEnabled = false;
            stopMotors();
        }

        if (c == 'R') {
            robotEnabled = true;
        }
    }
}

void sendTelemetry() {
    Serial.print((int)getFrontDistance()); Serial.print(",");
    Serial.print((int)getLeftDistance());  Serial.print(",");
    Serial.print((int)getRightDistance()); Serial.print(",");
    Serial.println(getStateString());
}

void setup() {
    Serial.begin(9600);
    SUART.begin(9600);

    initMotors();
    initSensors();

    Serial.println("START");
}

void loop() {

    readBLECommands();

    if (robotEnabled) {

        leftHandStep();
        updateMotion();

        if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {
            sendTelemetry();
            lastTelemetry = millis();
        }
    }
}