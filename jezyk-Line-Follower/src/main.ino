#include <Arduino.h>
#include <motors.h>
#include <line.h>
#include <sonar.h>
#include <rotation.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <gripper.h>
#include <pixels.h>

#define A1_PIN 6
#define A2_PIN 7
#define B1_PIN 4
#define B2_PIN 5

boolean tryFindLine();

int linePins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
Motors motors(A1_PIN, A2_PIN, B2_PIN, B1_PIN);
Line line(8, linePins);
Sonar sonar(13, 12);
Rotation rotation(2, 3);
Gripper gripper1(8, 2, 1.9);
Pixels pixels(11);

static int lastTime = 0;
int distanceFront = 0;
int sonarFlag = 0;

static lineSensorData lineState;

// Communication
//  Define RX and TX pins for HC-12
const int HC12_RX = 0;
const int HC12_TX = 1;
SoftwareSerial HC12(HC12_RX, HC12_TX);
// ohter stuff needed
boolean isOnLine = false;
bool taskActive = false;
bool finished = false; // when robot finishes the job
int robotID = 1;

// Comminication
bool done = false;
unsigned long taskStartTime = 0;
const unsigned long taskDuration = 20000; // 20 seconds for demonstration
bool taskRunning = false;
long timer = millis();
int noLineFlag = 0;

boolean ready = false;

uint8_t lineForward[] = {
  0b00011000,
  0b01110000,
  0b00110000,
  0b01100000,
  0b11000000,
  0b00111000,
  0b00001100,
  0b00001110,
  0b00000110,
  0b00000011,
  0b00011100
};

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
  // comunication
  HC12.begin(9600);
  timer = millis();
}

