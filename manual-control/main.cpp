#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <libserial/SerialStream.h>

using namespace std;
using namespace cv;
using namespace LibSerial;

// Defining serial messages
const char AVAILABILITY_MESSAGE = 'R';
const char ERROR_MESSAGE = 'E';
const char X_AXIS_COMMAND = 'X';
const char Y_AXIS_COMMAND = 'Y';
const char Z_AXIS_COMMAND = 'Z';
const char C_AXIS_COMMAND = 'C';
const char VACUUM_SYSTEM_COMMAND = 'V';

// Defining robot maximums
const int X_AXIS_MAX_COORDINATE = 825;
const int Y_AXIS_MAX_COORDINATE = 725;
const int Z_AXIS_MAX_COORDINATE = 90;

// Declaring a text drawing function
Mat drawText(Mat image, string text);

// Declaring an image showing function
void showImage(String windowName, Mat image);

// Declaing a picture capturing function
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients);

// Declaring a picture saving function
void savePicture(Mat image, string fileName);

// Declaring a command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter);

// Main function
int main(int argc, char** argv) {
    // Checking for the right number of command line arguments
    if (argc == 4) {
        // Storing commandline arguments and program name
        const string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = stoi(argv[2]);
        const string SERIAL_PORT = argv[3];
        const string WINDOW_NAME = "jgswpzzlbt manual control";
        // Creating objects for robot communication
        VideoCapture camera;
        Mat cameraMatrix;
        Mat distortionCoefficients;
        FileStorage cameraCalibration;
        SerialStream serial;
        string serialInput;
        char serialResponse;
        // Initializing robot
        cameraCalibration.open("camera-calibration.xml", FileStorage::READ);
        if (cameraCalibration.isOpened()) {
            cout << "Opened camera calibration successfully." << endl;
        } else {
            cout << "Failed to open camera calibration." << endl;
            return 1;
        }
        cameraCalibration["camera-matrix"] >> cameraMatrix;
        cameraCalibration["distortion-coefficients"] >> distortionCoefficients;
        cameraCalibration.release();
        camera.open(CAMERA_ID, CAP_V4L);
        if (camera.isOpened()) {
            cout << "Opened camera successfully." << endl;
        } else {
            cout << "Failed to open camera." << endl;
            return 1;
        }
        camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
        camera.set(CAP_PROP_FRAME_WIDTH, 1920);
        camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
        camera.set(CAP_PROP_FPS, 25);
        camera.set(CAP_PROP_BUFFERSIZE, 1);
        serial.Open(SERIAL_PORT);
        if (serial.IsOpen()) {
            cout << "Opened serial port successfully." << endl;
        } else {
            cout << "Unable to open serial port." << endl;
        }
        serial.SetBaudRate(BaudRate::BAUD_115200);
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE) {
            cout << "Robot homed successfully." << endl;
        } else {
            cout << "Homing of the robot failed." << endl;
            return 1;
        }
        // Displaying a small help at start
        namedWindow(WINDOW_NAME);
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nF: Save the current frame\n1 to 9: Set the value change to 1, 5, 10, 25, 50, 75, 100, 250 or 500\nD: Increase x axis coordinate\nA: Decrease x axis coordinate\nW: Increase y axis coordinate\nS: Decrease y axis coordinate\nUP ARROW: Increase z axis coordinate\nDOWN ARROW : Decrease z axis coordinate\nRIGHT ARROW: Increase c axis coordinate\nLEFT ARROW: Decrease c axis coordinate\nT: Turn vacuum system on\nG: Turn vacuum system off\nR: Indicate that the next operation can be performed"));
        // Endless frame capturing, displaying and communcation loop
        while (true) {
            // Creating image containers
            Mat rawFrame;
            Mat resizedFrame;
            Mat annotatedFrame;
            // Creating variable for storing the key pressed
            int keyPressed;
            // Defining coordinate storage
            static int xAxisCoordinate = 0;
            static int yAxisCoordinate = 0;
            static int zAxisCoordinate = 90;
            static bool vacuumSystemIsOn = false;
            // Creating variable for storing the increment/decrement
            static int valueChange = 10;
            // Showing a frame
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            resize(rawFrame, resizedFrame, Size(1280, 720));
            annotatedFrame = drawText(resizedFrame, "Value change: " + to_string(valueChange) + " X: " + to_string(xAxisCoordinate) + " Y: " + to_string(yAxisCoordinate) + " Z: " + to_string(zAxisCoordinate) + " V: " + to_string(vacuumSystemIsOn));
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
                cout << "Value change: 1" << endl;
            } else if (keyPressed == 50) {
                valueChange = 5;
                cout << "Value change: 5" << endl;
            } else if (keyPressed == 51) {
                valueChange = 10;
                cout << "Value change: 10" << endl;
            } else if (keyPressed == 52) {
                valueChange = 25;
                cout << "Value change: 25" << endl;
            } else if (keyPressed == 53) {
                valueChange = 50;
                cout << "Value change: 50" << endl;
            } else if (keyPressed == 54) {
                valueChange = 75;
                cout << "Value change: 75" << endl;
            } else if (keyPressed == 55) {
                valueChange = 100;
                cout << "Value change: 100" << endl;
            } else if (keyPressed == 56) {
                valueChange = 250;
                cout << "Value change: 250" << endl;
            } else if (keyPressed == 57) {
                valueChange = 500;
                cout << "Value change: 500" << endl;
            }
            // Increasing x axis coordinate when D is pressed
            else if (keyPressed == 100) {
                xAxisCoordinate += valueChange;
                if (xAxisCoordinate > X_AXIS_MAX_COORDINATE) {
                    xAxisCoordinate = X_AXIS_MAX_COORDINATE;
                }
                sendCommand(serial, X_AXIS_COMMAND, xAxisCoordinate);
                cout << "X: " << xAxisCoordinate << endl;
            }
            // Decreasing x axis coordinate when A is pressed
            else if (keyPressed == 97) {
                xAxisCoordinate -= valueChange;
                if (xAxisCoordinate < 0) {
                    xAxisCoordinate = 0;
                }
                sendCommand(serial, X_AXIS_COMMAND, xAxisCoordinate);
                cout << "X: " << xAxisCoordinate << endl;
            }
            // Increasing y axis coordinate when W is pressed
            else if (keyPressed == 119) {
                yAxisCoordinate += valueChange;
                if (yAxisCoordinate > Y_AXIS_MAX_COORDINATE) {
                    yAxisCoordinate = Y_AXIS_MAX_COORDINATE;
                }
                sendCommand(serial, Y_AXIS_COMMAND, yAxisCoordinate);
                cout << "Y: " << yAxisCoordinate << endl;
            }
            // Decreasing y axis coordinate when S is pressed
            else if (keyPressed == 115) {
                yAxisCoordinate -= valueChange;
                if (yAxisCoordinate < 0) {
                    yAxisCoordinate = 0;
                }
                sendCommand(serial, Y_AXIS_COMMAND, yAxisCoordinate);
                cout << "Y: " << yAxisCoordinate << endl;
            }
            // Increasing z axis coordinate when UP ARROW is pressed
            else if (keyPressed == 82) {
                zAxisCoordinate += valueChange;
                if (zAxisCoordinate > Z_AXIS_MAX_COORDINATE) {
                    zAxisCoordinate = Z_AXIS_MAX_COORDINATE;
                }
                sendCommand(serial, Z_AXIS_COMMAND, zAxisCoordinate);
                cout << "Z: " << yAxisCoordinate << endl;
            }
            // Decreasing z axis coordinate when DOWN ARROW is pressed
            else if (keyPressed == 84) {
                zAxisCoordinate -= valueChange;
                if (zAxisCoordinate < 0) {
                    zAxisCoordinate = 0;
                }
                sendCommand(serial, Z_AXIS_COMMAND, zAxisCoordinate);
                cout << "Z: " << yAxisCoordinate << endl;
            }
            // Increasing c axis coordinate when RIGHT ARROW is pressed
            else if (keyPressed == 83) {
                sendCommand(serial, C_AXIS_COMMAND, valueChange);
                cout << "C: " << valueChange << endl;
            }
            // Decreasing c axis coordinate when LEFT ARROW is pressed
            else if (keyPressed == 81) {
                sendCommand(serial, C_AXIS_COMMAND, valueChange * -1);
                cout << "C: " << (valueChange * -1) << endl;
            }
            // Turning vacuum system on when T is pressed
            else if (keyPressed == 116) {
                vacuumSystemIsOn = true;
                sendCommand(serial, VACUUM_SYSTEM_COMMAND, vacuumSystemIsOn);
                cout << "V: " << vacuumSystemIsOn << endl;
            }
            // Turning vacuum system off when G is pressed
            else if (keyPressed == 103) {
                vacuumSystemIsOn = false;
                sendCommand(serial, VACUUM_SYSTEM_COMMAND, vacuumSystemIsOn);
                cout << "V: " << vacuumSystemIsOn << endl;
            }
        }
        // Closing the program
        camera.release();
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
    string lineBuffer;
    vector<string> lines;
    stringstream textStream(text);
    // Cloining the input image for non-destructive drawing
    Mat imageBuffer = image.clone();
    // Splitting the text into lines and writing them on the image one by one
    while (getline(textStream, lineBuffer, '\n')) {
        lines.push_back(lineBuffer);
    }
    for (int i = 0; i < lines.size(); i++) {
        putText(imageBuffer, lines[i], Point(0, 30 * (i + 1)), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255));
    }
    return imageBuffer;
}

