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

bool robotEnabled = false;
bool algorithmSelected = false;

unsigned long lastTelemetry = 0;
const int TELEMETRY_INTERVAL = 200;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

int cellsTraversed = 0;
int turnsCount = 0;

bool runFinished = false;

const char* getStateString() {
    switch (getRobotState()) {

        case IDLE: return "IDLE";
        case DECIDE: return "DECIDE";
        case WAIT_FOR_FLOOD: return "FLOOD_WAIT";
        case DEBUG_PAUSE: return "DEBUG";

        case FORWARD: return "FORWARD";
        case TURNING_LEFT: return "LEFT";
        case TURNING_RIGHT: return "RIGHT";
        case TURNING_AROUND: return "AROUND";
        case POST_TURN_FORWARD: return "POST_FORWARD";
    }

    return "UNKNOWN";
}

void sendTelemetry() {

    int front = (int)getFrontFiltered();
    int left = (int)getLeftFiltered();
    int right = (int)getRightFiltered();

    const char* state = getStateString();

    uint8_t walls = 0;

    if (hasWall(getRobotX(), getRobotY(), 0)) walls |= 1 << 0;
    if (hasWall(getRobotX(), getRobotY(), 1)) walls |= 1 << 1;
    if (hasWall(getRobotX(), getRobotY(), 2)) walls |= 1 << 2;
    if (hasWall(getRobotX(), getRobotY(), 3)) walls |= 1 << 3;

    uint8_t value;

    if (currentAlgorithm == TREMAUX) {
        value = getCellVisit(getRobotX(), getRobotY());
    } else {
        value = getCellValue(getRobotX(), getRobotY());
    }

    SUART.print(front); SUART.print(",");
    SUART.print(left); SUART.print(",");
    SUART.print(right); SUART.print(",");
    SUART.print(state); SUART.print(",");
    SUART.print(getRobotX()); SUART.print(",");
    SUART.print(getRobotY()); SUART.print(",");
    SUART.print((int)getRobotDir()); SUART.print(",");
    SUART.print(walls); SUART.print(",");
    SUART.print(value); SUART.print(",");
    SUART.print(elapsedTime); SUART.print(",");
    SUART.print(cellsTraversed); SUART.print(",");
    SUART.print(turnsCount); SUART.print(",");
    SUART.println((int)currentAlgorithm);
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
}

void loop() {

    if (SUART.available()) {

        String command = SUART.readStringUntil('\n');
        command.trim();

        if (command == "S") {
            robotEnabled = false;
            stopMotors();
        }

        else if (command == "R") {

            if (!algorithmSelected) {
                return;
            }

            resetRobotState();

            robotEnabled = true;
            runFinished = false;

            startTime = millis();
            cellsTraversed = 0;
            turnsCount = 0;

            startDecision();

            sendMazeDebugBT(SUART);
        }

        else if (command.startsWith("ALG:")) {

            int algo = command.substring(4).toInt();

            if (algo >= 0 && algo <= 3) {

                currentAlgorithm = (NavigationAlgorithm)algo;
                algorithmSelected = true;

                SUART.print("ALG_SET:");
                SUART.println(algo);
            }
        }
    }

    updateTOF();

    if (robotEnabled && algorithmSelected) {
        updateMotion();
    }

    if (!runFinished) {
        elapsedTime = millis() - startTime;
    }

    if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {

        SUART.listen();
        sendTelemetry();
        lastTelemetry = millis();
    }
}