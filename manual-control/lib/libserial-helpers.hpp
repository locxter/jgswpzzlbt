#ifndef LIBSERIAL_HELPERS
#define LIBSERIAL_HELPERS
#include <iostream>
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
#endif
