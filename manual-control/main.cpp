// Including needed headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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

// Declaring text showing function
Mat showText(Mat image, string text);

// Declaring command sending function
void sendCommand(SerialStream &serial, char command, int commandParameter);

// Main function
int main(int argc, char *argv[])
{
    // Checking for the right number of command line arguments
    if (argc == 4)
    {
        // Defining camera calibration file, camera ID, serial port and window name
        const string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = stoi(argv[2]);
        const string SERIAL_PORT = argv[3];
        const string WINDOW_NAME = "jgswpzzlbt manual control";
        // Creating a camera object
        VideoCapture camera;
        // Creating camera calibration objects;
        Mat cameraMatrix;
        Mat distortionCoefficients;
        FileStorage cameraCalibration;
        // Creating a serial port object
        SerialStream serial;
        // Creating variables for storing serial input and extracted response
        string serialInput;
        char serialResponse;
        // Defining help message
        const string HELP_MESSAGE =
            "This program is fully keyboard driven. Here is a full list of all available actions:\n"
            "Q: Quit the program\n"
            "F: Save the current frame\n"
            "1 to 9: Set the value change to 1, 5, 10, 25, 50, 75, 100, 250 or 500\n"
            "INSERT: Increase motor speed\n"
            "DELETE: Decrease motor speed\n"
            "D: Increase x axis position\n"
            "A Decrease x axis position\n"
            "W: Increase y axis position\n"
            "S: Decrease y axis position\n"
            "UP ARROW: Increase z axis position\n"
            "DOWN ARROW : Decrease z axis position\n"
            "RIGHT ARROW: Increase c axis position\n"
            "LEFT ARROW: Decrease c axis position\n"
            "HOME: Increase vacuum pump duty cycle\n"
            "END: Decrease vacuum pump duty cycle\n"
            "PAGE UP: Increase LED duty cycle\n"
            "PAGE DOWN: Decrease LED duty cycle\n"
            "To close this help, press R."
        ;
        // Opening the camera calibration file
        cameraCalibration.open("camera-calibration.xml", FileStorage::READ);
        // Checking for success
        if (cameraCalibration.isOpened())
        {
            cout << "Opened camera calibration successfully." << endl;
        }
        else
        {
            cout << "Failed to open camera calibration." << endl;
            return 1;
        }
        // Reading the calibration
        cameraCalibration["camera_matrix"] >> cameraMatrix;
        cameraCalibration["distortion_coefficients"] >> distortionCoefficients;
        // Closing the file
        cameraCalibration.release();
        // Opening the camera
        camera.open(CAMERA_ID, CAP_V4L);
        // Checking for success
        if (camera.isOpened())
        {
            cout << "Opened camera successfully." << endl;
        }
        else
        {
            cout << "Failed to open camera." << endl;
            return 1;
        }
        // Changing some camera settings
        camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
        camera.set(CAP_PROP_FRAME_WIDTH, 1920);
        camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
        camera.set(CAP_PROP_FPS, 30);
        camera.set(CAP_PROP_BUFFERSIZE, 1);
        // Opening the serial port
        serial.Open(SERIAL_PORT);
        // Checking for success
        if (serial.IsOpen())
        {
            cout << "Opened serial port successfully." << endl;
        }
        else
        {
            cout << "Unable to open serial port." << endl;
        }
        // Changing the baud rate
        serial.SetBaudRate(BaudRate::BAUD_115200);
        // Checking for successful homing
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE)
        {
            cout << "Robot homed successfully." << endl;
        }
        else
        {
            cout << "Homing of the robot failed." << endl;
            return 1;
        }
        // Creating a new window
        namedWindow(WINDOW_NAME);
        // Displaying a small help at startup
        while (true)
        {
            Mat help = showText(Mat::zeros(Size(1920, 1080), CV_8UC3), HELP_MESSAGE);
            imshow(WINDOW_NAME, help);
            int keyPressed = waitKey(1000 / 30);
            // Closing the help when R is pressed
            if (keyPressed == 114)
            {
                break;
            }
        }
        // Endless frame capturing, displaying and communcating loop
        while (true)
        {
            // Creating image containers
            Mat rawFrame;
            Mat undistortedFrame;
            Mat flippedFrame;
            Mat annotatedFrame;
            // Creating variable for storing the key pressed
            int keyPressed;
            // Creating variables for robot values
            static int motorSpeed = 100;
            static int xAxisPosition = 0;
            static int yAxisPosition = 0;
            static int zAxisPosition = 0;
            static int cAxisPosition = 0;
            static int vacuumPumpDutyCyle = 0;
            static int ledDutyCyle = 0;
            // Defining maximum robot values
            const int MOTOR_SPEED_MAX = 100;
            const int X_AXIS_POSITION_MAX = 750;
            const int Y_AXIS_POSITION_MAX = 750;
            const int Z_AXIS_POSITION_MAX = 75;
            const int C_AXIS_POSITION_MAX = 359;
            const int VACUUM_PUMP_DUTY_CYCLE_MAX = 100;
            const int LED_DUTY_CYCLE_MAX = 100;
            // Defining serial commands
            const char MOTOR_SPEED_COMMAND = 'S';
            const char X_AXIS_COMMAND = 'X';
            const char Y_AXIS_COMMAND = 'Y';
            const char Z_AXIS_COMMAND = 'Z';
            const char C_AXIS_COMMAND = 'C';
            const char VACUUM_PUMP_COMMAND = 'V';
            const char LED_COMMAND = 'L';
            // Creating variable for storing the increment/decrement
            static int valueChange = 10;
            // Reading a frame
            camera.grab();
            camera.read(rawFrame);
            // Checking for success
            if (rawFrame.empty())
            {
                cout << "Blank frame grabbed." << endl;
                return 1;
            }
            // Undistorting the frame
            undistort(rawFrame, undistortedFrame, cameraMatrix, distortionCoefficients);
            // Flipping the frame to match the used coordinate system
            flip(undistortedFrame, flippedFrame, -1);
            // Adding some status information to the frame
            annotatedFrame = showText(flippedFrame, "VC: " + to_string(valueChange) + " S: " + to_string(motorSpeed) + " X: " + to_string(xAxisPosition) + " Y: " + to_string(yAxisPosition) + " Z: " + to_string(zAxisPosition) + " C: " + to_string(cAxisPosition) +  " V: " + to_string(vacuumPumpDutyCyle) + " L: " + to_string(ledDutyCyle));
            // Showing the frame
            imshow(WINDOW_NAME, annotatedFrame);
            // Fetching user input
            keyPressed = waitKey(1000 / 30);
            // Quiting when Q is pressed
            if (keyPressed == 113)
            {
                break;
            }
            // Saving the current frame when F is pressed
            else if (keyPressed == 102)
            {
                ifstream testFile;
                int counter = 0;
                string fileName = to_string(counter) + ".jpeg";
                testFile.open(fileName);
                while (testFile.is_open())
                {
                    testFile.close();
                    counter++;
                    fileName = to_string(counter) + ".jpeg";
                    testFile.open(fileName);
                }
                testFile.close();
                imwrite(fileName, flippedFrame);
                cout << "Successfully saved picture " << fileName << '.' << endl;
            }
            // Setting the value change to one of the presets
            else if (keyPressed == 49)
            {
                valueChange = 1;
                cout << "Value change changed to 1." << endl;
            }
            else if (keyPressed == 50)
            {
                valueChange = 5;
                cout << "Value change changed to 5." << endl;
            }
            else if (keyPressed == 51)
            {
                valueChange = 10;
                cout << "Value change changed to 10." << endl;
            }
            else if (keyPressed == 52)
            {
                valueChange = 25;
                cout << "Value change changed to 25." << endl;
            }
            else if (keyPressed == 53)
            {
                valueChange = 50;
                cout << "Value change changed to 50." << endl;
            }
            else if (keyPressed == 54)
            {
                valueChange = 75;
                cout << "Value change changed to 75." << endl;
            }
            else if (keyPressed == 55)
            {
                valueChange = 100;
                cout << "Value change changed to 100." << endl;
            }
            else if (keyPressed == 56)
            {
                valueChange = 250;
                cout << "Value change changed to 250." << endl;
            }
            else if (keyPressed == 57)
            {
                valueChange = 500;
                cout << "Value change changed to 500." << endl;
            }
            // Increasing motor speed when INSERT is pressed
            else if (keyPressed == 99)
            {
                motorSpeed += valueChange;
                if (motorSpeed > MOTOR_SPEED_MAX)
                {
                    motorSpeed = MOTOR_SPEED_MAX;
                }
                sendCommand(serial, MOTOR_SPEED_COMMAND, motorSpeed);
                cout << "Motor speed changed to " << motorSpeed << '.' << endl;
            }
            // Decreasing motor speed when DELETE is pressed
            else if (keyPressed == 255)
            {
                motorSpeed -= valueChange;
                if (motorSpeed < 1)
                {
                    motorSpeed = 1;
                }
                sendCommand(serial, MOTOR_SPEED_COMMAND, motorSpeed);
                cout << "Motor speed changed to " << motorSpeed << '.' << endl;
            }
            // Increasing x axis position when D is pressed
            else if (keyPressed == 100)
            {
                xAxisPosition += valueChange;
                if (xAxisPosition > X_AXIS_POSITION_MAX)
                {
                    xAxisPosition = X_AXIS_POSITION_MAX;
                }
                sendCommand(serial, X_AXIS_COMMAND, xAxisPosition);
                cout << "X axis position changed to " << xAxisPosition << '.' << endl;
            }
            // Decreasing x axis position when A is pressed
            else if (keyPressed == 97)
            {
                xAxisPosition -= valueChange;
                if (xAxisPosition < 0)
                {
                    xAxisPosition = 0;
                }
                sendCommand(serial, X_AXIS_COMMAND, xAxisPosition);
                cout << "X axis position changed to " << xAxisPosition << '.' << endl;
            }
            // Increasing y axis position when W is pressed
            else if (keyPressed == 119)
            {
                yAxisPosition += valueChange;
                if (yAxisPosition > Y_AXIS_POSITION_MAX)
                {
                    yAxisPosition = Y_AXIS_POSITION_MAX;
                }
                sendCommand(serial, Y_AXIS_COMMAND, yAxisPosition);
                cout << "Y axis position changed to " << yAxisPosition << '.' << endl;
            }
            // Decreasing y axis position when S is pressed
            else if (keyPressed == 115)
            {
                yAxisPosition -= valueChange;
                if (yAxisPosition < 0)
                {
                    yAxisPosition = 0;
                }
                sendCommand(serial, Y_AXIS_COMMAND, yAxisPosition);
                cout << "Y axis position changed to " << yAxisPosition << '.' << endl;
            }
            // Increasing z axis position when UP ARROW is pressed
            else if (keyPressed == 82)
            {
                zAxisPosition += valueChange;
                if (zAxisPosition > Z_AXIS_POSITION_MAX)
                {
                    zAxisPosition = Z_AXIS_POSITION_MAX;
                }
                sendCommand(serial, Z_AXIS_COMMAND, zAxisPosition);
                cout << "Z axis position changed to " << zAxisPosition << '.' << endl;
            }
            // Decreasing z axis position when DOWN ARROW is pressed
            else if (keyPressed == 84)
            {
                zAxisPosition -= valueChange;
                if (zAxisPosition < 0)
                {
                    zAxisPosition = 0;
                }
                sendCommand(serial, Z_AXIS_COMMAND, zAxisPosition);
                cout << "Z axis position changed to " << zAxisPosition << '.' << endl;
            }
            // Increasing c axis position when RIGHT ARROW is pressed
            else if (keyPressed == 83)
            {
                cAxisPosition += valueChange;
                if (cAxisPosition > C_AXIS_POSITION_MAX)
                {
                    cAxisPosition = C_AXIS_POSITION_MAX;
                }
                sendCommand(serial, C_AXIS_COMMAND, yAxisPosition);
                cout << "C axis position changed to " << cAxisPosition << '.' << endl;
            }
            // Decreasing c axis position when LEFT ARROW is pressed
            else if (keyPressed == 81)
            {
                cAxisPosition -= valueChange;
                if (cAxisPosition < 0)
                {
                    cAxisPosition = 0;
                }
                sendCommand(serial, C_AXIS_COMMAND, cAxisPosition);
                cout << "C axis position changed to " << cAxisPosition << '.' << endl;
            }
            // Increasing vacuum pump duty cycle when HOME is pressed
            else if (keyPressed == 80)
            {
                vacuumPumpDutyCyle += valueChange;
                if (vacuumPumpDutyCyle > VACUUM_PUMP_DUTY_CYCLE_MAX)
                {
                    vacuumPumpDutyCyle = VACUUM_PUMP_DUTY_CYCLE_MAX;
                }
                sendCommand(serial, VACUUM_PUMP_COMMAND, vacuumPumpDutyCyle);
                cout << "Vacuum pump duty cycle changed to " << vacuumPumpDutyCyle << '.' << endl;
            }
            // Decreasing vacuum pump duty cycle when END is pressed
            else if (keyPressed == 87)
            {
                vacuumPumpDutyCyle -= valueChange;
                if (vacuumPumpDutyCyle < 0)
                {
                    vacuumPumpDutyCyle = 0;
                }
                sendCommand(serial, VACUUM_PUMP_COMMAND, vacuumPumpDutyCyle);
                cout << "Vacuum pump duty cycle changed to " << vacuumPumpDutyCyle << '.' << endl;
            }
            // Increasing LED duty cycle when PAGE UP is pressed
            else if (keyPressed == 85)
            {
                ledDutyCyle += valueChange;
                if (ledDutyCyle > LED_DUTY_CYCLE_MAX)
                {
                    ledDutyCyle = LED_DUTY_CYCLE_MAX;
                }
                sendCommand(serial, LED_COMMAND, ledDutyCyle);
                cout << "LED duty cycle changed to " << ledDutyCyle << '.' << endl;
            }
            // Decreasing LED duty cycle when PAGE DOWN is pressed
            else if (keyPressed == 86)
            {
                ledDutyCyle -= valueChange;
                if (ledDutyCyle < 0)
                {
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
    }
    else
    {
        // Throwing an error on invalid number of command line arguments
        cout << "Wrong number of arguments. Three arguments containing the path of the camera calibration file, camera ID and serial port expected." << endl;
        cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0" << endl;
        return 1;
    }
}

// Defining text showing function
Mat showText(Mat image, string text)
{
    // Creating needed variables
    string lineBuffer;
    vector<string> lines;
    stringstream textStream(text);
    // Copying the image to buffer for non distructive drawing
    Mat imageBuffer = image.clone();
    // Splitting the text into lines and adding them one by one to the image
    while(getline(textStream, lineBuffer, '\n'))
    {
        lines.push_back(lineBuffer);
    }
    for (int i = 0; i < lines.size(); i++)
    {
        putText(imageBuffer, lines[i], Point(0, 40 * (i + 1)), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 2);
    }
    return imageBuffer;
}

// Defining command sending function
void sendCommand(SerialStream &serial, char command, int commandParameter)
{
    // Trying 3 times to send the command if necessary
    for (int numberOfTries = 0; numberOfTries < 3; numberOfTries++)
    {
        // Creating variables for storing serial input and extracted response
        string serialInput;
        char serialResponse;
        // Sending the command to the robot
        serial << command << commandParameter << endl;
        // Checking for successful execution
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE)
        {
            break;
        }
        else if (serialResponse == ERROR_MESSAGE)
        {
            cout << "Robot failed to execute the command " << command << commandParameter << '.' << endl;
            exit(1);
        }
    }
}
