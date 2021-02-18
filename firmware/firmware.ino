// Including the needed library
#include <Servo.h>

// Defining pin constants
const int X_AXIS_STP_PIN = A0;
const int X_AXIS_DIR_PIN = A1;
const int X_AXIS_ENA_PIN = 38;
const int X_AXIS_MS1_PIN = 5;
const int X_AXIS_MS2_PIN = 6;
const int Y_AXIS_STP_PIN = A6;
const int Y_AXIS_DIR_PIN = A7;
const int Y_AXIS_ENA_PIN = A2;
const int Y_AXIS_MS1_PIN = A5;
const int Y_AXIS_MS2_PIN = A4;
const int Y_AXIS_INVERTED_STP_PIN = 46;
const int Y_AXIS_INVERTED_DIR_PIN = 48;
const int Y_AXIS_INVERTED_ENA_PIN = A8;
const int Y_AXIS_INVERTED_MS1_PIN = 22;
const int Y_AXIS_INVERTED_MS2_PIN = 39;
const int C_AXIS_STP_PIN = 26;
const int C_AXIS_DIR_PIN = 28;
const int C_AXIS_ENA_PIN = 24;
const int C_AXIS_MS1_PIN = A9;
const int C_AXIS_MS2_PIN = A10;
const int ENDSTOP_PIN = 2;
const int VACUUM_PUMP_PIN = 8;
const int SERVO_PIN = 11;

// Defining other constants
const int X_AXIS_MAX_POSITION = 500;
const int X_AXIS_MOVEMENT_PER_ROTATION = 10;
const int Y_AXIS_MAX_POSITION = 500;
const int Y_AXIS_MOVEMENT_PER_ROTATION = 229;
const int SERVO_ACTIVE_ANGLE = 90;
const int SERVO_DEACTIVE_ANGLE = 0;

// Defining variables
int motorSpeed = 100;
int xAxisPosition = 0;
int yAxisPosition = 0;
int cAxisPosition = 0;
int vacuumPumpDutyCycle = 0;

// Creating a new servo object
Servo servo;

