// Including needed headers
#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <libserial/SerialStream.h>

// Setting default namespaces
using namespace std;
using namespace cv;
using namespace LibSerial;

// Defining messages
const char AVAILABILITY_MESSAGE = 'R';
const char ERROR_MESSAGE = 'E';

// Declaring a text drawing function
Mat drawText(Mat image, string text);

// Declaring an image showing function
void showImage(String windowName, Mat image);

// Declaring a command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter);

// Declaring a picture saving function
void savePicture(Mat image, string fileName);

// Declaing a picture capturing function
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients);

// Main function
int main(int argc, char** argv) {
    // Checking for the right number of command line arguments
    if (argc == 4) {
        // Defining camera calibration file, camera ID, serial port and window name
        const string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = stoi(argv[2]);
        const string SERIAL_PORT = argv[3];
        const string WINDOW_NAME = "jgswpzzlbt manual control";
        // Creating a camera object
        VideoCapture camera;
        // Creating camera calibration objects
        Mat cameraMatrix;
        Mat distortionCoefficients;
        FileStorage cameraCalibration;
        // Creating a serial port object
        SerialStream serial;
        // Creating variables for storing serial input and extracted response
        string serialInput;
        char serialResponse;
        // Opening the camera calibration file
        cameraCalibration.open("camera-calibration.xml", FileStorage::READ);
        // Checking for success
        if (cameraCalibration.isOpened()) {
            cout << "Opened camera calibration successfully." << endl;
        } else {
            cout << "Failed to open camera calibration." << endl;
            return 1;
        }
        // Reading the calibration
        cameraCalibration["camera-matrix"] >> cameraMatrix;
        cameraCalibration["distortion-coefficients"] >> distortionCoefficients;
        // Closing the file
        cameraCalibration.release();
        // Opening the camera
        camera.open(CAMERA_ID, CAP_V4L);
        // Checking for success
        if (camera.isOpened()) {
            cout << "Opened camera successfully." << endl;
        } else {
            cout << "Failed to open camera." << endl;
            return 1;
        }
        // Changing some camera settings
        camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
        camera.set(CAP_PROP_FRAME_WIDTH, 1920);
        camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
        camera.set(CAP_PROP_FPS, 25);
        camera.set(CAP_PROP_BUFFERSIZE, 1);
        // Opening the serial port
        serial.Open(SERIAL_PORT);
        // Checking for success
        if (serial.IsOpen()) {
            cout << "Opened serial port successfully." << endl;
        } else {
            cout << "Unable to open serial port." << endl;
        }
        // Changing the baud rate
        serial.SetBaudRate(BaudRate::BAUD_115200);
        // Checking for successful homing
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE) {
            cout << "Robot homed successfully." << endl;
        } else {
            cout << "Homing of the robot failed." << endl;
            return 1;
        }
        // Creating a new window
        namedWindow(WINDOW_NAME);
        // Displaying a small help at startup
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nF: Save the current frame\n1 to 9: Set the value change to 1, 5, 10, 25, 50, 75, 100, 250 or 500\nINSERT: Increase motor speed\nDELETE: Decrease motor speed\nD: Increase x axis coordinate\nA Decrease x axis coordinate\nW: Increase y axis coordinate\nS: Decrease y axis coordinate\nUP ARROW: Increase z axis coordinate\nDOWN ARROW : Decrease z axis coordinate\nRIGHT ARROW: Increase c axis coordinate\nLEFT ARROW: Decrease c axis coordinate\nHOME: Increase vacuum pump duty cycle\nEND: Decrease vacuum pump duty cycle\nPAGE UP: Increase LED duty cycle\nPAGE DOWN: Decrease LED duty cycle\nR: Indicate that the next operation can be performed"));
        // Endless frame capturing, displaying and communcating loop
        while (true) {
            // Creating image containers
            Mat rawFrame;
            Mat annotatedFrame;
            // Creating variable for storing the key pressed
            int keyPressed;
            // Defining robot control related variables
            static int motorSpeed = 100;
            static int xAxisCoordinate = 0;
            static int yAxisCoordinate = 0;
            static int zAxisCoordinate = 40;
            static int cAxisCoordinate = 0;
            static int vacuumPumpDutyCyle = 0;
            static int ledDutyCyle = 0;
            const int MOTOR_SPEED_MAX = 100;
            const int X_AXIS_COORDINATE_MAX = 825;
            const int Y_AXIS_COORDINATE_MAX = 725;
            const int Z_AXIS_COORDINATE_MAX = 40;
            const int C_AXIS_COORDINATE_MAX = 359;
            const int VACUUM_PUMP_DUTY_CYCLE_MAX = 100;
            const int LED_DUTY_CYCLE_MAX = 100;
            const char MOTOR_SPEED_COMMAND = 'S';
            const char X_AXIS_COMMAND = 'X';
            const char Y_AXIS_COMMAND = 'Y';
            const char Z_AXIS_COMMAND = 'Z';
            const char C_AXIS_COMMAND = 'C';
            const char VACUUM_PUMP_COMMAND = 'V';
            const char LED_COMMAND = 'L';
            // Creating variable for storing the increment/decrement
            static int valueChange = 10;
            // Capturing a frame
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            // Adding some status information to the frame
            annotatedFrame = drawText(rawFrame, "VC: " + to_string(valueChange) + " S: " + to_string(motorSpeed) + " X: " + to_string(xAxisCoordinate) + " Y: " + to_string(yAxisCoordinate) + " Z: " + to_string(zAxisCoordinate) + " C: " + to_string(cAxisCoordinate) + " V: " + to_string(vacuumPumpDutyCyle) + " L: " + to_string(ledDutyCyle));
            // Showing the frame
            imshow(WINDOW_NAME, annotatedFrame);
            // Fetching user input
            keyPressed = waitKey(1000 / 25);
            // Quiting when Q is pressed
            if (keyPressed == 113) {
                break;
            }
            // Saving the current frame when F is pressed
            else if (keyPressed == 102) {
                savePicture(rawFrame, "manual-control");
            }
            // Setting the value change to one of the presets
            else if (keyPressed == 49) {
                valueChange = 1;
                cout << "Value change changed to 1." << endl;
            } else if (keyPressed == 50) {
                valueChange = 5;
                cout << "Value change changed to 5." << endl;
            } else if (keyPressed == 51) {
                valueChange = 10;
                cout << "Value change changed to 10." << endl;
            } else if (keyPressed == 52) {
                valueChange = 25;
                cout << "Value change changed to 25." << endl;
            } else if (keyPressed == 53) {
                valueChange = 50;
                cout << "Value change changed to 50." << endl;
            } else if (keyPressed == 54) {
                valueChange = 75;
                cout << "Value change changed to 75." << endl;
            } else if (keyPressed == 55) {
                valueChange = 100;
                cout << "Value change changed to 100." << endl;
            } else if (keyPressed == 56) {
                valueChange = 250;
                cout << "Value change changed to 250." << endl;
            } else if (keyPressed == 57) {
                valueChange = 500;
                cout << "Value change changed to 500." << endl;
            }
            // Increasing motor speed when INSERT is pressed
            else if (keyPressed == 99) {
                motorSpeed += valueChange;
                if (motorSpeed > MOTOR_SPEED_MAX) {
                    motorSpeed = MOTOR_SPEED_MAX;
                }
                sendCommand(serial, MOTOR_SPEED_COMMAND, motorSpeed);
                cout << "Motor speed changed to " << motorSpeed << '.' << endl;
            }
            // Decreasing motor speed when DELETE is pressed
            else if (keyPressed == 255) {
                motorSpeed -= valueChange;
                if (motorSpeed < 1) {
                    motorSpeed = 1;
                }
                sendCommand(serial, MOTOR_SPEED_COMMAND, motorSpeed);
                cout << "Motor speed changed to " << motorSpeed << '.' << endl;
            }
            // Increasing x axis coordinate when D is pressed
            else if (keyPressed == 100) {
                xAxisCoordinate += valueChange;
                if (xAxisCoordinate > X_AXIS_COORDINATE_MAX) {
                    xAxisCoordinate = X_AXIS_COORDINATE_MAX;
                }
                sendCommand(serial, X_AXIS_COMMAND, xAxisCoordinate);
                cout << "X axis coordinate changed to " << xAxisCoordinate << '.' << endl;
            }
            // Decreasing x axis coordinate when A is pressed
            else if (keyPressed == 97) {
                xAxisCoordinate -= valueChange;
                if (xAxisCoordinate < 0) {
                    xAxisCoordinate = 0;
                }
                sendCommand(serial, X_AXIS_COMMAND, xAxisCoordinate);
                cout << "X axis coordinate changed to " << xAxisCoordinate << '.' << endl;
            }
            // Increasing y axis coordinate when W is pressed
            else if (keyPressed == 119) {
                yAxisCoordinate += valueChange;
                if (yAxisCoordinate > Y_AXIS_COORDINATE_MAX) {
                    yAxisCoordinate = Y_AXIS_COORDINATE_MAX;
                }
                sendCommand(serial, Y_AXIS_COMMAND, yAxisCoordinate);
                cout << "Y axis coordinate changed to " << yAxisCoordinate << '.' << endl;
            }
            // Decreasing y axis coordinate when S is pressed
            else if (keyPressed == 115) {
                yAxisCoordinate -= valueChange;
                if (yAxisCoordinate < 0) {
                    yAxisCoordinate = 0;
                }
                sendCommand(serial, Y_AXIS_COMMAND, yAxisCoordinate);
                cout << "Y axis coordinate changed to " << yAxisCoordinate << '.' << endl;
            }
            // Increasing z axis coordinate when UP ARROW is pressed
            else if (keyPressed == 82) {
                zAxisCoordinate += valueChange;
                if (zAxisCoordinate > Z_AXIS_COORDINATE_MAX) {
                    zAxisCoordinate = Z_AXIS_COORDINATE_MAX;
                }
                sendCommand(serial, Z_AXIS_COMMAND, zAxisCoordinate);
                cout << "Z axis coordinate changed to " << zAxisCoordinate << '.' << endl;
            }
            // Decreasing z axis coordinate when DOWN ARROW is pressed
            else if (keyPressed == 84) {
                zAxisCoordinate -= valueChange;
                if (zAxisCoordinate < 0) {
                    zAxisCoordinate = 0;
                }
                sendCommand(serial, Z_AXIS_COMMAND, zAxisCoordinate);
                cout << "Z axis coordinate changed to " << zAxisCoordinate << '.' << endl;
            }
            // Increasing c axis coordinate when RIGHT ARROW is pressed
            else if (keyPressed == 83) {
                cAxisCoordinate += valueChange;
                if (cAxisCoordinate > C_AXIS_COORDINATE_MAX) {
                    cAxisCoordinate = C_AXIS_COORDINATE_MAX;
                }
                sendCommand(serial, C_AXIS_COMMAND, yAxisCoordinate);
                cout << "C axis coordinate changed to " << cAxisCoordinate << '.' << endl;
            }
            // Decreasing c axis coordinate when LEFT ARROW is pressed
            else if (keyPressed == 81) {
                cAxisCoordinate -= valueChange;
                if (cAxisCoordinate < 0) {
                    cAxisCoordinate = 0;
                }
                sendCommand(serial, C_AXIS_COMMAND, cAxisCoordinate);
                cout << "C axis coordinate changed to " << cAxisCoordinate << '.' << endl;
            }
            // Increasing vacuum pump duty cycle when HOME is pressed
            else if (keyPressed == 80) {
                vacuumPumpDutyCyle += valueChange;
                if (vacuumPumpDutyCyle > VACUUM_PUMP_DUTY_CYCLE_MAX) {
                    vacuumPumpDutyCyle = VACUUM_PUMP_DUTY_CYCLE_MAX;
                }
                sendCommand(serial, VACUUM_PUMP_COMMAND, vacuumPumpDutyCyle);
                cout << "Vacuum pump duty cycle changed to " << vacuumPumpDutyCyle << '.' << endl;
            }
            // Decreasing vacuum pump duty cycle when END is pressed
            else if (keyPressed == 87) {
                vacuumPumpDutyCyle -= valueChange;
                if (vacuumPumpDutyCyle < 0) {
                    vacuumPumpDutyCyle = 0;
                }
                sendCommand(serial, VACUUM_PUMP_COMMAND, vacuumPumpDutyCyle);
                cout << "Vacuum pump duty cycle changed to " << vacuumPumpDutyCyle << '.' << endl;
            }
            // Increasing LED duty cycle when PAGE UP is pressed
            else if (keyPressed == 85) {
                ledDutyCyle += valueChange;
                if (ledDutyCyle > LED_DUTY_CYCLE_MAX) {
                    ledDutyCyle = LED_DUTY_CYCLE_MAX;
                }
                sendCommand(serial, LED_COMMAND, ledDutyCyle);
                cout << "LED duty cycle changed to " << ledDutyCyle << '.' << endl;
            }
            // Decreasing LED duty cycle when PAGE DOWN is pressed
            else if (keyPressed == 86) {
                ledDutyCyle -= valueChange;
                if (ledDutyCyle < 0) {
                    ledDutyCyle = 0;
                }
                sendCommand(serial, LED_COMMAND, ledDutyCyle);
                cout << "LED duty cycle changed to " << ledDutyCyle << '.' << endl;
            }
        }
        // Closing the camera
        camera.release();
        // Closing the serial port
        serial.Close();
        return 0;
    } else {
        // Throwing an error on invalid number of command line arguments
        cout << "Wrong number of arguments. Three arguments containing the path of the camera calibration file, camera ID and serial port expected." << endl;
        cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0" << endl;
        return 1;
    }
}