void loop()
{
  communication();
  if (true)
  {
    if (!ready)
    {
      gripper1.gripper(gripper1._OpenGripper);
      startMaze();
    }
    else
    {
      //put your main code here, to run repeatedly:
     //int reading = 0;
      // Serial.println(millis() - timer);
      // if (millis() - timer > 100)
      // {
      //   timer = millis();
       //reading = sonar.getDistance();
      // }
      // if (reading != 0 && reading < 10) {
      //   Serial.println("Object read");
      //   sonarFlag++;
      // }
      // if (sonarFlag > 3)
      // {
      //   Serial.println("Object detected");
      //   motors.stop();
      //   rotation.moveBackwardFor(5);
      //   rotation.turnDegreesRight(45);
      //   rotation.moveForwardFor(5);
      //   while (detectChange() != 1)
      //   {
      //     motors.left();
      //   }
      //   sonarFlag = 0;
      // }
      lineState = line.readLine();
      uint8_t decimal = convertToBinary(lineState);
      switch (decimal)
      {
      default:
        break;

      case 0b00011000:
      motors.forward(255);
      isOnLine = true;
      
        break;
      case 0b10000000:
      case 0b01000000:
      case 0b00100000:
      case 0b00010000:
      case 0b11100000:
      case 0b11110000:
      case 0b01110000:
      case 0b00110000:
      case 0b01100000:
      case 0b11000000:
        motors.zeroLeft(255);
        isOnLine = true;
        break;
      case 0b00000001:
      case 0b00000010:
      case 0b00000100:
      case 0b00011100:
      case 0b00001000:
      case 0b00001100:
      case 0b00001110:
      case 0b00001111:
      case 0b00000110:
      case 0b00000011:
        motors.zeroRight(255);
        isOnLine = true;
        break;
      case 0b11111111:
        rotation.moveForwardFor(3);
        if (convertToBinary(line.readLine()) == 0b11111111)
        {
          pixels.braking_Pixel();
          motors.stop();
          rotation.moveBackwardFor(3);
          gripper1.gripper(gripper1._OpenGripper);
          pixels.back_Pixel();
          rotation.moveBackwardFor(10);
          finished = true;
          while (1)
          ;
        }
        
        break;
      case 0b00000000:
        isOnLine = false;
        // noLineFlag++;
        // if (noLineFlag < 50) break;
             
        // //tryFindLine();
        // while (convertToBinary(line.readLine()) != 0b00011000)
        // {
        //   motors.zeroLeft(255);
        // }
        // noLineFlag = 0;

        break;
      }
      

      
    }
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

// uint8_t convertToBinary(lineSensorData line)
// {
//   uint8_t decimal = 00000000;
//   for (int i = 0; i < 8; i++)
//   {
//     if (line.linePoints[i].isLine)
//     {
//       decimal |= 1 << i;
//     }
//   }
//   //Serial.println(decimal);
//   return decimal;
// }

void communication()
{
  if (HC12.available() > 0)
  {
    String receivedMessage = HC12.readStringUntil('\n');
    receivedMessage.trim();
    // Serial.println("Received: " + receivedMessage);

    // Parse the message
    String fields[3];
    int fieldCount = 0;
    splitString(receivedMessage, '|', fields, fieldCount);

    if (fieldCount > 1)
    {
      String senderID = fields[0];
      String command = fields[1];

      if (senderID = robotID)
      {
        if (command == "START")
        {
          taskActive = true;
          taskStartTime = millis();
          // Serial.println("Slave: Task started.");
        }
        else if (command == "REQUEST" && taskActive)
        {
          // data to send, front distance, left wheel speed, right wheel speed, line sensor data
          int sonar = distanceFront;
          float speed = rotation.calculateSpeed();
          // Perform task and send periodic updates
          // HC12.println("1|DATA|" + String(sonar)+"|"+String(speed) + "\n");
          if (isOnLine == true)
          {
            Serial.println("1|DATA| speed: " + String(speed) + "| linefollower: True | distance1: " + String(sonar));
          }
          else
          {
            Serial.println("1|DATA| speed: " + String(speed) + "| linefollower: False | distance1: " + String(sonar));
          }

          // motor.forward();
          // Serial.println("Slave: Sent data update.");
        }
        else if (finished == true)
        {
          Serial.println("1|DONE\n");
        }
      }
    }
  }
}

// Utility function to split a string
void splitString(String input, char delimiter, String output[], int &count)
{
  count = 0;
  int start = 0;
  int end = input.indexOf(delimiter);

  while (end != -1)
  {
    output[count++] = input.substring(start, end);
    start = end + 1;
    end = input.indexOf(delimiter, start);
  }
  output[count++] = input.substring(start);
}

void startMaze()
{
  // while (sonar.getDistance() < 10) // random number test to see
  // {
  //   motors.stop();
  // }
  //
  if (sonar.getDistance() > 20)
  {
    gripper1.gripper(gripper1._OpenGripper);
    pixels.normal_Pixel();
    rotation.moveForwardFor(9);
    pixels.braking_Pixel();
    motors.stop();
    lineState = line.readLine();
    tryFindLine();

    while (convertToBinary(line.readLine()) != 0b11111111)
    {
      moveWithLine();
    }
    rotation.moveForwardFor(1);
    pixels.braking_Pixel();
    motors.stop();
    gripper1.gripper(gripper1._CloseGripper);
    lineState = line.readLine();
    uint8_t decimal = convertToBinary(lineState);
    pixels.normal_Pixel();
    rotation.moveForwardFor(6);
    pixels.left_Pixel();
    rotation.turnDegreesLeft(70);
    rotation.moveForwardFor(3);
    tryFindLine();
    while (convertToBinary(line.readLine()) != 0b00011000)
    {
      pixels.normal_Pixel();
      motors.zeroLeft();
    }
    pixels.braking_Pixel();
    motors.stop();
    ready = true;
  }
}

// //when robot reaches the end of the line
// void findEnd()
// {
//   lineState = line.readLine();
//   while (convertToBinary(line.readLine()) != 0b11111111)
//   {
//     lineState = line.readLine();
//     if(detectChange() == 1){
//       if(tryFindLine() == true){
//         isOnLine = true; //communication
//         moveWithLine();
//       }
//     }
//   }

// }
int Find(uint8_t arr[], uint8_t x)
{
  int n = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < n; i++)
    if (arr[i] == x)
      return i;
  return -1;
}

boolean tryFindLine() {
  long timer = millis();
  while (millis() - timer < 200)
  {
    motors.zeroLeft();
    if (Find(lineForward,convertToBinary(line.readLine())) != -1)
    {
      return true;
    }
  }
  timer = millis();
  while (millis() - timer < 400)
  {
    motors.zeroRight();
    if (Find(lineForward,convertToBinary(line.readLine())) != -1)
    {
      return true;
    }
  }
  return false;
}


void moveWithLine() {
  lineState = line.readLine();
  uint8_t decimal = convertToBinary(lineState);
  switch (decimal)
  {
  default:
      motors.stop();
      break;
  case 0b00011000:
      motors.forward();
      break;
  case 0b01110000:
  case 0b00110000:
  case 0b01100000:
  case 0b11000000:
  case 0b00111000:
      motors.zeroLeft();
      break;
  case 0b00001100:
  case 0b00001110:
  case 0b00000110:
  case 0b00000011:
  case 0b00011100:
      motors.zeroRight();
      break;
  }
}