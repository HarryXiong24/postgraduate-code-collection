#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <SparkFunLSM6DSO.h>
#include <Wire.h>
#include <SPI.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c3319144"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26aa"


LSM6DSO myIMU;
int stepCount = 0;
float threshold = 1.5; // Adjust this value based on calibration
bool stepDetected = false;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        pCharacteristic->setValue(String(stepCount).c_str());
    }
};

BLECharacteristic *pCharacteristic;

void calibrateThreshold() {
    Serial.println("Starting calibration...");
    float sum = 0;
    int sampleCount = 100; // 采样数量
    for (int i = 0; i < sampleCount; i++) {
        float accelX = myIMU.readFloatAccelX();
        float accelY = myIMU.readFloatAccelY();
        float accelValue = sqrt(accelX * accelX + accelY * accelY);
        sum += accelValue;
        delay(20);
    }
    float average = sum / sampleCount;
    threshold = average * 1.5;
    Serial.print("Calibration complete. New threshold: ");
    Serial.println(threshold);
}

void setup() {
    Serial.begin(9600);
    
    // Initialize the LSM6DSO sensor
    Wire.begin();
    delay(10);
    if( myIMU.begin() )
      Serial.println("Ready.");
    else { 
      Serial.println("Could not connect to IMU.");
      Serial.println("Freezing");
    }

    calibrateThreshold();

    Serial.println(threshold);

    // Initialize BLE
    BLEDevice::init("IOT Group 66");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void loop() {
    float accelX = myIMU.readFloatAccelX();
    float accelY = myIMU.readFloatAccelY();

    float accelValue = sqrt(accelX * accelX + accelY * accelY);

    if (accelValue > threshold) {
        if (!stepDetected) {
            stepCount++;
            Serial.print("Step Count: ");
            Serial.println(stepCount);
            pCharacteristic->setValue(String(stepCount).c_str());
            pCharacteristic->notify();
            stepDetected = true;
        }
    } else {
        stepDetected = false;
    }

    delay(20);
}

