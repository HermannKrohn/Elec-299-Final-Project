#include <Servo.h>
#include "QSerial.h"
QSerial myIRserial;

int type = 0;//initial position. left-0, middle-1, right-2

int cy = 0;
int cx = 0;
int cd = 0;
int x = 0;
int y = 0;
int d = 0;
int xIn;
int yIn;
int dIn;
int delayTLeft = 500; //500 when half bat 400 when full bat
int delayTRight = 500; //500 when half bat 400 when full bat
int delayBeforeT = 200; // 350 when half 250 when full
int lSP = 180;
int rSP = 190;
int turnMod = 90; // 80 when half 90 when full
int resetTMod = 0; // subtracts from turn speed after drop dice. 10 when full -10 when half;
int turnADPD = 140; // 150 when full 200 when half
int intersectMilli = 150; // 130 full 180 low

int leftBumper = 1;
int rightBumper = 1;

int IRpin = A3;

int forceSensor = A4;
int forceReading = 1027;
int gripThresh = 700;
int fIRThresh = 300;


int lIRPin = A0;
int cIRPin = A2;
int rIRPin = A1;

int lVal = 0;
int cVal = 0;
int rVal = 0;

int plVal = 0;
int pcVal = 0;
int prVal = 0;

int leftSpeed = 5;
int leftDirection = 4;
int rightSpeed = 6;
int rightDirection = 7;
int thresh = 900;
unsigned long lastInter = 0;
int IRVal = 0;

//---------------------------------
int arrayLeft[5][3] = {{1, 1, 3},
  {1, 5, 0},
  {5, 5, 1},
  {1, 2, 3},
  {1, 4, 3}
};
int arrayCentre[5][3] = {{1, 3, 3},
  {3, 5, 0},
  {5, 3, 1},
  {2, 5, 0},
  {4, 5, 0}
};
int arrayRight[5][3] = {{1, 5, 3},
  {5, 5, 0},
  {5, 1, 1},
  {5, 2, 1},
  {5, 4, 1}
};

int Destinations[5][3];

int txpin = -1;
int rxpin = 12;

char Start1 = '0';
char Start2 = '1';
char Start3 = '2';

//---------------------------------

int panPIN = 8;
int tiltPIN = 9;
int gripPIN = 10;

Servo pan, tilt, grip;
//--------------------------------
int lBump = 2;
int rBump = 3;

void setup() {
  // put your setup code here, to run once:
  pan.attach(panPIN);
  tilt.attach(tiltPIN);
  grip.attach(gripPIN);

  // put your setup code here, to run once:
  pinMode(lIRPin, INPUT);
  pinMode(cIRPin, INPUT);
  pinMode(rIRPin, INPUT);

  pinMode(leftSpeed,  OUTPUT);
  pinMode(leftDirection, OUTPUT);
  pinMode(rightSpeed, OUTPUT);
  pinMode(rightDirection, OUTPUT);

  pinMode(lBump, INPUT);
  pinMode(rBump, INPUT);

  pinMode(IRpin, INPUT);
  pinMode(forceSensor, INPUT);

  pan.write(100);
  tilt.write(190);
  grip.write(0);
  myIRserial.attach(rxpin, txpin);
  GetIR();

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("BEGINING LOOP()");
  for (int i = 0; i < 5; i++) {
    xIn = Destinations[i][0];
    yIn = Destinations[i][1];
    dIn = Destinations[i][2];
    GoToDice(xIn, yIn, dIn);
    approachDice();
    GoToBin(type);
    approachBin();
    delay(100);
  }
//forward(2);
//delay(1000);
//    xIn = 3;
//    yIn = 3;
//    dIn = 1;
//    GoToDice(xIn, yIn, dIn);
//    approachDice();
//    GoToBin(type);
//    approachBin();
//    delay(100);
//    xIn = 1;
//    yIn = 5;
//    dIn = 0;
//    GoToDice(xIn, yIn, dIn);
//    approachDice();
//    GoToBin(type);
//    approachBin();
//    delay(100);
//    xIn = 5;
//    yIn = 4;
//    dIn = 1;
//    GoToDice(xIn, yIn, dIn);
//    approachDice();
//    GoToBin(type);
//    approachBin();
//    delay(100);
  //  xIn = 1;
  //  yIn = 2;
  //  dIn = 3;
  //  GoToDice(xIn, yIn, dIn);
  //  approachDice();
  //  GoToBin(type);
  //  approachBin();
  //  delay(100);
  //  xIn = 1;
  //  yIn = 3;
  //  dIn = 3;
  //  GoToDice(xIn, yIn, dIn);
  //  approachDice();
  //  GoToBin(type);
  //  approachBin();
  exit(0);
}