// Defining an image showing function
void showImage(String windowName, Mat image) {
    Mat imageBuffer;
    // Resizing the image to fit inside the 1280 by 720 bounding box if needed
    if (image.cols > 1280 || image.rows > 720) {
        float scale;
        if (image.cols > image.rows) {
            scale = 1280.0 / image.cols;
        } else {
            scale = 720.0 / image.rows;
        }
        resize(image, imageBuffer, Size(), scale, scale);
    } else {
        imageBuffer = image.clone();
    }
    // Showing the image until the user reacts
    while (true) {
        imshow(windowName, imageBuffer);
        const int KEY_PRESSED = waitKey(1000 / 25);
        if (KEY_PRESSED == 113) {
            exit(0);
        } else if (KEY_PRESSED == 114) {
            break;
        }
    }
}

// Defining picture capturing function
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients) {
    Mat rawFrame;
    Mat undistortedFrame;
    Mat flippedFrame;
    // Cleaing the buffer and reading a frame
    camera.grab();
    camera.read(rawFrame);
    if (rawFrame.empty()) {
        cout << "Blank frame grabbed." << endl;
        exit(1);
    }
    // Performing basic preprocessing
    undistort(rawFrame, undistortedFrame, cameraMatrix, distortionCoefficients);
    flip(undistortedFrame, flippedFrame, -1);
    return flippedFrame;
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

// Defining command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter) {
    string serialInput;
    char serialResponse;
    // Sending the given command and waiting for a response
    serial << command << commandParameter << endl;
    getline(serial, serialInput);
    serialResponse = serialInput.at(0);
    if (serialResponse == ERROR_MESSAGE) {
        cout << "Robot failed to execute the command " << command << commandParameter << '.' << endl;
        exit(1);
    }
}