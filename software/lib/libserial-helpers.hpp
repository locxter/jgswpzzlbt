#ifndef LIBSERIAL_HELPERS
#define LIBSERIAL_HELPERS
#include <iostream>
#include <unistd.h>
#include <libserial/SerialStream.h>

// Serial messages
const char AVAILABILITY_MESSAGE = 'R';
const char ERROR_MESSAGE = 'E';
const char X_AXIS_COMMAND = 'X';
const char Y_AXIS_COMMAND = 'Y';
const char Z_AXIS_COMMAND = 'Z';
const char C_AXIS_COMMAND = 'C';
const char VACUUM_SYSTEM_COMMAND = 'V';

// Function for sending serial commands
void sendCommand(LibSerial::SerialStream& serial, char command, int commandParameter);

// Function to move to a x and y coordinate
void moveTo(LibSerial::SerialStream& serial, int x, int y);

// Function to pick a part from the puzzle mat
void pickPartFromPuzzleMat(LibSerial::SerialStream& serial);

// Function to pick a part from the turntable
void pickPartFromTurntable(LibSerial::SerialStream& serial);

// Function to release a part to the puzzle mat
void releasePartToPuzzleMat(LibSerial::SerialStream& serial);

// Function to release a part to the turntable
void releasePartToTurntable(LibSerial::SerialStream& serial);

// Function to rotate a part
void rotatePart(LibSerial::SerialStream& serial, int angle);
#endif
