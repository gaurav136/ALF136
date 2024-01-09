#include <SharpIR.h>
#define Kp 0.21  // experiment to determine this, start by something small that just makes your bot follow the line at a slow speed
#define Kd 0     // experiment to determine this, slowly increase the speeds and adjust this value. ( Note: Kp < Kd) 
#define Ki 0


#define MaxSpeed 180      // max speed of the robot
#define BaseSpeed 180     // this is the speed at which the motors should spin when the robot is perfectly on the line
#define SensorCount  8    // number of sensors used

#define speedturn 40

#define ena  5
#define in1  6
#define in2  7
#define in3  8
#define in4  9
#define enb  10


#define ir A6
#define model 1080

#define tir 4
SharpIR SharpIR(ir, model);

int P;
int I;
int D;
int rightMotorSpeed;
int leftMotorSpeed;

const uint8_t SensorPins[SensorCount] = {A0, A1, A2, A3, A4, A5, 3, 2};

int lastError = 0;
unsigned int Sensor[8];

int botPosition() {
  int Position = 0;
  int sensor = 0;
  for (int i = 0 ; i < SensorCount; i++) {
    Position += i * 1000 * digitalRead(SensorPins[i]);
    sensor += digitalRead(SensorPins[i]);
  }
  Position /= sensor;
  return Position ;
}

void move(int motor, int Speed, int Direction) {
  boolean inPin1 = HIGH;
  boolean inPin2 = LOW;

  if (Direction == 1) {
    inPin1 = HIGH;
    inPin2 = LOW;
  }
  if (Direction == 0) {
    inPin1 = LOW;
    inPin2 = HIGH;
  }

  if (motor == 0) {
    digitalWrite(in1, inPin1);
    digitalWrite(in2, inPin2);
    analogWrite(ena, Speed);
  }
  if (motor == 1) {
    digitalWrite(in3, inPin1);
    digitalWrite(in4, inPin2);
    analogWrite(enb, Speed);
  }
}


void setup()
{
  for (int i = 0; i < SensorCount ; i++) {
    pinMode(SensorPins[i], INPUT);
  }
  pinMode(tir, INPUT);

  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);


  delay(2000); // wait for 2s to position the bot before entering the main loop
  move(0, 100, 1);
  move(1, 100, 1);
  delay(800);
}



void loop()
{

  int Position = botPosition();

  if (
    (digitalRead(SensorPins[0]) == 1)
    && (digitalRead(SensorPins[1]) == 1)
    && (digitalRead(SensorPins[2]) == 1)
    && (digitalRead(SensorPins[3]) == 1)
    && (digitalRead(SensorPins[4]) == 1)
    && (digitalRead(SensorPins[5]) == 1)
    && (digitalRead(SensorPins[6]) == 1)
    && (digitalRead(SensorPins[7]) == 1)
    && (digitalRead(tir)           == 1)
  ) {
    // stop the bot at once
    move(0, 0, 0);
    move(1, 0, 0);
    Serial.print("stop");
    delay(10000);
    move(1, 100, 1);
    move(0, 100, 1);
    return ;
  }

  if (
    (digitalRead(SensorPins[0]) == 1)
    && (digitalRead(SensorPins[1]) == 1)
    && (digitalRead(SensorPins[2]) == 1)
    && (digitalRead(SensorPins[3]) == 1)
    && (digitalRead(SensorPins[4]) == 1)
    && (digitalRead(SensorPins[5]) == 0)
    && (digitalRead(SensorPins[6]) == 0)
    && (digitalRead(SensorPins[7]) == 0)
    && (digitalRead(tir)           == 1)
  ) {
    // stop the bot at once
    move(0, 0, 0);
    move(1, 0, 0);

    delay(1000);

    while (Position > 1000) {
      Position = botPosition();
      //Serial.println(Position);
      move(1, speedturn, 1);
      move(0, speedturn, 0);
      // delay(2000);
    }

    return ;
  }


  int dis = SharpIR.distance();

  if ( dis <= 8) {
    while (Position < 6000) {
      Position = botPosition();
      //Serial.println(Position);
      move(1, speedturn, 1);
      move(0, speedturn, 0);
      // delay(2000);
    }
    return;
  }


  if (Position == -1) {
    if (rightMotorSpeed > leftMotorSpeed) {
      leftMotorSpeed = 0;
      move(1, rightMotorSpeed, 1);
      move(0, leftMotorSpeed, 1);

      return;
    }
    if (rightMotorSpeed < leftMotorSpeed) {
      rightMotorSpeed = 0;
      move(1, rightMotorSpeed, 1);
      move(0, leftMotorSpeed, 1);

      return;
    }

  }
  int  error = Position - 3500;
  P = error;
  I = I + error;
  D = error - lastError;
  int motorSpeedDiff = Kp * P + Kd * D + Ki * I;
  lastError = error;

  rightMotorSpeed = BaseSpeed + motorSpeedDiff;
  leftMotorSpeed = BaseSpeed - motorSpeedDiff;


  if (rightMotorSpeed > MaxSpeed ) rightMotorSpeed = MaxSpeed; // prevent the motor from going beyond max speed
  if (leftMotorSpeed > MaxSpeed ) leftMotorSpeed = MaxSpeed; // prevent the motor from going beyond max speed
  if (rightMotorSpeed < 0)rightMotorSpeed = 0;
  if (leftMotorSpeed < 0)leftMotorSpeed = 0;


  move(1, rightMotorSpeed, 1);
  move(0, leftMotorSpeed, 1);
}
