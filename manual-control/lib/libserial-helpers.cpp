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
        std::exit(1);
    }
}