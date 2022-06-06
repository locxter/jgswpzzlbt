#include <Servo.h>

// Pins
const int X_AXIS_STP_PIN = A0;
const int X_AXIS_DIR_PIN = A1;
const int Y_AXIS_0_STP_PIN = A6;
const int Y_AXIS_0_DIR_PIN = A7;
const int Y_AXIS_1_STP_PIN = 46;
const int Y_AXIS_1_DIR_PIN = 48;
const int C_AXIS_STP_PIN = 26;
const int C_AXIS_DIR_PIN = 28;
const int X_AXIS_ENDSTOP_PIN = 3;
const int Y_AXIS_ENDSTOP_PIN = 14;
const int VACUUM_PUMP_PIN = 8;
const int VACUUM_VALVE_PIN = 10;

// Serial messages
const char AVAILABILITY_MESSAGE = 'R';
const char ERROR_MESSAGE = 'E';

// Servo object
Servo servo;

// Minimum x, y and c axis step intervals
const int X_AXIS_STEP_INTERVAL = 150;
const int Y_AXIS_STEP_INTERVAL = 750;
const int C_AXIS_STEP_INTERVAL = 200;

// Maximum x, y and z axis coordinates
const int X_AXIS_MAX_COORDINATE = 825;
const int Y_AXIS_MAX_COORDINATE = 725;
const int Z_AXIS_MAX_COORDINATE = 90;

// X and y axis movement per rotation
const int X_AXIS_MOVEMENT_PER_ROTATION = 40;
const int Y_AXIS_MOVEMENT_PER_ROTATION = 220;

// Own functions
void homeAxis();
void moveXAxis(int newXAxisCoordinate);
void moveYAxis(int newYAxisCoordinate);
void moveZAxis(int newZAxisCoordinate);
void moveCAxis(int newCAxisCoordinate);
void controlVacuumSystem(bool isOn);

// Setup function
void setup() {
    // More pins
    const int X_AXIS_ENA_PIN = 38;
    const int X_AXIS_MS1_PIN = 5;
    const int X_AXIS_MS2_PIN = 6;
    const int Y_AXIS_0_ENA_PIN = A2;
    const int Y_AXIS_0_MS1_PIN = A5;
    const int Y_AXIS_0_MS2_PIN = A4;
    const int Y_AXIS_1_ENA_PIN = A8;
    const int Y_AXIS_1_MS1_PIN = 22;
    const int Y_AXIS_1_MS2_PIN = 39;
    const int C_AXIS_ENA_PIN = 24;
    const int C_AXIS_MS1_PIN = A9;
    const int C_AXIS_MS2_PIN = A10;
    const int SERVO_PIN = 11;
    // Initiate pins and set them to their default states
    pinMode(X_AXIS_STP_PIN, OUTPUT);
    digitalWrite(X_AXIS_STP_PIN, LOW);
    pinMode(X_AXIS_DIR_PIN, OUTPUT);
    digitalWrite(X_AXIS_DIR_PIN, LOW);
    pinMode(X_AXIS_ENA_PIN, OUTPUT);
    digitalWrite(X_AXIS_ENA_PIN, LOW);
    pinMode(X_AXIS_MS1_PIN, OUTPUT);
    digitalWrite(X_AXIS_MS1_PIN, HIGH);
    pinMode(X_AXIS_MS2_PIN, OUTPUT);
    digitalWrite(X_AXIS_MS2_PIN, HIGH);
    pinMode(Y_AXIS_0_STP_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_STP_PIN, LOW);
    pinMode(Y_AXIS_0_DIR_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_DIR_PIN, HIGH);
    pinMode(Y_AXIS_0_ENA_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_ENA_PIN, LOW);
    pinMode(Y_AXIS_0_MS1_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_MS1_PIN, HIGH);
    pinMode(Y_AXIS_0_MS2_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_MS2_PIN, HIGH);
    pinMode(Y_AXIS_1_STP_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_STP_PIN, LOW);
    pinMode(Y_AXIS_1_DIR_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_DIR_PIN, LOW);
    pinMode(Y_AXIS_1_ENA_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_ENA_PIN, LOW);
    pinMode(Y_AXIS_1_MS1_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_MS1_PIN, HIGH);
    pinMode(Y_AXIS_1_MS2_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_MS2_PIN, HIGH);
    pinMode(C_AXIS_STP_PIN, OUTPUT);
    digitalWrite(C_AXIS_STP_PIN, LOW);
    pinMode(C_AXIS_DIR_PIN, OUTPUT);
    digitalWrite(C_AXIS_DIR_PIN, LOW);
    pinMode(C_AXIS_ENA_PIN, OUTPUT);
    digitalWrite(C_AXIS_ENA_PIN, LOW);
    pinMode(C_AXIS_MS1_PIN, OUTPUT);
    digitalWrite(C_AXIS_MS1_PIN, HIGH);
    pinMode(C_AXIS_MS2_PIN, OUTPUT);
    digitalWrite(C_AXIS_MS2_PIN, HIGH);
    pinMode(X_AXIS_ENDSTOP_PIN, INPUT);
    pinMode(Y_AXIS_ENDSTOP_PIN, INPUT);
    pinMode(VACUUM_PUMP_PIN, OUTPUT);
    digitalWrite(VACUUM_PUMP_PIN, LOW);
    pinMode(VACUUM_VALVE_PIN, OUTPUT);
    digitalWrite(VACUUM_VALVE_PIN, LOW);
    servo.attach(SERVO_PIN);
    // Open the serial connection
    Serial.begin(115200);
    // Home robot
    homeAxis();
}

