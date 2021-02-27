// Including the needed library
#include <Servo.h>

// Defining a pin constant
const int SERVO_PIN = 11;

// Creating a new servo object
Servo servo;

void setup() {
  // Initiating servo
  servo.attach(SERVO_PIN);
  // Opening the serial connection
  Serial.begin(115200);
  // Printing an availability message
  Serial.println('R');
}

void loop() {
  if (Serial.available() > 0) {
    Serial.println('B');
    int serialValue = Serial.parseInt();
    if (serialValue > 180) {
      serialValue = 180;
    } else if (serialValue < 0) {
      serialValue = 0;
    }
    servo.write(serialValue);
    delay(500);
    Serial.println('R');
  }
}
