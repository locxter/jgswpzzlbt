// Including the needed library
#include <Servo.h>

// Defining global pins
const int X_AXIS_STP_PIN = A0;
const int X_AXIS_DIR_PIN = A1;
const int Y_AXIS_0_STP_PIN = A6;
const int Y_AXIS_0_DIR_PIN = A7;
const int Y_AXIS_1_STP_PIN = 46;
const int Y_AXIS_1_DIR_PIN = 48;
const int C_AXIS_STP_PIN = 26;
const int C_AXIS_DIR_PIN = 28;
const int ENDSTOP_PIN = 2;
const int VACUUM_PUMP_PIN = 8;
const int LED_PIN = 9;

// Defining global constants
const int X_AXIS_MIN_STEP_INTERVAL = 100;
const int Y_AXIS_MIN_STEP_INTERVAL = 300;
const int C_AXIS_MIN_STEP_INTERVAL = 200;
const char AVAILABILITY_MESSAGE = 'R';
const char MOTOR_SPEED_COMMAND = 'S';
const char X_AXIS_COMMAND = 'X';
const char Y_AXIS_COMMAND = 'Y';
const char Z_AXIS_COMMAND = 'Z';
const char C_AXIS_COMMAND = 'C';
const char VACUUM_PUMP_COMMAND = 'V';
const char LED_COMMAND = 'L';

// Defining global variable
int motorSpeed = 100;

// Creating a new servo object
Servo servo;

// Initiating own functions
void homeAxis();
void setMotorSpeed(int newMotorSpeed);
void moveXAxis(int newXAxisPosition);
void moveYAxis(int newYAxisPosition);
void moveZAxis(int newZAxisPosition);
void moveCAxis(int newCAxisPosition);
void controlVacuumPump(int newVacuumPumpDutyCycle);
void controlLed(int newLedDutyCycle);

void setup()
{
    // Defining pin constants
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
    // Defining default motor direction
    const int X_AXIS_DIR_PIN_DEFAULT_STATE = LOW;
    const int Y_AXIS_DIR_PIN_DEFAULT_STATE = HIGH;
    const int C_AXIS_DIR_PIN_DEFAULT_STATE = HIGH;
    // Initiating pins and setting them to their default state
    pinMode(X_AXIS_STP_PIN, OUTPUT);
    digitalWrite(X_AXIS_STP_PIN, LOW);
    pinMode(X_AXIS_DIR_PIN, OUTPUT);
    digitalWrite(X_AXIS_DIR_PIN, X_AXIS_DIR_PIN_DEFAULT_STATE);
    pinMode(X_AXIS_ENA_PIN, OUTPUT);
    digitalWrite(X_AXIS_ENA_PIN, LOW);
    pinMode(X_AXIS_MS1_PIN, OUTPUT);
    digitalWrite(X_AXIS_MS1_PIN, HIGH);
    pinMode(X_AXIS_MS2_PIN, OUTPUT);
    digitalWrite(X_AXIS_MS2_PIN, HIGH);
    pinMode(Y_AXIS_0_STP_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_STP_PIN, LOW);
    pinMode(Y_AXIS_0_DIR_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_DIR_PIN, Y_AXIS_DIR_PIN_DEFAULT_STATE);
    pinMode(Y_AXIS_0_ENA_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_ENA_PIN, LOW);
    pinMode(Y_AXIS_0_MS1_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_MS1_PIN, HIGH);
    pinMode(Y_AXIS_0_MS2_PIN, OUTPUT);
    digitalWrite(Y_AXIS_0_MS2_PIN, HIGH);
    pinMode(Y_AXIS_1_STP_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_STP_PIN, LOW);
    pinMode(Y_AXIS_1_DIR_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_DIR_PIN, !Y_AXIS_DIR_PIN_DEFAULT_STATE);
    pinMode(Y_AXIS_1_ENA_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_ENA_PIN, LOW);
    pinMode(Y_AXIS_1_MS1_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_MS1_PIN, HIGH);
    pinMode(Y_AXIS_1_MS2_PIN, OUTPUT);
    digitalWrite(Y_AXIS_1_MS2_PIN, HIGH);
    pinMode(C_AXIS_STP_PIN, OUTPUT);
    digitalWrite(C_AXIS_STP_PIN, LOW);
    pinMode(C_AXIS_DIR_PIN, OUTPUT);
    digitalWrite(C_AXIS_DIR_PIN, C_AXIS_DIR_PIN_DEFAULT_STATE);
    pinMode(C_AXIS_ENA_PIN, OUTPUT);
    digitalWrite(C_AXIS_ENA_PIN, LOW);
    pinMode(C_AXIS_MS1_PIN, OUTPUT);
    digitalWrite(C_AXIS_MS1_PIN, HIGH);
    pinMode(C_AXIS_MS2_PIN, OUTPUT);
    digitalWrite(C_AXIS_MS2_PIN, HIGH);
    pinMode(ENDSTOP_PIN, INPUT);
    pinMode(VACUUM_PUMP_PIN, OUTPUT);
    digitalWrite(VACUUM_PUMP_PIN, LOW);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    servo.attach(SERVO_PIN);
    // Homing
    homeAxis();
    // Opening the serial connection
    Serial.begin(115200);
    // Printing an availability message
    Serial.println(AVAILABILITY_MESSAGE);
}