// Defining an axis homing function
void homeAxis() {
  // Moving servo to the deactive position
  if (servo.read() != SERVO_DEACTIVE_ANGLE) {
    servo.write(SERVO_DEACTIVE_ANGLE);
  }
  // Homing the x axis
  while(digitalRead(ENDSTOP_PIN) == HIGH) {
    digitalWrite(X_AXIS_STP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(X_AXIS_STP_PIN, LOW);
    delayMicroseconds(340);
  }
}

// Defining a motor speed changing function
void setMotorSpeed(int newMotorSpeed = motorSpeed) {
  // Printing a busy message
  Serial.println('B');
  // Limiting the parameter range
  if (newMotorSpeed > 100) {
    newMotorSpeed = 100;
  } else if (newMotorSpeed < 1) {
    newMotorSpeed = 1;
  }
  // Updating the motor speed
  if (newMotorSpeed != motorSpeed) {
    motorSpeed = newMotorSpeed;
  }
  // Printing an availability message
  Serial.println('R');
}

// Defining a x axis motor controlling function
void moveXAxis(int newXAxisPosition = xAxisPosition) {
  // Printing a busy message
  Serial.println('B');
  // Limiting the parameter range
  if (newXAxisPosition > X_AXIS_MAX_POSITION) {
    newXAxisPosition = X_AXIS_MAX_POSITION;
  } else if (newXAxisPosition < 0) {
    newXAxisPosition = 0;
  }
  // Controlling the motor
  if (newXAxisPosition != xAxisPosition) {
    int mmToGo;
    int stepInterval = round((100.0 / motorSpeed) * 35) - 10;
    // Choosing a movement direction
    if (newXAxisPosition > xAxisPosition) {
      mmToGo = newXAxisPosition - xAxisPosition;
    } else if (newXAxisPosition < xAxisPosition) {
      mmToGo = xAxisPosition - newXAxisPosition;
      digitalWrite(X_AXIS_DIR_PIN, LOW);
    }
    // Sending the step signals
    for (unsigned long long stepsToGo = round(((float)mmToGo / X_AXIS_MOVEMENT_PER_ROTATION) * 3200); stepsToGo > 0; stepsToGo--) {
      digitalWrite(X_AXIS_STP_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(X_AXIS_STP_PIN, LOW);
      delayMicroseconds(stepInterval);
    }
    // Resetting the movement direction
    if(newXAxisPosition < xAxisPosition) {
      digitalWrite(X_AXIS_DIR_PIN, HIGH);
    }
    // Updating the known x position
    xAxisPosition = newXAxisPosition;
  }
  // Printing an Availability message
  Serial.println('R');
}

// Defining a y axis motor controlling function
void moveYAxis(int newYAxisPosition = yAxisPosition) {
  // Printing a busy message
  Serial.println('B');
  // Limiting the parameter range
  if (newYAxisPosition > Y_AXIS_MAX_POSITION) {
    newYAxisPosition = Y_AXIS_MAX_POSITION;
  } else if (newYAxisPosition < 0) {
    newYAxisPosition = 0;
  }
  // Controlling the motor
  if (newYAxisPosition != yAxisPosition) {
    int mmToGo;
    int stepInterval = round((100.0 / motorSpeed) * 35) - 10;
    // Choosing a movement direction
    if (newYAxisPosition > yAxisPosition) {
      mmToGo = newYAxisPosition - yAxisPosition;
    } else if (newYAxisPosition < yAxisPosition) {
      mmToGo = yAxisPosition - newYAxisPosition;
      digitalWrite(Y_AXIS_DIR_PIN, LOW);
      digitalWrite(Y_AXIS_INVERTED_DIR_PIN, HIGH);
    }
    // Sending the step signals
    for (unsigned long long stepsToGo = round(((float)mmToGo / Y_AXIS_MOVEMENT_PER_ROTATION) * 3200); stepsToGo > 0; stepsToGo--) {
      digitalWrite(Y_AXIS_STP_PIN, HIGH);
      digitalWrite(Y_AXIS_INVERTED_STP_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(Y_AXIS_STP_PIN, LOW);
      digitalWrite(Y_AXIS_INVERTED_STP_PIN, LOW);
      delayMicroseconds(stepInterval);
    }
    // Resetting the movement direction
    if(newYAxisPosition < yAxisPosition) {
      digitalWrite(Y_AXIS_DIR_PIN, HIGH);
      digitalWrite(Y_AXIS_INVERTED_DIR_PIN, LOW);
    }
    // Updating the known y position
    yAxisPosition = newYAxisPosition;
  }
  // Printing an availability message
  Serial.println('R');
}

// Defining a c axis motor controlling function
void moveCAxis(int newCAxisPosition = cAxisPosition) {
  // Printing a busy message
  Serial.println('B');
  // Limiting the parameter range
  if (newCAxisPosition > 359) {
    newCAxisPosition = 359;
  } else if (newCAxisPosition < 0) {
    newCAxisPosition = 0;
  }
  // Controlling the motor
  if (newCAxisPosition != cAxisPosition) {
    int angleToGo;
    int stepInterval = round((100.0 / motorSpeed) * 35) - 10;
    // Changing the new position sometimes to avoid cable damage
    if (newCAxisPosition > 180) {
      newCAxisPosition -= 360;
    }
    if (cAxisPosition > 180) {
      cAxisPosition -= 360;
    }
    // Choosing an movement direction
    if (newCAxisPosition > cAxisPosition) {
      angleToGo = newCAxisPosition - cAxisPosition;
    } else if (newCAxisPosition < cAxisPosition) {
      angleToGo = cAxisPosition - newCAxisPosition;
      digitalWrite(C_AXIS_DIR_PIN, LOW);
    }
    // Sending the step signals
    for (unsigned long long stepsToGo = round(((float)angleToGo / 360) * 3200); stepsToGo > 0; stepsToGo--) {
      digitalWrite(C_AXIS_STP_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(C_AXIS_STP_PIN, LOW);
      delayMicroseconds(stepInterval);
    }
    // Resetting the movement direction
    if(newCAxisPosition < cAxisPosition) {
      digitalWrite(C_AXIS_DIR_PIN, HIGH);
    }
    // Recreating the original new position after changing it to avoid cable damage
    if (newCAxisPosition < 0) {
      newCAxisPosition += 360;
    }
    // Updating the known c position
    cAxisPosition = newCAxisPosition;
  }
  // Printing an availability message
  Serial.println('R');
}

// Defining a vacuum pump and servo controlling function
void changeVacuum(int newVacuumPumpDutyCycle = vacuumPumpDutyCycle) {
  // Printing a busy message
  Serial.println('B');
  // Limiting the parameter range
  if (newVacuumPumpDutyCycle > 255) {
    newVacuumPumpDutyCycle = 255;
  } else if (newVacuumPumpDutyCycle < 0) {
    newVacuumPumpDutyCycle = 0;
  }
  // Controlling the vacuum pump and servo
  if (newVacuumPumpDutyCycle != vacuumPumpDutyCycle) {
    analogWrite(VACUUM_PUMP_PIN, newVacuumPumpDutyCycle);
    // Moving the servo arm accordingly to the pump state
    if (newVacuumPumpDutyCycle == 0 && servo.read() != SERVO_DEACTIVE_ANGLE) {
      servo.write(SERVO_DEACTIVE_ANGLE);
    } else if (newVacuumPumpDutyCycle > 0 && servo.read() != SERVO_ACTIVE_ANGLE) {
      servo.write(SERVO_ACTIVE_ANGLE);
    }
    // Giving the servo so time to move
    delay(1000);
    // Updating the known vacuum pump duty cycle
    vacuumPumpDutyCycle = newVacuumPumpDutyCycle;
  }
  // Printing an availability message
  Serial.println('R');
}

void setup() {
  // Defining pins and setting them to their default state
  pinMode(X_AXIS_STP_PIN, OUTPUT);
  digitalWrite(X_AXIS_STP_PIN, LOW);
  pinMode(X_AXIS_DIR_PIN, OUTPUT);
  digitalWrite(X_AXIS_DIR_PIN, HIGH);
  pinMode(X_AXIS_ENA_PIN, OUTPUT);
  digitalWrite(X_AXIS_ENA_PIN, LOW);
  pinMode(X_AXIS_MS1_PIN, OUTPUT);
  digitalWrite(X_AXIS_MS1_PIN, HIGH);
  pinMode(X_AXIS_MS2_PIN, OUTPUT);
  digitalWrite(X_AXIS_MS2_PIN, HIGH);
  pinMode(Y_AXIS_STP_PIN, OUTPUT);
  digitalWrite(Y_AXIS_STP_PIN, LOW);
  pinMode(Y_AXIS_DIR_PIN, OUTPUT);
  digitalWrite(Y_AXIS_DIR_PIN, HIGH);
  pinMode(Y_AXIS_ENA_PIN, OUTPUT);
  digitalWrite(Y_AXIS_ENA_PIN, LOW);
  pinMode(Y_AXIS_MS1_PIN, OUTPUT);
  digitalWrite(Y_AXIS_MS1_PIN, HIGH);
  pinMode(Y_AXIS_MS2_PIN, OUTPUT);
  digitalWrite(Y_AXIS_MS2_PIN, HIGH);
  pinMode(Y_AXIS_INVERTED_STP_PIN, OUTPUT);
  digitalWrite(Y_AXIS_INVERTED_STP_PIN, LOW);
  pinMode(Y_AXIS_INVERTED_DIR_PIN, OUTPUT);
  digitalWrite(Y_AXIS_INVERTED_DIR_PIN, LOW);
  pinMode(Y_AXIS_INVERTED_ENA_PIN, OUTPUT);
  digitalWrite(Y_AXIS_INVERTED_ENA_PIN, LOW);
  pinMode(Y_AXIS_INVERTED_MS1_PIN, OUTPUT);
  digitalWrite(Y_AXIS_INVERTED_MS1_PIN, HIGH);
  pinMode(Y_AXIS_INVERTED_MS2_PIN, OUTPUT);
  digitalWrite(Y_AXIS_INVERTED_MS2_PIN, HIGH);
  pinMode(C_AXIS_STP_PIN, OUTPUT);
  digitalWrite(C_AXIS_STP_PIN, LOW);
  pinMode(C_AXIS_DIR_PIN, OUTPUT);
  digitalWrite(C_AXIS_DIR_PIN, HIGH);
  pinMode(C_AXIS_ENA_PIN, OUTPUT);
  digitalWrite(C_AXIS_ENA_PIN, LOW);
  pinMode(C_AXIS_MS1_PIN, OUTPUT);
  digitalWrite(C_AXIS_MS1_PIN, HIGH);
  pinMode(C_AXIS_MS2_PIN, OUTPUT);
  digitalWrite(C_AXIS_MS2_PIN, HIGH);
  pinMode(ENDSTOP_PIN, INPUT);
  pinMode(VACUUM_PUMP_PIN, OUTPUT);
  digitalWrite(VACUUM_PUMP_PIN, LOW);
  servo.attach(SERVO_PIN);
  // Homing
  homeAxis();
  // Opening the serial connection
  Serial.begin(115200);
  // Showing availability
  Serial.println('R');
}

void loop() {
  // Checking for available serial data
  if (Serial.available() > 0) {
    // Reading in commands with their according values
    char command = (char)Serial.read();
    int value = Serial.parseInt();
    Serial.println(command);
    Serial.println(value);
    // Executing known commands or printing an error
    switch (command) {
      case 'S':
        setMotorSpeed(value);
        break;
      case 'X':
        moveXAxis(value);
        break;
      case 'Y':
        moveYAxis(value);
        break;
      case 'C':
        moveCAxis(value);
        break;
      case 'V':
        changeVacuum(value);
        break;
      default:
        Serial.println('E');
        break;
    }
  }
}
