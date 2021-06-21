// Including needed libraries
#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <libserial/SerialStream.h>

// Setting default namespaces
using namespace std;
using namespace cv;
using namespace LibSerial;

// Defining camera ID and serial port
const int CAMERA_ID = 0;
const string SERIAL_PORT = "/dev/ttyUSB0";

// Defining messages
const char AVAILABILITY_MESSAGE = 'R';
const char ERROR_MESSAGE = 'E';

// Declaring command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter);

int main()
{
    // Creating a camera object
    VideoCapture camera;
    // Creating camera calibration objects;
    Mat cameraMatrix;
    Mat distCoeffs;
    FileStorage cameraCalibration;
    // Creating a serial port object
    SerialStream serial;
    // Creating variables for storing serial input and extracted response
    string serialInput;
    char serialResponse;
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
    cameraCalibration["distortion_coefficients"] >> distCoeffs;
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
    namedWindow("Manual control");
    // Endless frame capturing, displaying and communcating loop
    while (true)
    {
        // Creating image containers
        Mat rawFrame;
        Mat undistortedFrame;
        // Creating variable for storing the key pressed
        int keyPressed;
        // Creating variables for track bar values;
        static int motorSpeed = 0;
        static int newMotorSpeed = 100;
        static int xAxisPosition = 0;
        static int newXAxisPosition = 0;
        static int yAxisPosition = 0;
        static int newYAxisPosition = 0;
        static int zAxisPosition = 90;
        static int newZAxisPosition = 90;
        static int cAxisPosition = 0;
        static int newCAxisPosition = 0;
        static int vacuumPumpDutyCyle = 0;
        static int newVacuumPumpDutyCyle = 0;
        static int ledDutyCyle = 0;
        static int newLedDutyCycle = 0;
        // Defining serial commands
        const char MOTOR_SPEED_COMMAND = 'S';
        const char X_AXIS_COMMAND = 'X';
        const char Y_AXIS_COMMAND = 'Y';
        const char Z_AXIS_COMMAND = 'Z';
        const char C_AXIS_COMMAND = 'C';
        const char VACUUM_PUMP_COMMAND = 'V';
        const char LED_COMMAND = 'L';
        // Reading a frame
        camera.read(rawFrame);
        // Checking for success
        if (rawFrame.empty())
        {
            cout << "Blank frame grabbed." << endl;
            return 1;
        }
        // Undistorting the frame
        undistort(rawFrame, undistortedFrame, cameraMatrix, distCoeffs);
        // Showing the frame
        imshow("Manual control", undistortedFrame);
        // Creating trackbars
        createTrackbar("Motor speed", "Manual control", &newMotorSpeed, 100);
        createTrackbar("X axis", "Manual control", &newXAxisPosition, 750);
        createTrackbar("Y axis", "Manual control", &newYAxisPosition, 750);
        createTrackbar("Z axis", "Manual control", &newZAxisPosition, 90);
        createTrackbar("C axis", "Manual control", &newCAxisPosition, 359);
        createTrackbar("Vacuum pump", "Manual control", &newVacuumPumpDutyCyle, 255);
        createTrackbar("LED", "Manual control", &newLedDutyCycle, 255);
        // Checking for motor speed value change
        if (newMotorSpeed != motorSpeed)
        {
            motorSpeed = newMotorSpeed;
            sendCommand(serial, MOTOR_SPEED_COMMAND, motorSpeed);
        }
        // Checking for x axis value change
        if (newXAxisPosition != xAxisPosition)
        {
            xAxisPosition = newXAxisPosition;
            sendCommand(serial, X_AXIS_COMMAND, xAxisPosition);
        }
        // Checking for y axis value change
        if (newYAxisPosition != yAxisPosition)
        {
            yAxisPosition = newYAxisPosition;
            sendCommand(serial, Y_AXIS_COMMAND, yAxisPosition);
        }
        // Checking for z axis value change
        if (newZAxisPosition != zAxisPosition)
        {
            zAxisPosition = newZAxisPosition;
            sendCommand(serial, Z_AXIS_COMMAND, zAxisPosition);
        }
        // Checking for c axis value change
        if (newCAxisPosition != cAxisPosition)
        {
            cAxisPosition = newCAxisPosition;
            sendCommand(serial, C_AXIS_COMMAND, cAxisPosition);
        }
        // Checking for vacuum pump duty cycle value change
        if (newVacuumPumpDutyCyle != vacuumPumpDutyCyle)
        {
            vacuumPumpDutyCyle = newVacuumPumpDutyCyle;
            sendCommand(serial, VACUUM_PUMP_COMMAND, vacuumPumpDutyCyle);
        }
        // Checking for LED duty cycle value change
        if (newLedDutyCycle != ledDutyCyle)
        {
            ledDutyCyle = newLedDutyCycle;
            sendCommand(serial, LED_COMMAND, ledDutyCyle);
        }
        // Reacting to user input
        keyPressed = waitKey(1000 / 30);
        if (keyPressed == 113)
        {
            break;
        }
        else if (keyPressed == 115)
        {
            ifstream testFile;
            static int counter = 0;
            string fileName = to_string(counter) + ".jpg";
            testFile.open(fileName);
            while (testFile.is_open())
            {
                testFile.close();
                counter++;
                fileName = to_string(counter) + ".jpg";
                testFile.open(fileName);
            }
            testFile.close();
            imwrite(fileName, undistortedFrame);
            cout << "Successfully saved picture " << fileName << '.' << endl;
        }
    }
    // Closing the camera
    camera.release();
    // Closing the serial port
    serial.Close();
    return 0;
}

// Defining command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter)
{
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