void GetIR() {
  Serial.begin(115200);
  char val = -1;

  while (Destinations[0][0] == 0) {
    val = myIRserial.receive(200);
    type = 0;
    Serial.println(val);

    if (val == Start1) {
      Serial.println("Start Location 1");
      for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
          Destinations[i][j] = arrayLeft[i][j];
          Serial.println(Destinations[i][j]);
        }
      }
    }

    else if (val == Start2) {
      type = 1;
      Serial.println("Start Location 2");
      for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
          Destinations[i][j] = arrayCentre[i][j];
          Serial.println(Destinations[i][j]);
        }
      }
    }

    else if (val == Start3) {
      type = 2;
      Serial.println("Start Location 3");
      for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
          Destinations[i][j] = arrayRight[i][j];
          Serial.println(Destinations[i][j]);
        }
      }
    }

    else {
      Serial.println("Invalid transmitted value.");
    }
  }
}

//Function for robot to move to a specified ball's location
void GoToDice(int xIn1, int yIn1, int dIn1)
{
  x = xIn1;
  y = yIn1;
  d = dIn1;
  cx = type + 2;
  cy = 0;
  if (cy < y) { //if current y coordinate is less than the objective y then move forward until current y is equal to that of the objective
    forward(y - cy);
    cy = y;

  }
  if (cx != x) { //if the objective is not directly infront of the robot then turn in the direction of the ball and go forward until the ball is in front of the robot
    if (cx < x) {
      turn(0);
      forward(x - cx);
      cx = x;
    }
    else if (cx > x) {
      turn(1);
      forward(cx - x);
      cx = x;
    }

  }
  if (cd != d) { //if the robot is not facing the ball, then turn towards the ball
    if (abs(cd - d) == 2) {
      turn(1);
      turn(1);
    }
    else if (cd - d == -3) {
      turn (1);
    }
    else if (cd - d == 3) {
      turn (0);
    }
    else if (cd - d > 0)
    {
      turn(1);
    }
    else if (cd - d < 0)
    {
      turn(0);
    }
  }
  analogWrite(leftSpeed, lSP);
  analogWrite(rightSpeed, rSP);

}

void GoToBin(int typeIn)//left-0, middle-1 or right-2
{
  y = 0;
  x = typeIn + 2; //gives location of bin depending on starting position
  int a = 0;
  Serial.println(cx);
  Serial.println(cy);

  if (cd == 2)
  {

    Serial.println("direction fix");
    if (cx < x) {
      forward(1);
      turn(1);
    }
    else if (cx > x) {
      forward(1);
      turn(0);
    }
  }

  if (cx != x) { //if the objective is not directly infront of the robot then turn in the direction of the ball and go forward until the ball is in front of the robot
    if (cx < x) {
      Serial.println("Going forward");
      forward(x - cx);
      cx = x;
      turn(0);//turn right

      Serial.println("turning");

    }
    else if (cx > x) {
      Serial.println("Going forward");

      forward(cx - x);
      cx = x;
      turn(1);//turn left
    }

  }
  if (cy != y) { //if current y coordinate is less than the objective y then move forward until current y is equal to that of the objective
    if (yIn == 5 && dIn == 0) {
      cy = cy - 1;
    }
    forward(cy - y - 1);
    cy = y;
  }
  analogWrite(leftSpeed, lSP - 60);
  analogWrite(rightSpeed, rSP - 60);

  //drop into bin function
}

