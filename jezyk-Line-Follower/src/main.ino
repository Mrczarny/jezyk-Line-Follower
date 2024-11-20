#include <Arduino.h>
#include <motors.h>

#define A1_PIN 9
#define A2_PIN 6
#define B1_PIN 5
#define B2_PIN 3

//test

Motors motors(A2_PIN,A1_PIN, B1_PIN, B2_PIN);

void setup() {
  // put your setup code here, to run once:
  motors.setup(
    240,
    255,
    237,
    251
  );
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  motors.forward();
  delay(4000);
  motors.stop();
  delay(1000);

  motors.backward();
  delay(4000);
  motors.stop();
  delay(1000);

  motors.zeroLeft();
  delay(2000);
  motors.stop();
  delay(1000);

  motors.zeroRight();
  delay(2000);
  motors.stop();
  delay(1000);

    motors.left();
  delay(2000);
  motors.stop();
  delay(1000);

  motors.right();
  delay(2000);
  motors.stop();
  delay(1000);
}


