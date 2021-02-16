// Defining user changable constants
const bool ENDSTOP_ACTIVE_HIGH = false;
const bool ACTIVATE_MS1 = false;
const bool ACTIVATE_MS2 = false;
const int X_AXIS_STEPS_PER_REVOLUTION = 200;
const int X_AXIS_MM_PER_REVOLUTION = 223;
const int Y_AXIS_STEPS_PER_REVOLUTION = 200;
const int Y_AXIS_MM_PER_REVOLUTION = 100;
const int C_AXIS_STEPS_PER_REVOLUTION = 200;
const int SERVO_ACTIVE_POSITION = 90;
const int SERVO_DEACTIVE_POSITION = 0;

// Including the needed library
#include <Servo.h>

// Creating a new servo object
Servo servo;

void setup() {
  // Defining endstop pin
  pinMode(2, INPUT);
  // Defining microstepping pin MS1
  pinMode(5, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(A9, OUTPUT);
  if (ACTIVATE_MS1) {
    digitalWrite(5, HIGH);
    digitalWrite(A5, HIGH);
    digitalWrite(22, HIGH);
    digitalWrite(A9, HIGH);
  } else {
    digitalWrite(5, LOW);
    digitalWrite(A5, LOW);
    digitalWrite(22, LOW);
    digitalWrite(A9, LOW);
  }
  // Defining microstepping pin MS1
  pinMode(6, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(A10, OUTPUT);
  if (ACTIVATE_MS2) {
    digitalWrite(6, HIGH);
    digitalWrite(A4, HIGH);
    digitalWrite(39, HIGH);
    digitalWrite(A10, HIGH);
  } else {
    digitalWrite(6, LOW);
    digitalWrite(A4, LOW);
    digitalWrite(39, LOW);
    digitalWrite(A10, LOW);
  }
    
  // Opening the serial connection
  Serial.begin(9600);
  Serial.println("B");
}

void loop() {

}
