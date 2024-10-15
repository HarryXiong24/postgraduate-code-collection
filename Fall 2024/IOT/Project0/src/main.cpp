#include <Arduino.h>
#include <Servo.h>

Servo myServo;             

const int PhotoPin = 15;
const int LedPin = 13;
const int ServoPin = 32;

int lightVal;
int lightMax = 0;
int lightMin = INT_MAX;
int serveMotor;

void setup() {
  pinMode(PhotoPin, OUTPUT);
  myServo.attach(ServoPin);

  time_t startTime;

  startTime = time(&startTime);
  while (time(nullptr) - startTime < 10) {
    digitalWrite(LedPin, time(nullptr) % 2 == 0);

    lightVal = analogRead(PhotoPin);
    lightMax = max(lightVal, lightMax);
    lightMin = min(lightVal, lightMin);
  }
  digitalWrite(LedPin, LOW);

  Serial.println("Max: " + lightMax);
  Serial.println("Min: " + lightMin);
}

void loop() {
  lightVal = analogRead(PhotoPin);
  serveMotor = map(lightVal, lightMin, lightMax, 0, 179);
  myServo.write(serveMotor);
}