// Including the needed library
#include <Servo.h>

// Defining global pins
const int SERVO_PIN = 11;

// Defining global constants
const char AVAILABILITY_MESSAGE = 'R';

// Creating a new servo object
Servo servo;

void setup()
{
    // Initiating servo
    servo.attach(SERVO_PIN);
    // Opening the serial connection
    Serial.begin(115200);
    // Printing an availability message
    Serial.println(AVAILABILITY_MESSAGE);
}

void loop()
{
    if (Serial.available() > 0)
    {
        int serialValue = Serial.parseInt();
        if (serialValue > 180)
        {
            serialValue = 180;
        }
        else if (serialValue < 0)
        {
            serialValue = 0;
        }
        if (serialValue != servo.read())
        {
            servo.write(serialValue);
        }
        Serial.println('R');
    }
}
