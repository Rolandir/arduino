#include <Servo.h>
#include <NewPing.h>

const int TRIG = A2;
const int ECHO = A3;
long lecture_echo;

//using the servo.h disables pwm output on pins 9 and 10 when a servo is initalised
//your pwm is on pin 9 so nothing will happen.
//the pwm is blocked so no speed signal is being sent
//Move the enA from pin9 to another pwm pin
//https://www.reddit.com/r/arduino/comments/nvdfef/l298n_with_dc_motor_stops_working_when_i_attach_a/?rdt=41577
#define borneENA        5
#define borneIN1        4
#define borneIN2        8
#define borneIN3        6
#define borneIN4        7
#define borneENB        3

#define servoMOTOR      11

const int LeftMotorForward   = 6;
const int LeftMotorBackward  = 7;
const int RightMotorBackward = 4;
const int RightMotorForward  = 8;

const int delayUpdateSpeed = 20;
const int minimalSpeed  = 99;
const int maximalSpeed  = 151;

#define sonar_2 2

enum Direction {MARCHE_AVANT = 'V',  MARCHE_ARRIERE = 'R', TURN_RIGHT = 'T', TURN_LEFT = 'L'};
int distance = 100;

#define maximum_distance 200
NewPing sonar(TRIG, ECHO, maximum_distance);
NewPing sonar2(sonar_2, sonar_2, maximum_distance);
Servo servo_motor;

void updateMotorSpeed(int speed);
void updateBridgeConfiguration(Direction direction);
int  readPing();
int  lookRight();
int  lookLeft();
void stopMotors();
void DoGo(Direction);
void DoGoWithStepping(Direction);
void DoProcess(int);
int  lookSide(Direction);

void setup() {
  servo_motor.attach(servoMOTOR);
  servo_motor.write(90);

  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);

  pinMode(borneENA, OUTPUT);
  pinMode(borneIN1, OUTPUT);
  pinMode(borneIN2, OUTPUT);
  pinMode(borneIN3, OUTPUT);
  pinMode(borneIN4, OUTPUT);
  pinMode(borneENB, OUTPUT);
}

void loop() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(11);
  digitalWrite(TRIG, LOW);

  delay(50);
  distance = readPing();
  Serial.print(">Distance:");
  Serial.println(distance);

  int sonarbis=sonar2.ping_cm();
  Serial.print(">sonarbis:");
  Serial.println(sonarbis);
  distance=min(sonarbis, distance);

  DoProcess(distance);

}

void DoProcess(int distance) {
  if (distance < 30) {
    int distanceLeft = 0;
    int distanceRight = 0;
    stopMotors();
    delay(200);
    DoGo(MARCHE_ARRIERE);
    delay(400);
    stopMotors();
    delay(200);
    distanceRight = lookSide(TURN_RIGHT);
    Serial.print(">DistanceRight:");
    Serial.println(distanceRight);
    delay(300);
    distanceLeft = lookSide(TURN_LEFT);
    Serial.print(">DistanceLeft:");
    Serial.println(distanceLeft);  
    delay(300);
    if (distanceRight > distanceLeft && distanceRight > distance) {
      Serial.print("Turn right");
      updateBridgeConfiguration(TURN_RIGHT);
    }
    else if (distanceLeft > distance) {
      
      Serial.print("Turn left");
      updateBridgeConfiguration(TURN_LEFT);
    }
    updateMotorSpeed(maximalSpeed);
    delay(500);
    stopMotors();
  }
  else {
    DoGo(MARCHE_AVANT);
  }
}

void DoGo(Direction direction) {
  Serial.println("Forward");
  updateBridgeConfiguration(direction);
  updateMotorSpeed(minimalSpeed);
}

void DoGoWithStepping(Direction direction) {
  updateBridgeConfiguration(direction); 
  for (int vitesse = minimalSpeed; vitesse < maximalSpeed; vitesse++) {
    updateMotorSpeed(vitesse);
    delay(delayUpdateSpeed);
  }
  for (int vitesse = maximalSpeed; vitesse > minimalSpeed; vitesse--) {
    updateMotorSpeed(vitesse);
    delay(delayUpdateSpeed);
  }
  updateMotorSpeed(0);

  delay(1000);
}

void updateBridgeConfiguration(Direction direction) {

  if (direction == MARCHE_ARRIERE) {
    digitalWrite(borneIN1, HIGH);
    digitalWrite(borneIN2, LOW);
    digitalWrite(borneIN3, HIGH);
    digitalWrite(borneIN4, LOW);
    return;
  }
  
  if (direction == MARCHE_AVANT) {
    digitalWrite(borneIN1, LOW);
    digitalWrite(borneIN2, HIGH);
    digitalWrite(borneIN3, LOW);
    digitalWrite(borneIN4, HIGH);
    return;
  }

  // turn on place
  if (direction == TURN_RIGHT) {
    digitalWrite(borneIN1, HIGH);
    digitalWrite(borneIN2, LOW);
    digitalWrite(borneIN3, LOW);
    digitalWrite(borneIN4, HIGH);
    return;
  }

  if (direction == TURN_LEFT) {
    digitalWrite(borneIN1, LOW);
    digitalWrite(borneIN2, HIGH);
    digitalWrite(borneIN3, HIGH);
    digitalWrite(borneIN4, LOW);
    return;
  }
}

void updateMotorSpeed(int speed) {
  analogWrite(borneENA, speed);
  analogWrite(borneENB, speed);
}

void stopMotors() {
  Serial.println("Stop motor");
  digitalWrite(borneIN1, LOW);
  digitalWrite(borneIN2, LOW);  
  digitalWrite(borneIN3, LOW);
  digitalWrite(borneIN4, LOW); 
}

void down(int from, int to, int step) {
  for (int i=from; i>=to; i+=step) {
    servo_motor.write(i);
    delay(30);
  }
}

void up(int from, int to, int step) {
  for (int i=from; i<=to; i+=step) {
    servo_motor.write(i);
    delay(30);
  }
}

int lookSide(Direction direction) {
  Serial.println("Look side");
  delay(200);
  if (direction == TURN_RIGHT) {
    down(90, 0, -5);
  } else if (direction == TURN_LEFT) {
    up(90, 180, 5);
  }
  delay(500);
  int distance = readPing();
  delay(100);
  servo_motor.write(90);
  return distance;
}

int readPing() {
  Serial.println("Read ping");
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
  }
  return cm;
}
