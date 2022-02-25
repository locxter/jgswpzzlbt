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

// Defining image capture related variables
const int X_AXIS_CAPTURE_RANGE = 120;
const int Y_AXIS_CAPTURE_RANGE = 40;
const int X_AXIS_CAPTURE_STEP_SIZE = 20;
const int Y_AXIS_CAPTURE_STEP_SIZE = 10;

// Defining chessboard size
const int CHESSBOARD_SIZE[] = {
            6,
            9
};

// Declaring a text drawing function
Mat drawText(Mat image, string text);

// Declaring an image showing function
void showImage(String windowName, Mat image);

// Declaing a picture capturing function
Mat capturePicture(VideoCapture& camera);

// Declaring a command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter);

// Main function
int main(int argc, char** argv) {
    // Checking for the right number of command line arguments
    if (argc == 4) {
        // Storing commandline arguments and program name
        const int CAMERA_ID = stoi(argv[1]);
        const string SERIAL_PORT = argv[2];
        const string CAMERA_CALIBRATION_FILE = argv[3];
        const string WINDOW_NAME = "jgswpzzlbt camera calibration";
        // Defining calibration related variables
        const int X_AXIS_CENTER_COORDINATE = round(X_AXIS_MAX_COORDINATE / 2.0);
        const int Y_AXIS_CENTER_COORDINATE = round(Y_AXIS_MAX_COORDINATE / 2.0);
        vector<Mat> calibrationImages;
        vector<Point3f> objectPointsTemplate;
        for (int i = 0; i < CHESSBOARD_SIZE[1]; i++) {
            for (int j = 0; j < CHESSBOARD_SIZE[0]; j++) {
                objectPointsTemplate.push_back(Point3f(j, i, 0));
            }
        }
        vector<vector<Point3f>> objectPoints;
        vector<vector<Point2f>> imagePoints;
        Mat cameraMatrix;
        Mat distortionCoefficients;
        FileStorage cameraCalibration;
        Mat rotationVector;
        Mat translationVector;
        // Creating objects for robot communication
        VideoCapture camera;
        SerialStream serial;
        string serialInput;
        char serialResponse;
        // Initializing robot
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
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));
        // Moving to the start coordinates
        sendCommand(serial, X_AXIS_COMMAND, X_AXIS_CENTER_COORDINATE);
        sendCommand(serial, Y_AXIS_COMMAND, Y_AXIS_CENTER_COORDINATE);
        cout << "Moved to start coordinates." << endl;
        // Prompting the user to align the calibration pattern
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Please lay down the calibration pattern in the center of the camera viewport.\nThe next screen will help you with the alignment."));
        // Showing a life camera feed with alignment helps
        while (true) {
            // Creating image containers
            Mat rawFrame;
            Mat resizedFrame;
            // Creating variable for storing the key pressed
            int keyPressed;
            // Showing a frame
            rawFrame = capturePicture(camera);
            resize(rawFrame, resizedFrame, Size(1280, 720));
            line(resizedFrame, Point(620, 335), Point(660, 335), Scalar(0, 0, 255), 1.5);
            line(resizedFrame, Point(640, 335), Point(640, 385), Scalar(0, 0, 255), 1.5);
            line(resizedFrame, Point(620, 385), Point(660, 385), Scalar(0, 0, 255), 1.5);
            imshow(WINDOW_NAME, resizedFrame);
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
        // Moving to the required coordinates and taking the pictures
        cout << "Started the image capture." << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Image capture in progress. Please wait..."));
        for (int i = Y_AXIS_CENTER_COORDINATE; i < Y_AXIS_CENTER_COORDINATE + (Y_AXIS_CAPTURE_RANGE + 1); i += Y_AXIS_CAPTURE_STEP_SIZE) {
            sendCommand(serial, Y_AXIS_COMMAND, i);
            for (int j = X_AXIS_CENTER_COORDINATE; j < X_AXIS_CENTER_COORDINATE + (X_AXIS_CAPTURE_RANGE + 1); j += X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
            for (int j = X_AXIS_CENTER_COORDINATE - X_AXIS_CAPTURE_STEP_SIZE; j > X_AXIS_CENTER_COORDINATE - (X_AXIS_CAPTURE_RANGE + 1); j -= X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
        }
        for (int i = Y_AXIS_CENTER_COORDINATE - Y_AXIS_CAPTURE_STEP_SIZE; i > Y_AXIS_CENTER_COORDINATE - (Y_AXIS_CAPTURE_RANGE + 1); i -= Y_AXIS_CAPTURE_STEP_SIZE) {
            sendCommand(serial, Y_AXIS_COMMAND, i);
            for (int j = X_AXIS_CENTER_COORDINATE; j < X_AXIS_CENTER_COORDINATE + (X_AXIS_CAPTURE_RANGE + 1); j += X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
            for (int j = X_AXIS_CENTER_COORDINATE - X_AXIS_CAPTURE_STEP_SIZE; j > X_AXIS_CENTER_COORDINATE - (X_AXIS_CAPTURE_RANGE + 1); j -= X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << endl;
            }
        }
        // Closing the robot connection
        camera.release();
        serial.Close();
        // Looping over all the images and trying to find the chessboard in them
        cout << "Started the chessboard search." << endl;
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Chessboard search in progress. Please wait..."));
        for (int i = 0; i < calibrationImages.size(); i++) {
            // Creating image containers
            Mat rawFrame;
            Mat grayFrame;
            // Defining internal variables
            bool chessboardFound = false;
            vector<Point2f> cornerPoints;
            // Loading and preprocessing a picture
            rawFrame = calibrationImages[i];
            cvtColor(rawFrame, grayFrame, COLOR_BGR2GRAY);
            // Trying to find chessboard corners
            chessboardFound = findChessboardCornersSB(grayFrame, Size(CHESSBOARD_SIZE[0], CHESSBOARD_SIZE[1]), cornerPoints);
            if (chessboardFound) {
                cout << "Algorithm detected the chessboard on image " << i << '.' << endl;
                objectPoints.push_back(objectPointsTemplate);
                imagePoints.push_back(cornerPoints);
            } else {
                cout << "Algorithm failed to detect the chessboard on image " << i << '.' << endl;
            }
        }
        // Performing the calibration
        cout << "Started the calibration." << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Camera calibration in progress. Please wait..."));
        calibrateCamera(objectPoints, imagePoints, Size(calibrationImages[0].cols, calibrationImages[0].rows), cameraMatrix, distortionCoefficients, rotationVector, translationVector);
        cout << "Camera matrix: " << cameraMatrix << endl;
        cout << "Distortion coefficients : " << distortionCoefficients << endl;
        // Writing the calibration to file
        cameraCalibration.open(CAMERA_CALIBRATION_FILE, FileStorage::WRITE);
        cameraCalibration << "camera-matrix" << cameraMatrix;
        cameraCalibration << "distortion-coefficients" << distortionCoefficients;
        cameraCalibration.release();
        cout << "Finished the camera calibration!" << endl;
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Finished the camera calibration!"));
        return 0;
    } else {
        // Throwing an error on invalid number of command line arguments
        cout << "Wrong number of arguments. Three arguments containing the camera ID, serial port and name of camera calibration file expected." << endl;
        cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0 camera-calibration.xml" << endl;
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
Mat capturePicture(VideoCapture& camera) {
    Mat rawFrame;
    Mat blurredFrame;
    // Cleaing the buffer and reading a frame
    camera.grab();
    camera.read(rawFrame);
    if (rawFrame.empty()) {
        cout << "Blank frame grabbed." << endl;
        exit(1);
    }
    // Blurring the frame
    bilateralFilter(rawFrame, blurredFrame, 8, 64, 64);
    return blurredFrame;
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