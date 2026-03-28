#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "tof_sensors.h"
// #include "left_hand.h"
#include "mpu.h"

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

// void readBLECommands() {
//     while (SUART.available()) {
//         char c = SUART.read();

//         if (c == 'S') {
//             robotEnabled = false;
//             stopMotors();
//         }

//         if (c == 'R') {
//             robotEnabled = true;
//         }
//     }
// }

void sendTelemetry() {

    int front = (int)getFrontDistance();
    int left  = (int)getLeftDistance();
    int right = (int)getRightDistance();
    int angle = (int)getAngle();

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
    Serial.begin(9600);
    SUART.begin(9600);

    initMotors();
    initSensors();
    initMPU(); 

    Serial.println("START");
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
            currentState = MOVING_FORWARD;
            resetAngle(); // 🔥 MUST HAVE
        }
    }

    if (robotEnabled) {
        updateGyro();
        updateMotion();
    }

    if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {
        sendTelemetry();
        lastTelemetry = millis();
    }
}