void sweep(int lastSide) {
  int sweepMod = 20;
  if (lastSide == 0)//left
  {
    digitalWrite(leftDirection, LOW);
    digitalWrite(rightDirection, HIGH);
    analogWrite(leftSpeed, lSP + sweepMod);
    analogWrite(rightSpeed, rSP + sweepMod);
    cVal = analogRead(cIRPin);
    while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
      cVal = analogRead(cIRPin);
    }
  }
  else {
    digitalWrite(leftDirection, HIGH);
    digitalWrite(rightDirection, LOW);
    analogWrite(leftSpeed, lSP + sweepMod);
    analogWrite(rightSpeed, rSP + sweepMod);
    cVal = analogRead(cIRPin);
    while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
      cVal = analogRead(cIRPin);
    }
  }
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
  delay(50);
}


//Function to move robot forward a specified number of intersections
void forward(int numOfIntersections) {
  int intersectionCount = 0;
  digitalWrite(leftDirection, HIGH); //Set left motor direction to forward
  digitalWrite(rightDirection, HIGH); //Set right motor direction to forward

  //While not at the desired intersection, keep moving forward
  //    while (intersectionCount < numOfIntersections) {
  //      lVal = analogRead(lIRPin);
  //      cVal = analogRead(cIRPin);
  //      rVal = analogRead(rIRPin);
  //      IRVal = analogRead(IRpin);
  //
  //
  //      Serial.print(lVal);
  //      Serial.print(" || ");
  //      Serial.print(cVal);
  //      Serial.print(" || ");
  //      Serial.println(rVal);
  //      if (IRVal > fIRThresh) {
  //        analogWrite(leftSpeed, 0);
  //        analogWrite(rightSpeed, 0);
  //      }
  //      else  if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
  //        analogWrite(leftSpeed, lSP);
  //        analogWrite(rightSpeed, rSP);
  //      } else if ((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //        analogWrite(leftSpeed, lSP - 80);
  //        analogWrite(rightSpeed, rSP);
  //      } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh)) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //        analogWrite(leftSpeed, lSP);
  //        analogWrite(rightSpeed, rSP - 80);
  //      }
  //      else {
  //        analogWrite(leftSpeed, lSP - 30);
  //        analogWrite(rightSpeed, rSP - 30);
  //      }
  //
  //      if ((lVal > thresh) && (cVal > thresh) && (rVal > thresh) && (plVal > thresh) && (pcVal > thresh) && (prVal > thresh)) { //At an intersection. Increment intersection counter
  //        if ((millis() - lastInter) > intersectMilli) {
  //          Serial.println("INTERSECTION");
  //
  //          intersectionCount++;
  //          Serial.println(intersectionCount);
  //          lastInter = millis();
  //
  //        }
  //      }
  //      plVal = lVal;
  //      pcVal = cVal;
  //      prVal = rVal;
  //    }
  while (intersectionCount < numOfIntersections) {//While not at the desired intersection, keep moving forward
    lVal = analogRead(lIRPin);
    cVal = analogRead(cIRPin);
    rVal = analogRead(rIRPin);
    IRVal = analogRead(IRpin);
    int count = 0;
    Serial.println("intersection loop");
    while (count < 10) {
      lVal = analogRead(lIRPin);
      cVal = analogRead(cIRPin);
      rVal = analogRead(rIRPin);
      IRVal = analogRead(IRpin);
      //      followLineF(0);
      if (IRVal > fIRThresh) {
        analogWrite(leftSpeed, 0);
        analogWrite(rightSpeed, 0);
        delay(1500);
      }
      else  if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
        analogWrite(leftSpeed, lSP);
        analogWrite(rightSpeed, rSP);
      } else if ((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
        analogWrite(leftSpeed, lSP - 90);
        analogWrite(rightSpeed, rSP);
      } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh)) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
        analogWrite(leftSpeed, lSP);
        analogWrite(rightSpeed, rSP - 90);
      }
      else {
        analogWrite(leftSpeed, lSP - 30);
        analogWrite(rightSpeed, rSP - 30);
      }
      //      Serial.println("count loop");
      if ((analogRead(lIRPin) > thresh) && (analogRead(rIRPin) > thresh)) count++;
      else count = 0;
      delay(1);
    }
    int temp = intersectionCount;
    count = 0;
    while (intersectionCount < temp + 1) {
      lVal = analogRead(lIRPin);
      cVal = analogRead(cIRPin);
      rVal = analogRead(rIRPin);
      IRVal = analogRead(IRpin);
      while (count < 12) {
        lVal = analogRead(lIRPin);
        cVal = analogRead(cIRPin);
        rVal = analogRead(rIRPin);
        IRVal = analogRead(IRpin);
        //        followLineF(0);
        if (IRVal > fIRThresh) {
          analogWrite(leftSpeed, 0);
          analogWrite(rightSpeed, 0);
              delay(1500);
        }
        else  if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
          analogWrite(leftSpeed, lSP);
          analogWrite(rightSpeed, rSP);
        } else if ((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
          analogWrite(leftSpeed, lSP - 90);
          analogWrite(rightSpeed, rSP);
        } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh)) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
          analogWrite(leftSpeed, lSP);
          analogWrite(rightSpeed, rSP - 90);
        }
        else {
          analogWrite(leftSpeed, lSP - 30);
          analogWrite(rightSpeed, rSP - 30);
        }
        if ((analogRead(lIRPin) < thresh) || (analogRead(rIRPin) < thresh)) count++;
        else count = 0;
        delay(1);
      }
      intersectionCount++;
      Serial.println("Intersections" + (String) intersectionCount);
    }
  }
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
  delay(100);
}
//Function to turn in a specified direction until a black line is hit
void turn(int dir) {
  //drive a little past the intersection
  delay(50);
  analogWrite(leftSpeed, lSP - turnMod);
  analogWrite(rightSpeed, rSP - turnMod);
  digitalWrite(leftDirection, HIGH);
  digitalWrite(rightDirection, HIGH);
  delay(delayBeforeT);


  if (dir == 1) { //Begin turning counter-clockwise to ensure that center line sensor is not scaning the black tape
    analogWrite(leftSpeed, lSP - turnMod);
    analogWrite(rightSpeed, rSP - turnMod);
    digitalWrite(leftDirection, LOW);
    digitalWrite(rightDirection, HIGH);
    delay(delayTLeft);
    cVal = analogRead(cIRPin);
    while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
      cVal = analogRead(cIRPin);
      if ((cVal > thresh) && (pcVal > thresh)) { //At an intersection. Increment intersection counter
        if ((millis() - lastInter) > 100) {
          analogWrite(leftSpeed, 0);
          analogWrite(rightSpeed, 0);
          lastInter = millis();

        }
      }
      pcVal = cVal;
    }
    if (cd == 0) { //Update direction robot is facing
      cd = 3;
    } else {
      cd = cd - 1;
    }
  }

  else { //Begin turning clockwise to ensure that center line sensor is not scaning the black tape
    analogWrite(leftSpeed, lSP - turnMod);
    analogWrite(rightSpeed, rSP - turnMod);
    digitalWrite(leftDirection, HIGH);
    digitalWrite(rightDirection, LOW);
    delay(delayTRight);
    cVal = analogRead(cIRPin);
    while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
      cVal = analogRead(cIRPin);
      if ((cVal > thresh) && (pcVal > thresh)) { //At an intersection. Increment intersection counter
        if ((millis() - lastInter) > 100) {
          analogWrite(leftSpeed, 0);
          analogWrite(rightSpeed, 0);
          lastInter = millis();
        }
      }
      pcVal = cVal;
    }
    cd = (cd + 1) % 4; //Update direction robot is facing
  }

  if (dir == 1) {
    digitalWrite(leftDirection, LOW);
    digitalWrite(rightDirection, HIGH);
    analogWrite(leftSpeed, lSP - turnMod);
    analogWrite(rightSpeed, rSP - turnMod);
  } else {
    digitalWrite(leftDirection, HIGH);
    digitalWrite(rightDirection, LOW);
    analogWrite(leftSpeed, lSP - turnMod);
    analogWrite(rightSpeed, rSP - turnMod);
  }

  //  if (dir = 1) {
  //    cVal = analogRead(cIRPin);
  //    if (cVal < (thresh)) {
  //      while (cVal < (thresh)) {
  //        cVal = analogRead(cIRPin);
  //        digitalWrite(leftDirection, HIGH);
  //        digitalWrite(rightDirection, LOW);
  //        analogWrite(leftSpeed, lSP - turnMod);
  //        analogWrite(rightSpeed, rSP - turnMod);
  //        while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
  //          cVal = analogRead(cIRPin);
  //          if ((cVal > thresh) && (pcVal > thresh)) { //At an intersection. Increment intersection counter
  //            if ((millis() - lastInter) > 100) {
  //              analogWrite(leftSpeed, 0);
  //              analogWrite(rightSpeed, 0);
  //              lastInter = millis();
  //
  //            }
  //          }
  //          pcVal = cVal;
  //        }
  //      }
  //
  //    }
  //  }
  //  else {
  //    cVal = analogRead(cIRPin);
  //    if (cVal < (thresh)) {
  //      while (cVal < (thresh)) {
  //        cVal = analogRead(cIRPin);
  //        digitalWrite(leftDirection, LOW);
  //        digitalWrite(rightDirection, HIGH);
  //        analogWrite(leftSpeed, lSP - turnMod);
  //        analogWrite(rightSpeed, rSP - turnMod);
  //        while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
  //          cVal = analogRead(cIRPin);
  //          if ((cVal > thresh) && (pcVal > thresh)) { //At an intersection. Increment intersection counter
  //            if ((millis() - lastInter) > 100) {
  //              analogWrite(leftSpeed, 0);
  //              analogWrite(rightSpeed, 0);
  //              lastInter = millis();
  //
  //            }
  //          }
  //          pcVal = cVal;
  //        }
  //      }
  //    }
  //  }
  Serial.println("sweep");
  lVal = analogRead(lIRPin);
  cVal = analogRead(cIRPin);
  rVal = analogRead(rIRPin);
  delay(100);
  if (cVal < thresh) {
    sweep(dir);
  }
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
}

