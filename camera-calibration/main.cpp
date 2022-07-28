#include <iostream>
#include <unistd.h>
#include "lib/opencv-helpers.hpp"
#include "lib/libserial-helpers.hpp"

// Main function
int main(int argc, char** argv) {
    // Check for the right number of command line arguments
    if (argc == 4) {
        // Variables for command line arguments
        const int CAMERA_ID = std::stoi(argv[1]);
        const std::string SERIAL_PORT = argv[2];
        const std::string CAMERA_CALIBRATION_FILE = argv[3];
        const std::string WINDOW_NAME = "jgswpzzlbt camera calibration";
        // Robot axis maximums
        const int X_AXIS_MAX_COORDINATE = 825;
        const int Y_AXIS_MAX_COORDINATE = 725;
        // Objects for robot communication
        cv::VideoCapture camera;
        LibSerial::SerialStream serial;
        std::string serialInput;
        char serialResponse;
        // Calibration related variables
        const int X_AXIS_CAPTURE_RANGE = 120;
        const int Y_AXIS_CAPTURE_RANGE = 40;
        const int X_AXIS_CAPTURE_STEP_SIZE = 20;
        const int Y_AXIS_CAPTURE_STEP_SIZE = 10;
        const int CHESSBOARD_SIZE[] = {6, 9};
        const int X_AXIS_CENTER_COORDINATE = round(X_AXIS_MAX_COORDINATE / 2.0);
        const int Y_AXIS_CENTER_COORDINATE = round(Y_AXIS_MAX_COORDINATE / 2.0);
        std::vector<cv::Mat> calibrationImages;
        std::vector<cv::Point3f> objectPointsTemplate;
        for (int i = 0; i < CHESSBOARD_SIZE[1]; i++) {
            for (int j = 0; j < CHESSBOARD_SIZE[0]; j++) {
                objectPointsTemplate.push_back(cv::Point3f(j, i, 0));
            }
        }
        std::vector<std::vector<cv::Point3f>> objectPoints;
        std::vector<std::vector<cv::Point2f>> imagePoints;
        cv::Mat cameraMatrix;
        cv::Mat distortionCoefficients;
        cv::FileStorage cameraCalibration;
        cv::Mat rotationVector;
        cv::Mat translationVector;
        // Initialize robot
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
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE) {
            std::cout << "Robot homed successfully." << std::endl;
        } else {
            std::cout << "Homing of the robot failed." << std::endl;
            return 1;
        }
        // Display a small help at start
        cv::namedWindow(WINDOW_NAME);
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));
        // Moving to the start coordinates
        moveTo(serial, X_AXIS_CENTER_COORDINATE, Y_AXIS_CENTER_COORDINATE);
        std::cout << "Moved to start coordinates." << std::endl;
        // Prompt the user to align the calibration pattern
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Please lay down the calibration pattern in the center of the camera viewport.\nThe next screen will help you with the alignment..."));
        // Show a life camera feed with alignment helps
        while (true) {
            // Image containers and variable for storing the key pressed
            cv::Mat rawFrame;
            cv::Mat resizedFrame;
            int keyPressed;
            // Show a frame
            rawFrame = capturePicture(camera);
            cv::resize(rawFrame, resizedFrame, cv::Size(1280, 720));
            cv::line(resizedFrame, cv::Point(620, 335), cv::Point(660, 335), cv::Scalar(0, 0, 255), 1.5);
            cv::line(resizedFrame, cv::Point(640, 335), cv::Point(640, 385), cv::Scalar(0, 0, 255), 1.5);
            cv::line(resizedFrame, cv::Point(620, 385), cv::Point(660, 385), cv::Scalar(0, 0, 255), 1.5);
            cv::imshow(WINDOW_NAME, resizedFrame);
            // Fetch user input
            keyPressed = cv::waitKey(1000 / 25);
            // Quit when Q is pressed
            if (keyPressed == 113) {
                return 0;
            }
            // Move forward when R is pressed
            else if (keyPressed == 114) {
                break;
            }
        }
        // Move to the required coordinates and take the calibration pictures
        std::cout << "Started the image capture." << std::endl;
        cv::imshow(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Image capture in progress. Please wait..."));
        cv::waitKey(1000);
        for (int i = Y_AXIS_CENTER_COORDINATE; i < Y_AXIS_CENTER_COORDINATE + (Y_AXIS_CAPTURE_RANGE + 1); i += Y_AXIS_CAPTURE_STEP_SIZE) {
            sendCommand(serial, Y_AXIS_COMMAND, i);
            for (int j = X_AXIS_CENTER_COORDINATE; j < X_AXIS_CENTER_COORDINATE + (X_AXIS_CAPTURE_RANGE + 1); j += X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                std::cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << std::endl;
            }
            for (int j = X_AXIS_CENTER_COORDINATE - X_AXIS_CAPTURE_STEP_SIZE; j > X_AXIS_CENTER_COORDINATE - (X_AXIS_CAPTURE_RANGE + 1); j -= X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                std::cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << std::endl;
            }
        }
        for (int i = Y_AXIS_CENTER_COORDINATE - Y_AXIS_CAPTURE_STEP_SIZE; i > Y_AXIS_CENTER_COORDINATE - (Y_AXIS_CAPTURE_RANGE + 1); i -= Y_AXIS_CAPTURE_STEP_SIZE) {
            sendCommand(serial, Y_AXIS_COMMAND, i);
            for (int j = X_AXIS_CENTER_COORDINATE; j < X_AXIS_CENTER_COORDINATE + (X_AXIS_CAPTURE_RANGE + 1); j += X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                std::cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << std::endl;
            }
            for (int j = X_AXIS_CENTER_COORDINATE - X_AXIS_CAPTURE_STEP_SIZE; j > X_AXIS_CENTER_COORDINATE - (X_AXIS_CAPTURE_RANGE + 1); j -= X_AXIS_CAPTURE_STEP_SIZE) {
                sendCommand(serial, X_AXIS_COMMAND, j);
                sleep(1);
                calibrationImages.push_back(capturePicture(camera));
                std::cout << "Captured a picture at x coordinate " << j << " and y coordinate " << i << '.' << std::endl;
            }
        }
        // Close the robot connection
        camera.release();
        serial.Close();
        // Loop over all the images and try to find the chessboard in them
        std::cout << "Started the chessboard search." << std::endl;
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Chessboard search in progress. Please wait..."));
        for (int i = 0; i < calibrationImages.size(); i++) {
            // Image containers and other internal variables
            cv::Mat rawFrame;
            cv::Mat grayFrame;
            bool chessboardFound = false;
            std::vector<cv::Point2f> cornerPoints;
            // Load and preprocess a picture
            rawFrame = calibrationImages[i];
            cv::cvtColor(rawFrame, grayFrame, cv::COLOR_BGR2GRAY);
            // Actually try to find chessboard corners
            chessboardFound = cv::findChessboardCornersSB(grayFrame, cv::Size(CHESSBOARD_SIZE[0], CHESSBOARD_SIZE[1]), cornerPoints);
            if (chessboardFound) {
                std::cout << "Algorithm detected the chessboard on image " << i << '.' << std::endl;
                objectPoints.push_back(objectPointsTemplate);
                imagePoints.push_back(cornerPoints);
            } else {
                std::cout << "Algorithm failed to detect the chessboard on image " << i << '.' << std::endl;
            }
        }
        // Perform the calibration
        std::cout << "Started the calibration." << std::endl;
        cv::imshow(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Camera calibration in progress. Please wait..."));
        cv::waitKey(1000);
        cv::calibrateCamera(objectPoints, imagePoints, cv::Size(calibrationImages[0].cols, calibrationImages[0].rows), cameraMatrix, distortionCoefficients, rotationVector, translationVector);
        std::cout << "Camera matrix: " << cameraMatrix << std::endl;
        std::cout << "Distortion coefficients : " << distortionCoefficients << std::endl;
        // Write the calibration to file
        cameraCalibration.open(CAMERA_CALIBRATION_FILE, cv::FileStorage::WRITE);
        cameraCalibration << "camera-matrix" << cameraMatrix;
        cameraCalibration << "distortion-coefficients" << distortionCoefficients;
        cameraCalibration.release();
        // Close the program
        std::cout << "Finished the camera calibration!" << std::endl;
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Finished the camera calibration!"));
        return 0;
    } else {
        // Throw an error on invalid number of command line arguments
        std::cout << "Wrong number of arguments. Three arguments containing the camera ID, serial port and name of camera calibration file expected." << std::endl;
        std::cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0 camera-calibration.xml" << std::endl;
        return 1;
    }
}
