#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "tof_sensors.h"
#include "left_hand.h"
#include "mpu.h"

SoftwareSerial SUART(12, 13);

bool robotEnabled = true;

unsigned long lastTelemetry = 0;
const int TELEMETRY_INTERVAL = 200;

const char* getStateString() {
    switch (getRobotState()) {
        case IDLE: return "IDLE";
        case FORWARD: return "FORWARD";
        case TURNING_LEFT: return "LEFT";
        case TURNING_RIGHT: return "RIGHT";
        //case TURNING_AROUND: return "AROUND";
    }
    return "UNKNOWN";
}

void sendTelemetry() {

    SUART.listen();  

    int front = (int)getFrontDistance();
    int left  = (int)getLeftDistance();
    int right = (int)getRightDistance();
    int angle = getAngle();

    const char* state = getStateString();

    SUART.print(front);
    SUART.print(",");
    SUART.print(left);
    SUART.print(",");
    SUART.print(right);
    SUART.print(",");
    SUART.print(angle);
    SUART.print(",");
    SUART.println(state);
}

void setup() {

    SUART.begin(9600);

    initMotors();
    initSensors();
    initMPU(); 

}

void loop() {

    if (SUART.available()) {
        char c = SUART.read();

        if (c == 'S') {
            robotEnabled = false;
            stopMotors();
        }

        if (c == 'R') {
            robotEnabled = true;
        }
    }

    if (robotEnabled) {

        updateGyro();
        updateMotion();

    }

    if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {

        SUART.listen(); 

        sendTelemetry();
        lastTelemetry = millis();
    }
}