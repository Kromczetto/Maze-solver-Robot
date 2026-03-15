#include <Arduino.h>
#include <SoftwareSerial.h>

#include "motors.h"
#include "motion.h"
#include "ultrasonic.h"
#include "left_hand.h"

SoftwareSerial SUART(12, 13);

bool robotEnabled = true;

unsigned long lastTelemetry = 0;
const int TELEMETRY_INTERVAL = 200;

const char* getStateString() {

    switch (getRobotState()) {

        case IDLE:
            return "IDLE";

        case MOVING_FORWARD:
            return "FORWARD";

        case MOVING_TO_CENTER:
            return "CENTER";     

        case TURNING_LEFT:
            return "LEFT";

        case TURNING_RIGHT:
            return "RIGHT";

        case TURNING_AROUND:
            return "AROUND";
    }

    return "UNKNOWN";
}

void readBLECommands() {

    while (SUART.available()) {

        char c = SUART.read();

        if (c == 'S') {     // STOP

            robotEnabled = false;
            stopMotors();

            Serial.println("Robot STOP");
        }

        if (c == 'R') {     // RESUME

            robotEnabled = true;

            Serial.println("Robot START");
        }
    }
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
    readBLECommands();

    if (robotEnabled) {
        updateMotion();

        if (isRobotIdle()) {

            leftHandStep();
        }        
        
      if (millis() - lastTelemetry > TELEMETRY_INTERVAL) {

          sendTelemetry();
          lastTelemetry = millis();
      }
    }

}