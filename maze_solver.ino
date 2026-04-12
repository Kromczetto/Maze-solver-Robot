#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "tof_sensors.h"
#include "encoders.h"
#include "maze.h"

SoftwareSerial SUART(A1, A2);

bool robotEnabled = true;

unsigned long lastTelemetry = 0;
const int TELEMETRY_INTERVAL = 200;

const char* getStateString() {
    switch (getRobotState()) {
        case IDLE: return "IDLE";
        case FORWARD: return "FORWARD";
        case TURNING_LEFT: return "LEFT";
        case TURNING_RIGHT: return "RIGHT";
        case TURNING_AROUND: return "AROUND";
    }
    return "UNKNOWN";
}

void sendTelemetry() {

    SUART.listen();  

    int front = (int)getFrontDistance();
    int left  = (int)getLeftDistance();
    int right = (int)getRightDistance();
    int angle = 0;

    const char* state = getStateString();

    uint8_t walls = 0;

    if (hasWall(getRobotX(), getRobotY(), 0)) walls |= 1 << 0;
    if (hasWall(getRobotX(), getRobotY(), 1)) walls |= 1 << 1;
    if (hasWall(getRobotX(), getRobotY(), 2)) walls |= 1 << 2;
    if (hasWall(getRobotX(), getRobotY(), 3)) walls |= 1 << 3;

    SUART.print(front);
    SUART.print(",");
    SUART.print(left);
    SUART.print(",");
    SUART.print(right);
    SUART.print(",");
    SUART.print(angle);
    SUART.print(",");
    SUART.print(state);
    SUART.print(",");
    SUART.print(getRobotX());
    SUART.print(",");
    SUART.print(getRobotY());
    SUART.print(",");
    SUART.print((int)getRobotDir());
    SUART.print(",");
    SUART.println(walls); 
}

void setup() {

    SUART.begin(9600);

    initMotors();
    initSensors();
    initEncoders();
    initMaze();

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

        updateMotion();

    }

    if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {

        SUART.listen(); 

        sendTelemetry();
        lastTelemetry = millis();
    }
}