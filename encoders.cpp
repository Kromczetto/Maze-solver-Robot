#include <Arduino.h>

volatile long ticksLeft = 0;
volatile long ticksRight = 0;

void isrLeft() {
    ticksLeft++;
}

void isrRight() {
    ticksRight++;
}

void initEncoders() {
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(2), isrLeft, RISING);
    attachInterrupt(digitalPinToInterrupt(3), isrRight, RISING);
}

void resetEncoders() {
    ticksLeft = 0;
    ticksRight = 0;
}

long getLeftTicks() { return ticksLeft; }
long getRightTicks() { return ticksRight; }