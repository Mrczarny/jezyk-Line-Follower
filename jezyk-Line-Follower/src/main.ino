#include <Arduino.h>

#define A1_PIN 9
#define A2_PIN 6
#define B1_PIN 5
#define B2_PIN 3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A1_PIN, OUTPUT);
  pinMode(A2_PIN, OUTPUT);
  pinMode(B1_PIN, OUTPUT);
  pinMode(B2_PIN, OUTPUT);

}

void loop() {
  //turnRight();
  // stop();
  // delay(5000);
   turnLeft();
  // stop();
  // delay(5000);
}

void forward() {
  analogWrite(A1_PIN, 255);
  analogWrite(B2_PIN, 255);
  delay(1000);
}

void backward() {
  analogWrite(A2_PIN, 255);
  analogWrite(B1_PIN, 255);
  delay(1000);
}

void stop() {
  analogWrite(A1_PIN, 0);
  analogWrite(A2_PIN, 0);
  analogWrite(B1_PIN, 0);
  analogWrite(B2_PIN, 0);
  delay(1000);
}

void zeroTurnRight() {
  analogWrite(A2_PIN, 255);
  analogWrite(B2_PIN, 255);
  delay(1000);
}

void zeroTurnLeft() {
  analogWrite(A1_PIN, 255);
  analogWrite(B1_PIN, 255);
  delay(1000);
}

void turnRight() {
  analogWrite(A2_PIN, 255);
  analogWrite(B1_PIN, 180);
  delay(1000);
}

void turnLeft() {
  analogWrite(A2_PIN, 180);
  analogWrite(B1_PIN, 255);
  delay(1000);
}

