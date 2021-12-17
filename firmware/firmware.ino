// Including needed header
#include <Servo.h>

// Defining pins
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
const int LED_PIN = 9;

// Defining the minimum x and y axis step interval
const int X_AXIS_MIN_STEP_INTERVAL = 150;
const int Y_AXIS_MIN_STEP_INTERVAL = 600;
// Defining messages
const char AVAILABILITY_MESSAGE = 'R';
const char ERROR_MESSAGE = 'E';

// Defining motor speed
int motorSpeed = 100;

// Creating a new servo object
Servo servo;

// Declaring own functions
void homeAxis();
void setMotorSpeed(int newMotorSpeed);
void moveXAxis(int newXAxisCoordinate);
void moveYAxis(int newYAxisCoordinate);
void moveZAxis(int newZAxisCoordinate);
void moveCAxis(int newCAxisCoordinate);
void controlVacuumPump(int newVacuumPumpDutyCycle);
void controlLed(int newLedDutyCycle);

// Main setup function
void setup()
{
    // Defining pins
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
    pinMode(X_AXIS_ENDSTOP_PIN, INPUT);
    pinMode(Y_AXIS_ENDSTOP_PIN, INPUT);
    pinMode(VACUUM_PUMP_PIN, OUTPUT);
    digitalWrite(VACUUM_PUMP_PIN, LOW);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    servo.attach(SERVO_PIN);
    // Opening the serial connection
    Serial.begin(115200);
    // Homing
    homeAxis();
}

// Main function
void loop()
{
    // Checking for new serial data
    if (Serial.available() > 0)
    {
        // Defining commands
        const char MOTOR_SPEED_COMMAND = 'S';
        const char X_AXIS_COMMAND = 'X';
        const char Y_AXIS_COMMAND = 'Y';
        const char Z_AXIS_COMMAND = 'Z';
        const char C_AXIS_COMMAND = 'C';
        const char VACUUM_PUMP_COMMAND = 'V';
        const char LED_COMMAND = 'L';
        // Reading commands and their according parameters
        String serialInput = Serial.readStringUntil('\n');
        char command = serialInput.charAt(0);
        int commandParameter = serialInput.substring(1).toInt();
        // Executing known commands or printing an error
        switch (command)
        {
            case MOTOR_SPEED_COMMAND:
                setMotorSpeed(commandParameter);
                break;
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
            case VACUUM_PUMP_COMMAND:
                controlVacuumPump(commandParameter);
                break;
            case LED_COMMAND:
                controlLed(commandParameter);
                break;
            default:
                Serial.print((String) ERROR_MESSAGE + '\n');
                break;
        }
    }
}

