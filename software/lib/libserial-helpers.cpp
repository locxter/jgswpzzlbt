#include "libserial-helpers.hpp"

// Function for sending serial commands
void sendCommand(LibSerial::SerialStream& serial, char command, int commandParameter) {
    std::string serialInput;
    char serialResponse;
    // Sending the given command and waiting for a response
    serial << command << commandParameter << std::endl;
    std::getline(serial, serialInput);
    serialResponse = serialInput.at(0);
    if (serialResponse == ERROR_MESSAGE) {
        std::cout << "Robot failed to execute the command " << command << commandParameter << '.' << std::endl;
        exit(1);
    }
}

// Function to move to a x and y coordinate
void moveTo(LibSerial::SerialStream& serial, int xCoordinate, int yCoordinate) {
    sendCommand(serial, X_AXIS_COMMAND, xCoordinate);
    sendCommand(serial, Y_AXIS_COMMAND, yCoordinate);
}

// Function to pick a part from the puzzle mat
void pickPartFromPuzzleMat(LibSerial::SerialStream& serial) {
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 100);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

// Function to pick a part from the turntable
void pickPartFromTurntable(LibSerial::SerialStream& serial) {
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 100);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 50);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

// Function to release a part to the puzzle mat
void releasePartToPuzzleMat(LibSerial::SerialStream& serial) {
    sendCommand(serial, Z_AXIS_COMMAND, 30);
    sleep(1);
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

// Function to release a part to the turntable
void releasePartToTurntable(LibSerial::SerialStream& serial) {
    sendCommand(serial, Z_AXIS_COMMAND, 80);
    sleep(1);
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

// Function to rotate a part
void rotatePart(LibSerial::SerialStream& serial, int angle) {
    sendCommand(serial, X_AXIS_COMMAND, 0);
    sleep(1);
    releasePartToTurntable(serial);
    sendCommand(serial, C_AXIS_COMMAND, angle);
    sleep(1);
    pickPartFromTurntable(serial);
}