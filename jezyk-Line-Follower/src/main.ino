#include <Arduino.h>
#include <motors.h>
#include <line.h>
#include <sonar.h>
#include <rotation.h>
#include <string.h>

#define A1_PIN 6
#define A2_PIN 7
#define B1_PIN 4
#define B2_PIN 5

int linePins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
Motors motors(A1_PIN, A2_PIN, B2_PIN, B1_PIN);
Line line(8, linePins);
Sonar sonar(9, 10);
Rotation rotation(2, 3);

static int lastTime = 0;

static lineSensorData lineState;

void setup()
{
  // put your setup code here, to run once:
  motors.setup(
      0, 0, 255, 255);
  Serial.begin(9600);
  line.setup(700);
  lineState = line.readLine();
  rotation.setup(&motors, 0, 0);
  // Serial.println("setup");
}

void loop()
{
  // put your main code here, to run repeatedly:
  lineState = line.readLine();
  uint8_t decimal = convertToBinary(lineState);
    switch (decimal)
    {
    default:
      break;
    
    case 0b00011000:
      motors.forward(255);
      break;
    case 0b11110000:
    case 0b01110000:
    case 0b00110000:
    case 0b01100000:
    case 0b11000000:
      motors.zeroRight(255);
      break;
    case 0b00001100:
    case 0b00001110:
    case 0b00001111:
    case 0b00000110:
    case 0b00000011:
      motors.zeroLeft(255);
      break;
    case 0b11111111:
      rotation.moveForwardFor(5);
      if (convertToBinary(line.readLine()) == 0b11111111)
      {
        motors.stop();
        while(1);
      }
      
      break;
    }
  
}

int detectChange()
{
  // line
  lineSensorData newLineState = line.readLine();
  for (int i = 0; i < 8; i++)
  {
    if (newLineState.linePoints[i].isLine != lineState.linePoints[i].isLine)
    {
      lineState = newLineState;
      return 1;
    }
  }

  // object
  // use milis
  // int currentTime = millis();
  // if (currentTime - lastTime > 1000)
  // {
  //   int d = sonar.getDistance();
  //   // Serial.println(d);
  //   lastTime = currentTime;
  //   if (d < 10)
  //   {
  //     return 2;
  //   }
  // }

  return 0;
}

uint8_t convertToBinary(lineSensorData line)
{
  uint8_t decimal = 00000000;
  for (int i = 0; i < 8; i++)
  {
    if (line.linePoints[i].isLine)
    {
      decimal |= 1 << i;
    }
  }
  //Serial.println(decimal);
  return decimal;
}
