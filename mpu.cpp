#include <Wire.h>
#include <Arduino.h>
#include "mpu.h"

#define MPU_ADDR 0x68

float angleZ = 0;
float gyroOffset = 0;
unsigned long lastTime = 0;

int16_t rawGyroZ;
float lastAngle = 0;

void writeMPU(byte reg, byte data) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

void initMPU() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission();

    delay(100);

    for (int i = 0; i < 200; i++) {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x47);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU_ADDR, 2);

        rawGyroZ = Wire.read() << 8 | Wire.read();
        gyroOffset += rawGyroZ;
        delay(5);
    }

    gyroOffset /= 200.0;

    lastTime = millis();
}

void updateGyro() {

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;
    lastTime = now;

    if (dt <= 0 || dt > 0.1) return;

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 2);

    rawGyroZ = Wire.read() << 8 | Wire.read();

    float gyroZ = (rawGyroZ - gyroOffset) / 131.0; // deg/s

    float newAngle = angleZ + gyroZ * dt;

    if (abs(newAngle - lastAngle) < 50) {
        angleZ = newAngle;
        lastAngle = angleZ;
    }
}

void resetAngle() {
    angleZ = 0;
    lastAngle = 0;
}

float getAngle() {
    return angleZ;
}