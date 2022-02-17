// Including needed headers
#include <iostream>
#include <fstream>
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

// Declaring a text drawing function
Mat drawText(Mat image, string text);

// Declaring an image showing function
void showImage(String windowName, Mat image);

// Declaring a command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter);

// Declaing a picture capturing function
Mat capturePicture(VideoCapture& camera);

// Main function
int main(int argc, char** argv) {
    // Checking for the right number of command line arguments
    if (argc == 3) {
        // Defining camera ID, serial port, camera calibration file and window name
        const int CAMERA_ID = stoi(argv[1]);
        const string SERIAL_PORT = argv[2];
        const string CAMERA_CALIBRATION_FILE = argv[3];
        const string WINDOW_NAME = "jgswpzzlbt camera calibration";
        // Defining image capture related variables
        const int X_AXIS_CAPTURE_RANGE = 120;
        const int Y_AXIS_CAPTURE_RANGE = 40;
        const int X_AXIS_CAPTURE_STEP_SIZE = 20;
        const int Y_AXIS_CAPTURE_STEP_SIZE = 10;
        // Defining robot control related variables
        const int MOTOR_SPEED_MAX = 100;
        const int X_AXIS_COORDINATE_MAX = 825;
        const int X_AXIS_COORDINATE_CENTER = round((float)X_AXIS_COORDINATE_MAX / 2);
        const int Y_AXIS_COORDINATE_MAX = 725;
        const int Y_AXIS_COORDINATE_CENTER = round((float)Y_AXIS_COORDINATE_MAX / 2);
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
        // Defining calibration related variables
        int IMAGE_SIZE[2];
        const int CHECKBOARD_SIZE[] = {
            6,
            9
        };
        vector<Point3f> OBJECT_POINTS_TEMPLATE;
        for (int i = 0; i < CHECKBOARD_SIZE[1]; i++) {
            for (int j = 0; j < CHECKBOARD_SIZE[0]; j++) {
                OBJECT_POINTS_TEMPLATE.push_back(Point3f(j, i, 0));
            }
        }
        vector<vector<Point3f>> objectPoints;
        vector<vector<Point2f>> imagePoints;
        Mat cameraMatrix;
        Mat distortionCoefficients;
        FileStorage cameraCalibration;
        Mat rotationVector;
        Mat translationVector;
        // Creating a camera object
        VideoCapture camera;
        // Creating a serial port object
        SerialStream serial;
        // Creating variables for storing serial input and extracted response
        string serialInput;
        char serialResponse;
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
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));
        // Moving to the start coordinates
        sendCommand(serial, X_AXIS_COMMAND, X_AXIS_COORDINATE_CENTER);
        sendCommand(serial, Y_AXIS_COMMAND, Y_AXIS_COORDINATE_CENTER);
        cout << "Moved to start coordinates." << endl;
        // Prompting the user to align the calibration pattern
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Please lay down the calibration pattern in the center of the camera viewport.\nThe next screen will help you with the alignment."));
        // Showing a life camera feed with alignment helps
        while (true) {
            // Creating image container
            Mat frame;
            // Creating variable for storing the key pressed
            int keyPressed;
            // Capturing a frame
            frame = capturePicture(camera);
            // Drawing the alignment help
            line(frame, Point(880, 500), Point(1040, 500), Scalar(0, 0, 255), 2);
            line(frame, Point(960, 500), Point(960, 580), Scalar(0, 0, 255), 2);
            line(frame, Point(880, 580), Point(1040, 580), Scalar(0, 0, 255), 2);
            // Showing the frame
            imshow(WINDOW_NAME, frame);
            // Fetching user input
            keyPressed = waitKey(1000 / 25);
            // Quiting when Q is pressed
            if (keyPressed == 113) {
                return 0;
            }
            // Moving forward when R is pressed
            else if (keyPressed == 114) {
                break;
            }
        }
        // Telling the user that the program is capturing images
        cout << "Started the image capture." << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Image capture in progress.\nPlease wait..."));
        // Moving to the required coordinates and taking the pictures
        vector<Mat> FRAMES;
        for (int i = Y_AXIS_COORDINATE_CENTER; i < Y_AXIS_COORDINATE_CENTER + (Y_AXIS_CAPTURE_RANGE + 1); i += Y_AXIS_CAPTURE_STEP_SIZE) {
            sendCommand(serial, Y_AXIS_COMMAND, i);
            for (int j = X_AXIS_COORDINATE_CENTER; j < X_AXIS_COORDINATE_CENTER + (X_AXIS_CAPTURE_RANGE + 1); j += X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                FRAMES.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
            for (int j = X_AXIS_COORDINATE_CENTER - X_AXIS_CAPTURE_STEP_SIZE; j > X_AXIS_COORDINATE_CENTER - (X_AXIS_CAPTURE_RANGE + 1); j -= X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                FRAMES.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
        }
        for (int i = Y_AXIS_COORDINATE_CENTER - Y_AXIS_CAPTURE_STEP_SIZE; i > Y_AXIS_COORDINATE_CENTER - (Y_AXIS_CAPTURE_RANGE + 1); i -= Y_AXIS_CAPTURE_STEP_SIZE) {
            sendCommand(serial, Y_AXIS_COMMAND, i);
            for (int j = X_AXIS_COORDINATE_CENTER; j < X_AXIS_COORDINATE_CENTER + (X_AXIS_CAPTURE_RANGE + 1); j += X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                FRAMES.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
            for (int j = X_AXIS_COORDINATE_CENTER - X_AXIS_CAPTURE_STEP_SIZE; j > X_AXIS_COORDINATE_CENTER - (X_AXIS_CAPTURE_RANGE + 1); j -= X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                FRAMES.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
        }
        // Closing the camera
        camera.release();
        // Closing the serial port
        serial.Close();
        // Telling the user to look at every frame
        cout << "Started the chessboard search." << endl;
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "The next screen will show you all the captured images together with the found\nchessboard patterns."));
        // Looping over all the images and trying to find the chessboard in them
        for (int i = 0; i < FRAMES.size(); i++) {
            // Creating image containers
            Mat rawFrame;
            Mat grayFrame;
            Mat annotatedFrame;
            // Defining interal variables
            bool chessboardFound = false;
            vector<Point2f> cornerPoints;
            // Loading a picture
            rawFrame = FRAMES[i];
            // Reading it's width and height on the first iteration
            if (i == 0) {
                IMAGE_SIZE[0] = rawFrame.cols;
                IMAGE_SIZE[1] = rawFrame.rows;
            }
            // Converting it to gray scale for chessboard detection and saving a copy of it
            cvtColor(rawFrame, grayFrame, COLOR_BGR2GRAY);
            annotatedFrame = rawFrame;
            // Trying to find chessboard corners
            chessboardFound = findChessboardCornersSB(grayFrame, Size(CHECKBOARD_SIZE[0], CHECKBOARD_SIZE[1]), cornerPoints);
            // Checking for success
            if (chessboardFound) {
                cout << "Algorithm detected the chessboard on image " << i << '.' << endl;
                // Annotating the current frame
                drawChessboardCorners(annotatedFrame, Size(CHECKBOARD_SIZE[0], CHECKBOARD_SIZE[1]), cornerPoints, chessboardFound);
                // Adding the extracted data to the lists
                objectPoints.push_back(OBJECT_POINTS_TEMPLATE);
                imagePoints.push_back(cornerPoints);
            } else {
                cout << "Algorithm failed to detect the chessboard on image " << i << '.' << endl;
            }
            // Showing the current frame
            showImage(WINDOW_NAME, annotatedFrame);
        }
        // Telling the user that the program is calculating the calibraton
        cout << "Started the calibration." << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Camera calibration in progress.\nPlease wait..."));
        // Performing the calibration
        calibrateCamera(objectPoints, imagePoints, Size(IMAGE_SIZE[0], IMAGE_SIZE[1]), cameraMatrix, distortionCoefficients, rotationVector, translationVector);
        cout << "Camera matrix: " << cameraMatrix << endl;
        cout << "Distortion coefficients : " << distortionCoefficients << endl;
        // Opening the calibration file
        cameraCalibration.open(CAMERA_CALIBRATION_FILE, FileStorage::WRITE);
        // Writing the calibration to the file
        cameraCalibration << "camera-matrix" << cameraMatrix;
        cameraCalibration << "distortion-coefficients" << distortionCoefficients;
        // Closing the calibration file
        cameraCalibration.release();
        return 0;
    } else {
        // Throwing an error on invalid number of command line arguments
        cout << "Wrong number of arguments. Two arguments containing the camera ID and serial port expected." << endl;
        cout << "Example: " << argv[0] << " 0 /dev/ttyUSB0" << endl;
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

// Defining picture capturing function
Mat capturePicture(VideoCapture& camera) {
    // Creating image containers
    Mat rawFrame;
    Mat blurredFrame;
    // Reading a frame
    camera.grab();
    camera.read(rawFrame);
    // Checking for success
    if (rawFrame.empty()) {
        cout << "Blank frame grabbed." << endl;
        exit(1);
    }
    // Blurring the frame
    bilateralFilter(rawFrame, blurredFrame, 8, 64, 64);
    return blurredFrame;
}