void loop()
{
    // Checking for new serial data
    if (Serial.available() > 0)
    {
        // Defining error message
        const char ERROR_MESSAGE = 'E';
        // Reading commands and their according values
        char serialCommand = (char)Serial.read();
        int serialCommandValue = Serial.parseInt();
        // Executing known commands or printing an error
        switch (serialCommand)
        {
            case MOTOR_SPEED_COMMAND:
                setMotorSpeed(serialCommandValue);
                break;
            case X_AXIS_COMMAND:
                moveXAxis(serialCommandValue);
                break;
            case Y_AXIS_COMMAND:
                moveYAxis(serialCommandValue);
                break;
            case Z_AXIS_COMMAND:
                moveZAxis(serialCommandValue);
                break;
            case C_AXIS_COMMAND:
                moveCAxis(serialCommandValue);
                break;
            case VACUUM_PUMP_COMMAND:
                controlVacuumPump(serialCommandValue);
                break;
            case LED_COMMAND:
                controlLed(serialCommandValue);
                break;
            default:
                Serial.println(ERROR_MESSAGE);
                break;
        }
        // Printing an availability message
        Serial.println(AVAILABILITY_MESSAGE);
    }
}

// Defining a homing function
void homeAxis()
{
    static int stepInterval = round(2 * X_AXIS_MIN_STEP_INTERVAL) - 10;
    // Moving servo to a deactive position
    if (servo.read() != 180)
    {
        servo.write(180);
    }
    // Homing the x axis
    digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
    while(digitalRead(ENDSTOP_PIN) == HIGH)
    {
        digitalWrite(X_AXIS_STP_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(X_AXIS_STP_PIN, LOW);
        delayMicroseconds(stepInterval);
    }
    digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
}

// Defining a motor speed changing function
void setMotorSpeed(int newMotorSpeed)
{
    // Printing a debug message
    Serial.print(MOTOR_SPEED_COMMAND);
    Serial.println(newMotorSpeed);
    // Limiting the parameter range
    if (newMotorSpeed > 100)
    {
        newMotorSpeed = 100;
    }
    else if (newMotorSpeed < 1)
    {
        newMotorSpeed = 1;
    }
    // Updating the motor speed
    if (newMotorSpeed != motorSpeed)
    {
        motorSpeed = newMotorSpeed;
    }
}

// Defining a x axis motor controlling function
void moveXAxis(int newXAxisPosition)
{
    // Printing a debug message
    Serial.print(X_AXIS_COMMAND);
    Serial.println(newXAxisPosition);
    // Defining max x axis position
    const int X_AXIS_MAX_POSITION = 750;
    // Defining variable to store current position
    static int xAxisPosition = 0;
    // Limiting the parameter range
    if (newXAxisPosition > X_AXIS_MAX_POSITION)
    {
        newXAxisPosition = X_AXIS_MAX_POSITION;
    }
    else if (newXAxisPosition < 0)
    {
        newXAxisPosition = 0;
    }
    // Controlling the motor
    if (newXAxisPosition != xAxisPosition)
    {
        const int X_AXIS_MOVEMENT_PER_ROTATION = 40;
        int mmToGo;
        int stepInterval = round((100.0 / motorSpeed) * X_AXIS_MIN_STEP_INTERVAL) - 10;
        // Choosing a movement direction
        if (newXAxisPosition > xAxisPosition)
        {
            mmToGo = newXAxisPosition - xAxisPosition;
        }
        else if (newXAxisPosition < xAxisPosition)
        {
            mmToGo = xAxisPosition - newXAxisPosition;
            digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
        }
        // Sending the step signals
        for (unsigned long long stepsToGo = round(((float)mmToGo / X_AXIS_MOVEMENT_PER_ROTATION) * 3200); stepsToGo > 0; stepsToGo--)
        {
            digitalWrite(X_AXIS_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(X_AXIS_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if(newXAxisPosition < xAxisPosition)
        {
            digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
        }
        // Updating the known x position
        xAxisPosition = newXAxisPosition;
    }
}

// Defining a y axis motor controlling function
void moveYAxis(int newYAxisPosition)
{
    // Printing a debug message
    Serial.print(Y_AXIS_COMMAND);
    Serial.println(newYAxisPosition);
    // Defining max y position
    const int Y_AXIS_MAX_POSITION = 750;
    // Defining variable to store current position
    static int yAxisPosition = 0;
    // Limiting the parameter range
    if (newYAxisPosition > Y_AXIS_MAX_POSITION)
    {
        newYAxisPosition = Y_AXIS_MAX_POSITION;
    }
    else if (newYAxisPosition < 0)
    {
        newYAxisPosition = 0;
    }
    // Controlling the motor
    if (newYAxisPosition != yAxisPosition)
    {
        const int Y_AXIS_MOVEMENT_PER_ROTATION = 229;
        int mmToGo;
        int stepInterval = round((100.0 / motorSpeed) * Y_AXIS_MIN_STEP_INTERVAL) - 10;
        // Choosing a movement direction
        if (newYAxisPosition > yAxisPosition)
        {
            mmToGo = newYAxisPosition - yAxisPosition;
        }
        else if (newYAxisPosition < yAxisPosition)
        {
            mmToGo = yAxisPosition - newYAxisPosition;
            digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
            digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
        }
        // Sending the step signals
        for (unsigned long long stepsToGo = round(((float)mmToGo / Y_AXIS_MOVEMENT_PER_ROTATION) * 3200); stepsToGo > 0; stepsToGo--)
        {
            digitalWrite(Y_AXIS_0_STP_PIN, HIGH);
            digitalWrite(Y_AXIS_1_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(Y_AXIS_0_STP_PIN, LOW);
            digitalWrite(Y_AXIS_1_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if(newYAxisPosition < yAxisPosition)
        {
            digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
            digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
        }
        // Updating the known y position
        yAxisPosition = newYAxisPosition;
    }
}

// Defining a z axis servo controlling function
void moveZAxis(int newZAxisPosition)
{
    // Printing a debug message
    Serial.print(Z_AXIS_COMMAND);
    Serial.println(newZAxisPosition);
    // Defining max z axis position
    const int Z_AXIS_MAX_POSITION = 90;
    // Limiting the parameter range
    if (newZAxisPosition > Z_AXIS_MAX_POSITION)
    {
        newZAxisPosition = Z_AXIS_MAX_POSITION;
    }
    else if (newZAxisPosition < 0)
    {
        newZAxisPosition = 0;
    }
    // Adapting the new position to a sevro angle
    newZAxisPosition += 90;
    // Controlling the servo
    if (newZAxisPosition != servo.read())
    {
        servo.write(newZAxisPosition);
    }
}

// Defining a c axis motor controlling function
void moveCAxis(int newCAxisPosition)
{
    // Printing a debug message
    Serial.print(C_AXIS_COMMAND);
    Serial.println(newCAxisPosition);
    // Defining variable to store current position
    static int cAxisPosition = 0;
    // Limiting the parameter range
    if (newCAxisPosition > 359)
    {
        newCAxisPosition = 359;
    }
    else if (newCAxisPosition < 0)
    {
        newCAxisPosition = 0;
    }
    // Controlling the motor
    if (newCAxisPosition != cAxisPosition)
    {
        int angleToGo;
        int stepInterval = round((100.0 / motorSpeed) * C_AXIS_MIN_STEP_INTERVAL) - 10;
        // Changing the new position sometimes to avoid cable damage
        if (newCAxisPosition > 180)
        {
            newCAxisPosition -= 360;
        }
        if (cAxisPosition > 180)
        {
            cAxisPosition -= 360;
        }
        // Choosing a movement direction
        if (newCAxisPosition > cAxisPosition)
        {
            angleToGo = newCAxisPosition - cAxisPosition;
        }
        else if (newCAxisPosition < cAxisPosition)
        {
            angleToGo = cAxisPosition - newCAxisPosition;
            digitalWrite(C_AXIS_DIR_PIN, !digitalRead(C_AXIS_DIR_PIN));
        }
        // Sending the step signals
        for (unsigned long long stepsToGo = round(((float)angleToGo / 360) * 3200); stepsToGo > 0; stepsToGo--)
        {
            digitalWrite(C_AXIS_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(C_AXIS_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if(newCAxisPosition < cAxisPosition)
        {
            digitalWrite(C_AXIS_DIR_PIN, !digitalRead(C_AXIS_DIR_PIN));
        }
        // Recreating the original new position after changing it to avoid cable damage
        if (newCAxisPosition < 0)
        {
            newCAxisPosition += 360;
        }
        // Updating the known c position
        cAxisPosition = newCAxisPosition;
    }
}

// Defining a vacuum pump controlling function
void controlVacuumPump(int newVacuumPumpDutyCycle)
{
    // Printing a debug message
    Serial.print(VACUUM_PUMP_COMMAND);
    Serial.println(newVacuumPumpDutyCycle);
    // Defining variable to store current vacuum pump duty cycle
    static int vacuumPumpDutyCycle = 0;
    // Limiting the parameter range
    if (newVacuumPumpDutyCycle > 255)
    {
        newVacuumPumpDutyCycle = 255;
    }
    else if (newVacuumPumpDutyCycle < 0)
    {
        newVacuumPumpDutyCycle = 0;
    }
    // Controlling the vacuum pump
    if (newVacuumPumpDutyCycle != vacuumPumpDutyCycle)
    {
        analogWrite(VACUUM_PUMP_PIN, newVacuumPumpDutyCycle);
        // Updating the known vacuum pump duty cycle
        vacuumPumpDutyCycle = newVacuumPumpDutyCycle;
    }
}

// Defining a LED controlling function
void controlLed(int newLedDutyCycle)
{
    // Printing a debug message
    Serial.print(LED_COMMAND);
    Serial.println(newLedDutyCycle);
    // Defining variable to store current LED duty cycle
    static int ledDutyCycle = 0;
    // Limiting the parameter range
    if (newLedDutyCycle > 255)
    {
        newLedDutyCycle = 255;
    }
    else if (newLedDutyCycle < 0)
    {
        newLedDutyCycle = 0;
    }
    // Controlling the LED
    if (newLedDutyCycle != ledDutyCycle)
    {
        analogWrite(LED_PIN, newLedDutyCycle);
        // Updating the known LED duty cycle
        ledDutyCycle = newLedDutyCycle;
    }
}
