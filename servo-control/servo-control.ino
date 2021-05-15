// Including the needed library
#include <Servo.h>

// Defining global pins
const int SERVO_PIN = 11;

// Defining global constants
const String AVAILABILITY_MESSAGE = "R\n";

// Creating a new servo object
Servo servo;

void setup()
{
    // Initiating servo
    servo.attach(SERVO_PIN);
    // Opening the serial connection
    Serial.begin(115200);
    // Printing an availability message
    Serial.print(AVAILABILITY_MESSAGE);
}

void loop()
{
    // Checking for new serial data
    if (Serial.available() > 0)
    {
        // Reading the value
        int serialValue = Serial.parseInt();
        // Limiting the value range
        if (serialValue > 180)
        {
            serialValue = 180;
        }
        else if (serialValue < 0)
        {
            serialValue = 0;
        }
        // Updating the servo angle
        if (serialValue != servo.read())
        {
            servo.write(serialValue);
        }
        // Printing an availability message
        Serial.print(AVAILABILITY_MESSAGE);
    }
}
