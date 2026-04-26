#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "tof_sensors.h"
#include "encoders.h"
#include "maze.h"
#include "drive.h"
#include "robot_config.h"
#include "navigator.h"

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

    int front = (int)getFrontFiltered();
    int left = (int)getLeftFiltered();
    int right = (int)getRightFiltered();
    int mazeSize = MAZE_SIZE;

    const char* state = getStateString();

    uint8_t walls = 0;

    if (hasWall(getRobotX(), getRobotY(), 0)) walls |= 1 << 0;
    if (hasWall(getRobotX(), getRobotY(), 1)) walls |= 1 << 1;
    if (hasWall(getRobotX(), getRobotY(), 2)) walls |= 1 << 2;
    if (hasWall(getRobotX(), getRobotY(), 3)) walls |= 1 << 3;

    ///uint8_t value = getCellValue(getRobotX(), getRobotY());

    uint8_t value;

    if (currentAlgorithm == TREMAUX) {
        value = getCellVisit(getRobotX(), getRobotY());
    } else {
        value = getCellValue(getRobotX(), getRobotY());
    }

    SUART.print(front);
    SUART.print(",");
    SUART.print(left);
    SUART.print(",");
    SUART.print(right);
    SUART.print(",");
    SUART.print(mazeSize);
    SUART.print(",");
    SUART.print(state);
    SUART.print(",");
    SUART.print(getRobotX());
    SUART.print(",");
    SUART.print(getRobotY());
    SUART.print(",");
    SUART.print((int)getRobotDir());
    SUART.print(",");
    SUART.print(walls);
    SUART.print(",");
    SUART.println(value); 
}

void setup() {

    SUART.begin(9600);

    initMotors();
    initSensors();
    initEncoders();
    initMaze();

    sendMazeDebugBT(SUART);

    stopMotors();
    delay(2000);

    for (int i = 0; i < 20; i++) {
        updateTOF();
        delay(20);
    }

    float left = getLeftFiltered();
    float right = getRightFiltered();
    float front = getFrontFiltered();

    updateWalls(left, front, right);

    startDecision();
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

    updateTOF();
    if (robotEnabled) {
        updateMotion();
    }

    if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {

        SUART.listen(); 

        sendTelemetry();
        lastTelemetry = millis();
    }
}