// Main function
void loop() {
    // Check for new serial data
    if (Serial.available() > 0) {
        // More serial messages
        const char X_AXIS_COMMAND = 'X';
        const char Y_AXIS_COMMAND = 'Y';
        const char Z_AXIS_COMMAND = 'Z';
        const char C_AXIS_COMMAND = 'C';
        const char VACUUM_SYSTEM_COMMAND = 'V';
        // Reade commands and their according parameters
        String serialInput = Serial.readStringUntil('\n');
        char command = serialInput.charAt(0);
        int commandParameter = serialInput.substring(1).toInt();
        // Execute known commands or print an error
        switch (command) {
            case X_AXIS_COMMAND:
                moveXAxis(commandParameter);
                break;
            case Y_AXIS_COMMAND:
                moveYAxis(commandParameter);
                break;
            case Z_AXIS_COMMAND:
                moveZAxis(commandParameter);
                break;
            case C_AXIS_COMMAND:
                moveCAxis(commandParameter);
                break;
            case VACUUM_SYSTEM_COMMAND:
                if (commandParameter < 1) {
                    controlVacuumSystem(false);
                } else {
                    controlVacuumSystem(true);
                }
                break;
            default:
                Serial.print((String) ERROR_MESSAGE + '\n');
                break;
        }
    }
}

