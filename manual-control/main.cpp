#include <iostream>
#include "lib/opencv-helpers.hpp"
#include "lib/libserial-helpers.hpp"

// Main function
int main(int argc, char** argv) {
    // Check for the right number of command line arguments
    if (argc == 4) {
        // Variables for command line arguments
        const std::string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = std::stoi(argv[2]);
        const std::string SERIAL_PORT = argv[3];
        const std::string WINDOW_NAME = "jgswpzzlbt manual control";
        // Robot axis maximums
        const int X_AXIS_MAX = 825;
        const int Y_AXIS_MAX = 725;
        const int Z_AXIS_MAX = 90;
        // Objects for robot communication
        cv::VideoCapture camera;
        cv::Mat cameraMatrix;
        cv::Mat distortionCoefficients;
        cv::FileStorage cameraCalibration;
        LibSerial::SerialStream serial;
        std::string response;
        // Initialize robot
        cameraCalibration.open("camera-calibration.xml", cv::FileStorage::READ);
        if (cameraCalibration.isOpened()) {
            std::cout << "Opened camera calibration successfully." << std::endl;
        } else {
            std::cout << "Failed to open camera calibration." << std::endl;
            return 1;
        }
        cameraCalibration["camera-matrix"] >> cameraMatrix;
        cameraCalibration["distortion-coefficients"] >> distortionCoefficients;
        cameraCalibration.release();
        camera.open(CAMERA_ID, cv::CAP_V4L);
        if (camera.isOpened()) {
            std::cout << "Opened camera successfully." << std::endl;
        } else {
            std::cout << "Failed to open camera." << std::endl;
            return 1;
        }
        camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        camera.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
        camera.set(cv::CAP_PROP_FPS, 25);
        camera.set(cv::CAP_PROP_BUFFERSIZE, 1);
        serial.Open(SERIAL_PORT);
        if (serial.IsOpen()) {
            std::cout << "Opened serial port successfully." << std::endl;
        } else {
            std::cout << "Unable to open serial port." << std::endl;
            return 1;
        }
        serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        std::getline(serial, response);
        if (response.at(0) == AVAILABILITY_MESSAGE) {
            std::cout << "Robot homed successfully." << std::endl;
        } else {
            std::cout << "Homing of the robot failed." << std::endl;
            return 1;
        }
        // Display a small help at start
        cv::namedWindow(WINDOW_NAME);
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nF: Save the current frame\n1 to 9: Set the value change to 1, 5, 10, 25, 50, 75, 100, 250 or 500\nD: Increase x axis coordinate\nA: Decrease x axis coordinate\nW: Increase y axis coordinate\nS: Decrease y axis coordinate\nUP ARROW: Increase z axis coordinate\nDOWN ARROW : Decrease z axis coordinate\nRIGHT ARROW: Increase c axis coordinate\nLEFT ARROW: Decrease c axis coordinate\nT: Turn vacuum system on\nG: Turn vacuum system off\nR: Indicate that the next operation can be performed"));
        // Endless frame capturing, displaying and communcation loop
        while (true) {
            // Image containers and variable for storing the key pressed
            cv::Mat rawFrame;
            cv::Mat resizedFrame;
            cv::Mat annotatedFrame;
            int keyPressed;
            // Coordinate storage and variable for storing the increment/decrement
            static int x = 0;
            static int y = 0;
            static int z = 90;
            static bool isVacuumOn = false;
            static int valueChange = 10;
            // Show a frame
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cv::resize(rawFrame, resizedFrame, cv::Size(1280, 720));
            annotatedFrame = drawText(resizedFrame, "Value change: " + std::to_string(valueChange) + " X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z: " + std::to_string(z) + " V: " + std::to_string(isVacuumOn));
            cv::imshow(WINDOW_NAME, annotatedFrame);
            // Fetch user input
            keyPressed = cv::waitKey(1000 / 25);
            // Quit when Q is pressed
            if (keyPressed == 113) {
                break;
            }
            // Save the current frame when F is pressed
            else if (keyPressed == 102) {
                savePicture(rawFrame, "manual-control");
            }
            // Set the value change to one of the presets
            else if (keyPressed == 49) {
                valueChange = 1;
                std::cout << "Value change: 1" << std::endl;
            } else if (keyPressed == 50) {
                valueChange = 5;
                std::cout << "Value change: 5" << std::endl;
            } else if (keyPressed == 51) {
                valueChange = 10;
                std::cout << "Value change: 10" << std::endl;
            } else if (keyPressed == 52) {
                valueChange = 25;
                std::cout << "Value change: 25" << std::endl;
            } else if (keyPressed == 53) {
                valueChange = 50;
                std::cout << "Value change: 50" << std::endl;
            } else if (keyPressed == 54) {
                valueChange = 75;
                std::cout << "Value change: 75" << std::endl;
            } else if (keyPressed == 55) {
                valueChange = 100;
                std::cout << "Value change: 100" << std::endl;
            } else if (keyPressed == 56) {
                valueChange = 250;
                std::cout << "Value change: 250" << std::endl;
            } else if (keyPressed == 57) {
                valueChange = 500;
                std::cout << "Value change: 500" << std::endl;
            }
            // Increase x axis coordinate when D is pressed
            else if (keyPressed == 100) {
                x += valueChange;
                if (x > X_AXIS_MAX) {
                    x = X_AXIS_MAX;
                }
                sendCommand(serial, X_AXIS_COMMAND, x);
                std::cout << "X: " << x << std::endl;
            }
            // Decrease x axis coordinate when A is pressed
            else if (keyPressed == 97) {
                x -= valueChange;
                if (x < 0) {
                    x = 0;
                }
                sendCommand(serial, X_AXIS_COMMAND, x);
                std::cout << "X: " << x << std::endl;
            }
            // Increase y axis coordinate when W is pressed
            else if (keyPressed == 119) {
                y += valueChange;
                if (y > Y_AXIS_MAX) {
                    y = Y_AXIS_MAX;
                }
                sendCommand(serial, Y_AXIS_COMMAND, y);
                std::cout << "Y: " << y << std::endl;
            }
            // Decrease y axis coordinate when S is pressed
            else if (keyPressed == 115) {
                y -= valueChange;
                if (y < 0) {
                    y = 0;
                }
                sendCommand(serial, Y_AXIS_COMMAND, y);
                std::cout << "Y: " << y << std::endl;
            }
            // Increase z axis coordinate when UP ARROW is pressed
            else if (keyPressed == 82) {
                z += valueChange;
                if (z > Z_AXIS_MAX) {
                    z = Z_AXIS_MAX;
                }
                sendCommand(serial, Z_AXIS_COMMAND, z);
                std::cout << "Z: " << z << std::endl;
            }
            // Decrease z axis coordinate when DOWN ARROW is pressed
            else if (keyPressed == 84) {
                z -= valueChange;
                if (z < 0) {
                    z = 0;
                }
                sendCommand(serial, Z_AXIS_COMMAND, z);
                std::cout << "Z: " << z << std::endl;
            }
            // Increase c axis coordinate when RIGHT ARROW is pressed
            else if (keyPressed == 83) {
                sendCommand(serial, C_AXIS_COMMAND, valueChange);
                std::cout << "C: " << valueChange << std::endl;
            }
            // Decrease c axis coordinate when LEFT ARROW is pressed
            else if (keyPressed == 81) {
                sendCommand(serial, C_AXIS_COMMAND, valueChange * -1);
                std::cout << "C: " << (valueChange * -1) << std::endl;
            }
            // Turning vacuum system on when T is pressed
            else if (keyPressed == 116) {
                isVacuumOn = true;
                sendCommand(serial, VACUUM_SYSTEM_COMMAND, isVacuumOn);
                std::cout << "V: " << isVacuumOn << std::endl;
            }
            // Turning vacuum system off when G is pressed
            else if (keyPressed == 103) {
                isVacuumOn = false;
                sendCommand(serial, VACUUM_SYSTEM_COMMAND, isVacuumOn);
                std::cout << "V: " << isVacuumOn << std::endl;
            }
        }
        // Close the program
        camera.release();
        serial.Close();
        return 0;
    } else {
        // Throw an error on invalid number of command line arguments
        std::cout << "Wrong number of arguments. Three arguments containing the path of the camera calibration file, camera ID and serial port expected." << std::endl;
        std::cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0" << std::endl;
        return 1;
    }
}
