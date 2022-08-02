#include "libserial-helpers.hpp"

// Function for sending serial commands
void sendCommand(LibSerial::SerialStream& serial, char command, int commandParameter) {
    std::string response;
    // Sending the given command and waiting for a response
    serial << command << commandParameter << std::endl;
    std::getline(serial, response);
    if (response.at(0) == ERROR_MESSAGE) {
        std::cout << "Robot failed to execute the command " << command << commandParameter << '.' << std::endl;
        std::exit(1);
    }
}