void turnWithDice()
{
  digitalWrite(leftDirection, HIGH);
  digitalWrite(rightDirection, HIGH);
  unsigned long timer = millis();
  while ((millis() - timer) < 200) {
    if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
      analogWrite(leftSpeed, lSP);
      analogWrite(rightSpeed, rSP);
    } else if (((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) || ((lVal > thresh) && (cVal > thresh) && (rVal < thresh))) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
      analogWrite(leftSpeed, lSP - 90);
      analogWrite(rightSpeed, rSP );
    } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh) || ((lVal < thresh) && (cVal > thresh) && (rVal > thresh))) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
      analogWrite(leftSpeed, lSP );
      analogWrite(rightSpeed, rSP - 90);
    }

  }
  analogWrite(leftSpeed, lSP - turnMod + 20);
  analogWrite(rightSpeed, rSP - turnMod + 20);
  digitalWrite(leftDirection, HIGH);
  digitalWrite(rightDirection, LOW);
  delay(700);
  analogWrite(leftSpeed, lSP - turnMod + 5);
  analogWrite(rightSpeed, rSP - turnMod + 5);
  delay(delayTLeft);
  cVal = analogRead(cIRPin);
  while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
    cVal = analogRead(cIRPin);
    if ((cVal > thresh) && (pcVal > thresh)) { //At an intersection. Increment intersection counter
      if ((millis() - lastInter) > 100) {
        analogWrite(leftSpeed, 0);
        analogWrite(rightSpeed, 0);
        lastInter = millis();

      }
    }
    pcVal = cVal;
  }
  if (cd == 0) { //Update direction robot is facing
    cd = 2;
  }
  else if (cd == 1) {
    cd = 3;
  }
  else if (cd == 2)
  {
    cd = 0;
  }
  else //cd == 3
  {
    cd = 1;
  }
  //    int a=30;
  //    digitalWrite(leftDirection, HIGH);
  //    digitalWrite(rightDirection,HIGH);
  //    if((lVal < thresh) && (cVal > thresh) && (rVal < thresh)){ //SET MOTORS TO DRIVE FORWARD
  //      analogWrite(leftSpeed, lSP-turnMod+a);
  //      analogWrite(rightSpeed, rSP-turnMod+a);
  //    }else if((lVal > thresh) && (cVal < thresh) && (rVal < thresh)){//LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //      analogWrite(leftSpeed, lSP-turnMod-40+a);
  //      analogWrite(rightSpeed, rSP-turnMod+20+a);
  //    }else if((lVal < thresh) && (cVal < thresh) && (rVal > thresh)){//LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //      analogWrite(leftSpeed, lSP-turnMod+20+a);
  //      analogWrite(rightSpeed, rSP-turnMod-40+a);
  //    }
  //    delay(600);
  //  if (dIn == 0 && yIn == 5) {
  //    forward(1);
  //  }
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
  delay(50);
  //  Serial.println(cd);
  //    exit(0);

}