// Function for homing the robot
void homeAxis() {
    // Variables for storing time before homing the x or y axis and success of that operation
    unsigned long startAxisHoming;
    bool success = true;
    // Move servo to a deactive coordinate
    if (servo.read() != Z_AXIS_MAX_COORDINATE) {
        servo.write(Z_AXIS_MAX_COORDINATE);
    }
    // Home the x axis
    startAxisHoming = millis();
    digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
    while (digitalRead(X_AXIS_ENDSTOP_PIN) == HIGH) {
        digitalWrite(X_AXIS_STP_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(X_AXIS_STP_PIN, LOW);
        delayMicroseconds((2 * X_AXIS_STEP_INTERVAL) - 10);
        if (millis() - startAxisHoming > 60000) {
            success = false;
            break;
        }
    }
    digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
    // Home the y axis
    startAxisHoming = millis();
    digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
    digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
    while (digitalRead(Y_AXIS_ENDSTOP_PIN) == HIGH) {
        digitalWrite(Y_AXIS_0_STP_PIN, HIGH);
        digitalWrite(Y_AXIS_1_STP_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(Y_AXIS_0_STP_PIN, LOW);
        digitalWrite(Y_AXIS_1_STP_PIN, LOW);
        delayMicroseconds((2 * Y_AXIS_STEP_INTERVAL) - 10);
        if (millis() - startAxisHoming > 60000) {
            success = false;
            break;
        }
    }
    digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
    digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
    // Check for homing error and print an according message
    if (success) {
        Serial.print((String) AVAILABILITY_MESSAGE + '\n');
    } else {
        Serial.print((String) ERROR_MESSAGE + '\n');
    }
}

// Function for controlling the x axis motor 
void moveXAxis(int newXAxisCoordinate) {
    // Variable to store current coordinate
    static int xAxisCoordinate = 0;
    // Limiting the parameter range
    if (newXAxisCoordinate > X_AXIS_MAX_COORDINATE) {
        newXAxisCoordinate = X_AXIS_MAX_COORDINATE;
    } else if (newXAxisCoordinate < 0) {
        newXAxisCoordinate = 0;
    }
    // Controlling the motor
    if (newXAxisCoordinate != xAxisCoordinate) {
        int mmToGo;
        unsigned long long stepsToGo;
        // Calculating step interval related variables
        const long MAX_STEP_INTERVAL = (long) 100 * X_AXIS_STEP_INTERVAL;
        const long MIN_STEP_INTERVAL = X_AXIS_STEP_INTERVAL;
        const float STEP_INTERVAL_CHANGE = ((float) MAX_STEP_INTERVAL - MIN_STEP_INTERVAL) / 200;
        // Choosing a movement direction
        if (newXAxisCoordinate > xAxisCoordinate) {
            mmToGo = newXAxisCoordinate - xAxisCoordinate;
        } else if (newXAxisCoordinate < xAxisCoordinate) {
            mmToGo = xAxisCoordinate - newXAxisCoordinate;
            digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
        }
        stepsToGo = round(((float) mmToGo / X_AXIS_MOVEMENT_PER_ROTATION) * 3200);
        // Sending the step signals
        for (unsigned long long i = 0; i < stepsToGo; i++) {
            long stepInterval = MIN_STEP_INTERVAL - 10;
            // Slowing down at the start and end of the move
            if (i < 201) {
                stepInterval = round(MAX_STEP_INTERVAL - (i * STEP_INTERVAL_CHANGE)) - 10;
            } else if (i > (stepsToGo - 201)) {
                stepInterval = round(MIN_STEP_INTERVAL + ((i - (stepsToGo - 200)) * STEP_INTERVAL_CHANGE)) - 10;
            }
            digitalWrite(X_AXIS_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(X_AXIS_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if (newXAxisCoordinate < xAxisCoordinate) {
            digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
        }
        // Updating the known x coordinate
        xAxisCoordinate = newXAxisCoordinate;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Function for controlling the y axis motor 
void moveYAxis(int newYAxisCoordinate) {
    // Defining variable to store current coordinate
    static int yAxisCoordinate = 0;
    // Limiting the parameter range
    if (newYAxisCoordinate > Y_AXIS_MAX_COORDINATE) {
        newYAxisCoordinate = Y_AXIS_MAX_COORDINATE;
    } else if (newYAxisCoordinate < 0) {
        newYAxisCoordinate = 0;
    }
    // Controlling the motor
    if (newYAxisCoordinate != yAxisCoordinate) {
        int mmToGo;
        unsigned long long stepsToGo;
        // Choosing a movement direction
        if (newYAxisCoordinate > yAxisCoordinate) {
            mmToGo = newYAxisCoordinate - yAxisCoordinate;
        } else if (newYAxisCoordinate < yAxisCoordinate) {
            mmToGo = yAxisCoordinate - newYAxisCoordinate;
            digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
            digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
        }
        stepsToGo = round(((float) mmToGo / Y_AXIS_MOVEMENT_PER_ROTATION) * 3200);
        // Sending the step signals
        for (unsigned long long i = 0; i < stepsToGo; i++) {
            long stepInterval = Y_AXIS_STEP_INTERVAL - 10;
            // Slowing down at the start and end of the move
            if (i < 201 || i >(stepsToGo - 201)) {
                stepInterval = (2 * Y_AXIS_STEP_INTERVAL) - 10;
            }
            digitalWrite(Y_AXIS_0_STP_PIN, HIGH);
            digitalWrite(Y_AXIS_1_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(Y_AXIS_0_STP_PIN, LOW);
            digitalWrite(Y_AXIS_1_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if (newYAxisCoordinate < yAxisCoordinate) {
            digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
            digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
        }
        // Updating the known y coordinate
        yAxisCoordinate = newYAxisCoordinate;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Function for controlling the x axis servo 
void moveZAxis(int newZAxisCoordinate) {
    // Limiting the parameter range
    if (newZAxisCoordinate > Z_AXIS_MAX_COORDINATE) {
        newZAxisCoordinate = Z_AXIS_MAX_COORDINATE;
    } else if (newZAxisCoordinate < 0) {
        newZAxisCoordinate = 0;
    }
    // Controlling the servo
    if (newZAxisCoordinate != servo.read()) {
        servo.write(newZAxisCoordinate);
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Function for controlling the c axis motor 
void moveCAxis(int angle) {
    unsigned long long stepsToGo = round((abs(angle) / 90.0) * 3200);
    // Calculating step interval related variables
    const long MAX_STEP_INTERVAL = 100 * C_AXIS_STEP_INTERVAL;
    const long MIN_STEP_INTERVAL = C_AXIS_STEP_INTERVAL;
    const float STEP_INTERVAL_CHANGE = ((float) MAX_STEP_INTERVAL - MIN_STEP_INTERVAL) / 200;
    // Choosing a movement direction
    if (angle < 0) {
        digitalWrite(C_AXIS_DIR_PIN, !digitalRead(C_AXIS_DIR_PIN));
    }
    // Sending the step signals
    for (unsigned long long i = 0; i < stepsToGo; i++) {
        long stepInterval = MIN_STEP_INTERVAL - 10;
        // Slowing down at the start and end of the move
        if (i < 201) {
            stepInterval = round(MAX_STEP_INTERVAL - (i * STEP_INTERVAL_CHANGE)) - 10;
        } else if (i > (stepsToGo - 201)) {
            stepInterval = round(MIN_STEP_INTERVAL + ((i - (stepsToGo - 200)) * STEP_INTERVAL_CHANGE)) - 10;
        }
        digitalWrite(C_AXIS_STP_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(C_AXIS_STP_PIN, LOW);
        delayMicroseconds(stepInterval);
    }
    // Resetting the movement direction
    if (angle < 0) {
        digitalWrite(C_AXIS_DIR_PIN, !digitalRead(C_AXIS_DIR_PIN));
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Function for controlling the vacuum system
void controlVacuumSystem(bool isOn) {
    // Controlling the vacuum system
    if (isOn) {
        digitalWrite(VACUUM_VALVE_PIN, LOW);
        digitalWrite(VACUUM_PUMP_PIN, HIGH);
    } else {
        digitalWrite(VACUUM_VALVE_PIN, HIGH);
        digitalWrite(VACUUM_PUMP_PIN, LOW);
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}
