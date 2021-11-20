// Including needed headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
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

// Declaing picture capturing function
Mat capturePicture(VideoCapture &camera, Mat cameraMatrix, Mat distortionCoefficients);

// Main function
int main(int argc, char *argv[])
{
    // Checking for the right number of command line arguments
    if (argc == 9)
    {
        // Defining camera calibration file, camera ID, serial port and window name
        const string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = stoi(argv[2]);
        const string SERIAL_PORT = argv[3];
        const string OVERVIEW_IMAGE = argv[4];
        const int COLUMN_COUNT = stoi(argv[5]);
        const int ROW_COUNT = stoi(argv[6]);
        const float WIDTH = stof(argv[7]);
        const float HEIGHT = stof(argv[8]);
        const string WINDOW_NAME = "jgswpzzlbt";
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
            "R: Indicate that the next operation can be performed\n"
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

        // Defining serial commands
        const char MOTOR_SPEED_COMMAND = 'S';
        const char X_AXIS_COMMAND = 'X';
        const char Y_AXIS_COMMAND = 'Y';
        const char Z_AXIS_COMMAND = 'Z';
        const char C_AXIS_COMMAND = 'C';
        const char VACUUM_PUMP_COMMAND = 'V';
        const char LED_COMMAND = 'L';

        sendCommand(serial, MOTOR_SPEED_COMMAND, 50);
        for (int i = 0; i < ROW_COUNT; i++)
        {
            for (int j = 0; j < COLUMN_COUNT; j++)
            {
                sendCommand(serial, X_AXIS_COMMAND, 38);
                sendCommand(serial, Y_AXIS_COMMAND, 38);
                sendCommand(serial, Z_AXIS_COMMAND, 75);
                while (true)
                {
                    Mat instruction = showText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Please lay down the next piece in the bottom left corner of the work area.\nReady?");
                    imshow(WINDOW_NAME, instruction);
                    int keyPressed = waitKey(1000 / 30);
                    // Moving forward when R is pressed
                    if (keyPressed == 114)
                    {
                        break;
                    }
                }
                sendCommand(serial, Z_AXIS_COMMAND, 0);
                sendCommand(serial, VACUUM_PUMP_COMMAND, 100);
                sleep(1);
                sendCommand(serial, Z_AXIS_COMMAND, 75);
                sendCommand(serial, X_AXIS_COMMAND, round(((750.0 / COLUMN_COUNT) * .5) + ((750.0 / COLUMN_COUNT) * j)));
                sendCommand(serial, Y_AXIS_COMMAND, round(750 - (((750.0 / ROW_COUNT) * .5) + ((750.0 / ROW_COUNT) * i))));
                sendCommand(serial, Z_AXIS_COMMAND, 0);
                sendCommand(serial, VACUUM_PUMP_COMMAND, 0);
                sleep(1);
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
        cout << "Wrong number of arguments. Eight arguments containing the path of the camera calibration file, camera ID, serial port as well as overview image, column count, row count, width and height of the jigsaw puzzle expected." << endl;
        cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0 overview.png 9 7 35.5 23" << endl;
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

// Defining picture capturing function
Mat capturePicture(VideoCapture &camera, Mat cameraMatrix, Mat distortionCoefficients)
{
    // Creating image containers
    Mat rawFrame;
    Mat undistortedFrame;
    Mat flippedFrame;
    // Reading a frame
    camera.grab();
    camera.read(rawFrame);
    // Checking for success
    if (rawFrame.empty())
    {
        cout << "Blank frame grabbed." << endl;
        exit(1);
    }
    // Undistorting the frame
    undistort(rawFrame, undistortedFrame, cameraMatrix, distortionCoefficients);
    // Flipping the frame to match the used coordinate system
    flip(undistortedFrame, flippedFrame, -1);
    return flippedFrame;
}