void closeGrip() {
  int i = 0;
  int x = 190;
  while (x > 65) {
    tilt.write(x);
    x--;
    delay(10);
  }

  forceReading = analogRead(forceSensor);


  while (forceReading < gripThresh) {
    forceReading = analogRead(forceSensor);

    i++;
    int j = i % 110;

    grip.write(j);
    delay(15);

    if (j == 0) {
      delay(500);
    }
  }
  tilt.write(190);
  Serial.println("OBJECT GRABBED");
}

void dropDice() {
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
  int x = 190;
  while (x > 65) {
    tilt.write(x);
    x--;
    if (x == 80) {
      grip.write(0);
      Serial.println("Dice Dropped");
    }
  }
}


void approachDice() {
  if (cd == 3)
  {
    cx--;
  }
  else if (cd == 0)
  {
    cy++;
  }
  else if (cd == 1)
  {
    cx++;
  }

  IRVal = analogRead(IRpin);
  leftBumper = digitalRead(lBump);
  rightBumper = digitalRead(rBump);
  while (leftBumper && rightBumper)
  {
    leftBumper = digitalRead(lBump);
    rightBumper = digitalRead(rBump);
    digitalWrite(leftDirection, HIGH);
    digitalWrite(rightDirection, HIGH);

    lVal = analogRead(lIRPin);
    cVal = analogRead(cIRPin);
    rVal = analogRead(rIRPin);


    if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
      analogWrite(leftSpeed, lSP - turnMod);
      analogWrite(rightSpeed, rSP - turnMod);
    } else if ((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
      analogWrite(leftSpeed, lSP - turnMod);
      analogWrite(rightSpeed, rSP - turnMod + 60);
    } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh)) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
      analogWrite(leftSpeed, lSP - turnMod + 60);
      analogWrite(rightSpeed, rSP - turnMod);
    }

  }
  digitalWrite(leftDirection, LOW);
  digitalWrite(rightDirection, LOW);
  analogWrite(leftSpeed, lSP - turnMod);
  analogWrite(rightSpeed, rSP - turnMod);
  delay(300);
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
  closeGrip();

  turnWithDice();

  //  if((lVal < thresh) && (cVal > thresh) && (rVal < thresh)){ //SET MOTORS TO DRIVE FORWARD
  //      analogWrite(leftSpeed, 110);
  //      analogWrite(rightSpeed, 100);
  //    }else if((lVal > thresh) && (cVal < thresh) && (rVal < thresh)){//LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //      analogWrite(leftSpeed, 160);
  //      analogWrite(rightSpeed, 80);
  //    }else if((lVal < thresh) && (cVal < thresh) && (rVal > thresh)){//LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //      analogWrite(leftSpeed, 100);
  //      analogWrite(rightSpeed, 140);
  //    }
  //  delay(1000);

}