// Defining a homing function
void homeAxis()
{
    // Calculating the step intervals
    static int xAxisStepInterval = round((100.0 / 75) * X_AXIS_MIN_STEP_INTERVAL) - 10;
    static int yAxisStepInterval = round((100.0 / 75) * Y_AXIS_MIN_STEP_INTERVAL) - 10;
    // Creating variables for storing time before homing the x axis and success of that operation
    unsigned long start;
    bool success = true;
    // Moving servo to a deactive coordinate
    if (servo.read() != 0)
    {
        servo.write(0);
    }
    // Homing the x axis
    start = millis();
    digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
    while(digitalRead(X_AXIS_ENDSTOP_PIN) == HIGH)
    {
        digitalWrite(X_AXIS_STP_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(X_AXIS_STP_PIN, LOW);
        delayMicroseconds(xAxisStepInterval);
        if (millis() - start > 30000)
        {
            success = false;
            break;
        }
    }
    digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
    // Homing the y axis
    start = millis();
    digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
    digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
    while(digitalRead(Y_AXIS_ENDSTOP_PIN) == HIGH)
    {
        digitalWrite(Y_AXIS_0_STP_PIN, HIGH);
        digitalWrite(Y_AXIS_1_STP_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(Y_AXIS_0_STP_PIN, LOW);
        digitalWrite(Y_AXIS_1_STP_PIN, LOW);
        delayMicroseconds(yAxisStepInterval);
        if (millis() - start > 30000)
        {
            success = false;
            break;
        }
    }
    digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
    digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
    // Checking for homing error and printing an according message
    if (success)
    {
        Serial.print((String) AVAILABILITY_MESSAGE + '\n');
    }
    else
    {
        Serial.print((String) ERROR_MESSAGE + '\n');
    }
}

// Defining a motor speed changing function
void setMotorSpeed(int newMotorSpeed)
{
    // Defining maximum motor speed
    const int MOTOR_SPEED_MAX = 100;
    // Limiting the parameter range
    if (newMotorSpeed > MOTOR_SPEED_MAX)
    {
        newMotorSpeed = MOTOR_SPEED_MAX;
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
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Defining a x axis motor controlling function
void moveXAxis(int newXAxisCoordinate)
{
    // Defining maximum x axis coordinate
    const int X_AXIS_COORDINATE_MAX = 750;
    // Defining variable to store current coordinate
    static int xAxisCoordinate = 0;
    // Limiting the parameter range
    if (newXAxisCoordinate > X_AXIS_COORDINATE_MAX)
    {
        newXAxisCoordinate = X_AXIS_COORDINATE_MAX;
    }
    else if (newXAxisCoordinate < 0)
    {
        newXAxisCoordinate = 0;
    }
    // Controlling the motor
    if (newXAxisCoordinate != xAxisCoordinate)
    {
        // Defining the x axis movement per rotation
        const int X_AXIS_MOVEMENT_PER_ROTATION = 40;
        int mmToGo;
        // Calculating the step inerval
        int stepInterval = round((100.0 / motorSpeed) * X_AXIS_MIN_STEP_INTERVAL) - 10;
        // Choosing a movement direction
        if (newXAxisCoordinate > xAxisCoordinate)
        {
            mmToGo = newXAxisCoordinate - xAxisCoordinate;
        }
        else if (newXAxisCoordinate < xAxisCoordinate)
        {
            mmToGo = xAxisCoordinate - newXAxisCoordinate;
            digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
        }
        // Sending the step signals
        for (unsigned long long i = round(((float) mmToGo / X_AXIS_MOVEMENT_PER_ROTATION) * 3200); i > 0; i--)
        {
            digitalWrite(X_AXIS_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(X_AXIS_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if(newXAxisCoordinate < xAxisCoordinate)
        {
            digitalWrite(X_AXIS_DIR_PIN, !digitalRead(X_AXIS_DIR_PIN));
        }
        // Updating the known x coordinate
        xAxisCoordinate = newXAxisCoordinate;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Defining a y axis motor controlling function
void moveYAxis(int newYAxisCoordinate)
{
    // Defining maximum y axis coordinate
    const int Y_AXIS_COORDINATE_MAX = 800;
    // Defining variable to store current coordinate
    static int yAxisCoordinate = 0;
    // Limiting the parameter range
    if (newYAxisCoordinate > Y_AXIS_COORDINATE_MAX)
    {
        newYAxisCoordinate = Y_AXIS_COORDINATE_MAX;
    }
    else if (newYAxisCoordinate < 0)
    {
        newYAxisCoordinate = 0;
    }
    // Controlling the motor
    if (newYAxisCoordinate != yAxisCoordinate)
    {
        // Defining the y axis movement per rotation
        const int Y_AXIS_MOVEMENT_PER_ROTATION = 220;
        int mmToGo;
        // Calculating the step interval
        int stepInterval = round((100.0 / motorSpeed) * Y_AXIS_MIN_STEP_INTERVAL) - 10;
        // Choosing a movement direction
        if (newYAxisCoordinate > yAxisCoordinate)
        {
            mmToGo = newYAxisCoordinate - yAxisCoordinate;
        }
        else if (newYAxisCoordinate < yAxisCoordinate)
        {
            mmToGo = yAxisCoordinate - newYAxisCoordinate;
            digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
            digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
        }
        // Sending the step signals
        for (unsigned long long i = round(((float) mmToGo / Y_AXIS_MOVEMENT_PER_ROTATION) * 3200); i > 0; i--)
        {
            digitalWrite(Y_AXIS_0_STP_PIN, HIGH);
            digitalWrite(Y_AXIS_1_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(Y_AXIS_0_STP_PIN, LOW);
            digitalWrite(Y_AXIS_1_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if(newYAxisCoordinate < yAxisCoordinate)
        {
            digitalWrite(Y_AXIS_0_DIR_PIN, !digitalRead(Y_AXIS_0_DIR_PIN));
            digitalWrite(Y_AXIS_1_DIR_PIN, !digitalRead(Y_AXIS_1_DIR_PIN));
        }
        // Updating the known y coordinate
        yAxisCoordinate = newYAxisCoordinate;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Defining a z axis servo controlling function
void moveZAxis(int newZAxisCoordinate)
{
    // Defining maximum z axis coordinate
    const int Z_AXIS_COORDINATE_MAX = 75;
    // Limiting the parameter range
    if (newZAxisCoordinate > Z_AXIS_COORDINATE_MAX)
    {
        newZAxisCoordinate = Z_AXIS_COORDINATE_MAX;
    }
    else if (newZAxisCoordinate < 0)
    {
        newZAxisCoordinate = 0;
    }
    // Controlling the servo
    if (newZAxisCoordinate != servo.read())
    {
        servo.write(newZAxisCoordinate);
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Defining a c axis motor controlling function
void moveCAxis(int newCAxisCoordinate)
{
    // Defining maximum c axis coordinate
    const int C_AXIS_COORDINATE_MAX = 359;
    // Defining variable to store current coordinate
    static int cAxisCoordinate = 0;
    // Limiting the parameter range
    if (newCAxisCoordinate > C_AXIS_COORDINATE_MAX)
    {
        newCAxisCoordinate = C_AXIS_COORDINATE_MAX;
    }
    else if (newCAxisCoordinate < 0)
    {
        newCAxisCoordinate = 0;
    }
    // Controlling the motor
    if (newCAxisCoordinate != cAxisCoordinate)
    {
        // Defining the minimum c axis step interval
        const int C_AXIS_MIN_STEP_INTERVAL = 200;
        int angleToGo;
        // Calculating the step interval
        int stepInterval = round((100.0 / motorSpeed) * C_AXIS_MIN_STEP_INTERVAL) - 10;
        // Changing the new coordinate sometimes to avoid cable damage
        if (newCAxisCoordinate > 180)
        {
            newCAxisCoordinate -= 360;
        }
        if (cAxisCoordinate > 180)
        {
            cAxisCoordinate -= 360;
        }
        // Choosing a movement direction
        if (newCAxisCoordinate > cAxisCoordinate)
        {
            angleToGo = newCAxisCoordinate - cAxisCoordinate;
        }
        else if (newCAxisCoordinate < cAxisCoordinate)
        {
            angleToGo = cAxisCoordinate - newCAxisCoordinate;
            digitalWrite(C_AXIS_DIR_PIN, !digitalRead(C_AXIS_DIR_PIN));
        }
        // Sending the step signals
        for (unsigned long long i = round(((float) angleToGo / 360) * 3200); i > 0; i--)
        {
            digitalWrite(C_AXIS_STP_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(C_AXIS_STP_PIN, LOW);
            delayMicroseconds(stepInterval);
        }
        // Resetting the movement direction
        if(newCAxisCoordinate < cAxisCoordinate)
        {
            digitalWrite(C_AXIS_DIR_PIN, !digitalRead(C_AXIS_DIR_PIN));
        }
        // Recreating the original new coordinate after changing it
        if (newCAxisCoordinate < 0)
        {
            newCAxisCoordinate += 360;
        }
        // Updating the known c axis coordinate
        cAxisCoordinate = newCAxisCoordinate;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Defining a vacuum pump controlling function
void controlVacuumPump(int newVacuumPumpDutyCycle)
{
    // Defining maximum vacuum pump duty cycle
    const int VACUUM_PUMP_DUTY_CYCLE_MAX = 100;
    // Defining variable to store current vacuum pump duty cycle
    static int vacuumPumpDutyCycle = 0;
    // Limiting the parameter range
    if (newVacuumPumpDutyCycle > VACUUM_PUMP_DUTY_CYCLE_MAX)
    {
        newVacuumPumpDutyCycle = VACUUM_PUMP_DUTY_CYCLE_MAX;
    }
    else if (newVacuumPumpDutyCycle < 0)
    {
        newVacuumPumpDutyCycle = 0;
    }
    // Controlling the vacuum pump
    if (newVacuumPumpDutyCycle != vacuumPumpDutyCycle)
    {
        analogWrite(VACUUM_PUMP_PIN, round(((float) newVacuumPumpDutyCycle / 100) * 255));
        // Updating the known vacuum pump duty cycle
        vacuumPumpDutyCycle = newVacuumPumpDutyCycle;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}

// Defining a LED controlling function
void controlLed(int newLedDutyCycle)
{
    // Defining maximum LED duty cycle
    const int LED_DUTY_CYCLE_MAX = 100;
    // Defining variable to store current LED duty cycle
    static int ledDutyCycle = 0;
    // Limiting the parameter range
    if (newLedDutyCycle > LED_DUTY_CYCLE_MAX)
    {
        newLedDutyCycle = LED_DUTY_CYCLE_MAX;
    }
    else if (newLedDutyCycle < 0)
    {
        newLedDutyCycle = 0;
    }
    // Controlling the LED
    if (newLedDutyCycle != ledDutyCycle)
    {
        analogWrite(LED_PIN, round(((float) newLedDutyCycle / 100) * 255));
        // Updating the known LED duty cycle
        ledDutyCycle = newLedDutyCycle;
    }
    // Printing an availability message
    Serial.print((String) AVAILABILITY_MESSAGE + '\n');
}