// Defining text drawing function
Mat drawText(Mat image, string text) {
    // Creating needed variables
    string lineBuffer;
    vector<string> lines;
    stringstream textStream(text);
    // Copying the image to buffer for non distructive drawing
    Mat imageBuffer = image.clone();
    // Splitting the text into lines and adding them one by one to the image
    while (getline(textStream, lineBuffer, '\n')) {
        lines.push_back(lineBuffer);
    }
    for (int i = 0; i < lines.size(); i++) {
        putText(imageBuffer, lines[i], Point(0, (50 * i) + 40), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 0), 2);
    }
    return imageBuffer;
}

// Defining an image showing function
void showImage(String windowName, Mat image) {
    while (true) {
        imshow(windowName, image);
        int keyPressed = waitKey(1000 / 25);
        // Quiting when Q is pressed
        if (keyPressed == 113) {
            exit(0);
        }
        // Moving forward when R is pressed
        else if (keyPressed == 114) {
            break;
        }
    }
}

// Defining command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter) {
    // Trying 3 times to send the command if necessary
    for (int numberOfTries = 0; numberOfTries < 3; numberOfTries++) {
        // Creating variables for storing serial input and extracted response
        string serialInput;
        char serialResponse;
        // Sending the command to the robot
        serial << command << commandParameter << endl;
        // Checking for successful execution
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE) {
            break;
        } else if (serialResponse == ERROR_MESSAGE) {
            cout << "Robot failed to execute the command " << command << commandParameter << '.' << endl;
            exit(1);
        }
    }
}

// Definining a picture saving function
void savePicture(Mat image, string fileName) {
    ifstream testFile;
    int counter = 0;
    string finalFileName = fileName + '-' + to_string(counter) + ".jpeg";
    testFile.open(finalFileName);
    while (testFile.is_open()) {
        testFile.close();
        counter++;
        finalFileName = fileName + '-' + to_string(counter) + ".jpeg";
        testFile.open(finalFileName);
    }
    testFile.close();
    imwrite(finalFileName, image);
}

// Defining picture capturing function
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients) {
    // Creating image containers
    Mat rawFrame;
    Mat undistortedFrame;
    Mat flippedFrame;
    // Reading a frame
    camera.grab();
    camera.read(rawFrame);
    // Checking for success
    if (rawFrame.empty()) {
        cout << "Blank frame grabbed." << endl;
        exit(1);
    }
    // Undistorting the frame
    undistort(rawFrame, undistortedFrame, cameraMatrix, distortionCoefficients);
    // Flipping the frame to match the used coordinate system
    flip(undistortedFrame, flippedFrame, -1);
    return flippedFrame;
}