void approachBin() {
  IRVal = analogRead(IRpin);
  leftBumper = digitalRead(lBump);
  rightBumper = digitalRead(rBump);
  while (leftBumper && rightBumper)
  {
    leftBumper = digitalRead(lBump);
    rightBumper = digitalRead(rBump);
    digitalWrite(leftDirection, HIGH);
    digitalWrite(rightDirection, HIGH);
    IRVal = analogRead(IRpin);
    lVal = analogRead(lIRPin);
    cVal = analogRead(cIRPin);
    rVal = analogRead(rIRPin);


    if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
      analogWrite(leftSpeed, lSP);
      analogWrite(rightSpeed, rSP);
    } else if ((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
      analogWrite(leftSpeed, lSP - 80);
      analogWrite(rightSpeed, rSP );
    } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh)) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
      analogWrite(leftSpeed, lSP );
      analogWrite(rightSpeed, rSP - 80);
    }
  }

  dropDice();
  Serial.println("dice dropped");
  digitalWrite(leftDirection, LOW);
  digitalWrite(rightDirection, LOW);
  analogWrite(leftSpeed, lSP - 40);
  analogWrite(rightSpeed, rSP - 40);
  delay(500);
  Serial.println("went back");
  //  turn(0);
  analogWrite(leftSpeed, lSP - turnMod + 5);
  analogWrite(rightSpeed, rSP - turnMod + 5);
  digitalWrite(leftDirection, LOW);
  digitalWrite(rightDirection, HIGH);
  delay(delayTLeft);
  cVal = analogRead(cIRPin);
  while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
    cVal = analogRead(cIRPin);
    if ((cVal > thresh) && (pcVal > thresh)) { //At an intersection. Increment intersection counter
      if ((millis() - lastInter) > 100) {
        analogWrite(leftSpeed, 0);
        analogWrite(rightSpeed, 0);
        lastInter = millis();

      }
    }
    pcVal = cVal;
  }
  delay(100);
  sweep(1);
  delay(200);
  //  analogWrite(leftSpeed, lSP - turnMod - resetTMod);
  //  analogWrite(rightSpeed, rSP - turnMod - resetTMod);
  //  digitalWrite(leftDirection, HIGH);
  //  digitalWrite(rightDirection, LOW);
  //  delay(1000);
  //  cVal = analogRead(cIRPin);
  //  while (cVal < thresh) { //Continue rotating in specified direction until the center line sensor reads the black tape value
  //    cVal = analogRead(cIRPin);
  //    digitalWrite(leftDirection, HIGH);
  //    digitalWrite(rightDirection, LOW);
  //    analogWrite(leftSpeed, lSP - turnMod - resetTMod);
  //    analogWrite(rightSpeed, rSP - turnMod - resetTMod);
  //  }
  //  digitalWrite(leftDirection, LOW);
  //  digitalWrite(rightDirection, HIGH);
  //  analogWrite(leftSpeed, lSP);
  //  analogWrite(rightSpeed, rSP);
  //  delay(100);
  //  digitalWrite(leftDirection, HIGH);
  //  digitalWrite(rightDirection, HIGH);
  //  unsigned long timer = millis();
  //  while ((millis() - timer) < turnADPD) {
  //    if ((lVal < thresh) && (cVal > thresh) && (rVal < thresh)) { //SET MOTORS TO DRIVE FORWARD
  //      analogWrite(leftSpeed, lSP);
  //      analogWrite(rightSpeed, rSP);
  //    } else if (((lVal > thresh) && (cVal < thresh) && (rVal < thresh)) || ((lVal > thresh) && (cVal > thresh) && (rVal < thresh))) { //LEANING INTO THE RIGHT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //      analogWrite(leftSpeed, lSP - 90);
  //      analogWrite(rightSpeed, rSP);
  //    } else if ((lVal < thresh) && (cVal < thresh) && (rVal > thresh) || ((lVal < thresh) && (cVal > thresh) && (rVal > thresh))) { //LEANING INTO THE LEFT...SPEED UP RIGHT MOTOR (CALIBRATE)
  //      analogWrite(leftSpeed, lSP);
  //      analogWrite(rightSpeed, rSP - 90);
  //    } else {
  //      analogWrite(leftSpeed, lSP - 30);
  //      analogWrite(rightSpeed, rSP - 30);
  //    }
  //
  //  }
  analogWrite(leftSpeed, 0);
  analogWrite(rightSpeed, 0);
  cd = 0;
  delay(50);
  tilt.write(190);
  delay(50);
}
