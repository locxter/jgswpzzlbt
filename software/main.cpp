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
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients);

// Main function
int main(int argc, char** argv) {
    // Checking for the right number of command line arguments
    if (argc == 9) {
        // Defining camera calibration file, camera ID, serial port, reference image, jigsaw puzzle column, row and part count as well as width and height and window name
        const string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = stoi(argv[2]);
        const string SERIAL_PORT = argv[3];
        const string REFERENCE_IMAGE = argv[4];
        const int COLUMN_COUNT = stoi(argv[5]);
        const int ROW_COUNT = stoi(argv[6]);
        const int PART_COUNT = COLUMN_COUNT * ROW_COUNT;
        const int WIDTH = stof(argv[7]);
        const int HEIGHT = stof(argv[8]);
        const string WINDOW_NAME = "jgswpzzlbt";
        // Defining robot control related variables
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
        const int X_TOOL_OFFSET = 0;
        const int Y_TOOL_OFFSET = 50;
        // Defining part storage related variables
        const int STORAGE_COLUMN_WIDTH = round((((float)WIDTH / COLUMN_COUNT) * 2));
        const int STORAGE_COLUMN_COUNT = floor((float)X_AXIS_COORDINATE_MAX / STORAGE_COLUMN_WIDTH);
        const int STORAGE_ROW_HEIGHT = round((((float)HEIGHT / ROW_COUNT) * 2));
        const int STORAGE_ROW_COUNT = ceil((float)PART_COUNT / STORAGE_COLUMN_COUNT);
        vector<vector<int>> PARTS_STORAGE_COORDINATES;
        // Defining part matching related variables
        vector<vector<Mat>> PARTS_MATCHING_IMAGES;
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
        // Filling the part storage coordinates with data
        for (int i = 0; i < STORAGE_ROW_COUNT; i++) {
            for (int j = 0; j < STORAGE_COLUMN_COUNT; j++) {
                vector<int> PART_STORAGE_COORDINATES;
                if (PARTS_STORAGE_COORDINATES.size() == PART_COUNT) {
                    break;
                }
                PART_STORAGE_COORDINATES.push_back(floor((STORAGE_COLUMN_WIDTH * .5) + (STORAGE_COLUMN_WIDTH * j)));
                PART_STORAGE_COORDINATES.push_back(floor(Y_AXIS_COORDINATE_MAX - Y_TOOL_OFFSET - (((float)STORAGE_ROW_HEIGHT * .5) + (STORAGE_ROW_HEIGHT * i))));
                PARTS_STORAGE_COORDINATES.push_back(PART_STORAGE_COORDINATES);
            }
        }
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
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));

        // Setting the speed to 3/4 throttle for testing
        // sendCommand(serial, MOTOR_SPEED_COMMAND, 75);

        // Moving all parts to their storage coordinates
        for (int i = 0; i < PART_COUNT; i++) {
            // Defining variables for pickup coordinates and position adjustments
            const int X_PICKUP_COORDINATE = 120;
            const int Y_PICKUP_COORDINATE = 80;
            int X_COORDINATE_ADJUSTMENT = 0;
            int Y_COORDINATE_ADJUSTMENT = 0;
            int C_COORDINATE_ADJUSTMENT = 0;
            // Creating image containers
            Mat rawFrame;
            Mat grayFrame;
            Mat preprocessedFrame;
            Mat cannyFrame;
            Mat processedFrame;
            // Creating image processing variables
            vector<vector<Point>> contours;
            RotatedRect minRect;
            vector<Point> minRectContour;
            Scalar color = Scalar(0, 255, 0);
            Point2f minRectPoints[4];

            vector<Mat> PART_MATCHING_IMAGES;
            Rect boundRect;
            vector<Point> boundRectContour;
            Mat croppedFrame;

            // Moving the head to the pickup coordinates
            sendCommand(serial, X_AXIS_COMMAND, X_PICKUP_COORDINATE);
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP_COORDINATE);
            cout << "Moved the pickup coordinates successfully" << endl;
            // Prompting the user to lay down a jigsaw puzzle part
            showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Please lay down the next piece in the bottom left corner of the work area."));
            // Capturing and preprocessing a picture of the part for position correction
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cout << "Captured part image successfully." << endl;
            showImage(WINDOW_NAME, rawFrame);
            cvtColor(rawFrame, grayFrame, COLOR_BGR2GRAY);
            bilateralFilter(grayFrame, preprocessedFrame, 8, 64, 64);
            threshold(preprocessedFrame, preprocessedFrame, 0, 255, THRESH_OTSU);
            // Performing canny edge detection
            Canny(preprocessedFrame, cannyFrame, 128, 255);
            // Finding contours and selecting only the largest one together with it's minimum bounding rectangle
            findContours(cannyFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (int j = 0; j < contours.size(); j++) {
                RotatedRect buffer = minAreaRect(contours[j]);
                static float largestArea = 0;
                if (buffer.size.area() > largestArea) {
                    minRect = buffer;
                    minRectContour = contours[j];
                    largestArea = buffer.size.area();
                }
            }
            cout << "Contour detection finished successfully." << endl;
            // Creating and showing the processed frame
            processedFrame = Mat::zeros(cannyFrame.size(), CV_8UC3);
            drawContours(processedFrame, vector<vector<Point>>(1, minRectContour), 0, color);
            minRect.points(minRectPoints);
            for (int j = 0; j < 4; j++) {
                line(processedFrame, minRectPoints[j], minRectPoints[(j + 1) % 4], color);
            }
            circle(processedFrame, minRect.center, 4, color, -1);
            showImage(WINDOW_NAME, processedFrame);
            // Extracting the important information
            X_COORDINATE_ADJUSTMENT = round(((minRect.center.x - 960) / 8) + X_TOOL_OFFSET);
            Y_COORDINATE_ADJUSTMENT = round(((540 - minRect.center.y) / 8) - Y_TOOL_OFFSET);
            C_COORDINATE_ADJUSTMENT = round(abs(minRect.angle));
            cout << "X adjustment: " << X_COORDINATE_ADJUSTMENT << endl;
            cout << "Y adjustment: " << Y_COORDINATE_ADJUSTMENT << endl;
            cout << "C adjustment: " << C_COORDINATE_ADJUSTMENT << endl;
            // Moving the head to the adjusted coordinates
            sendCommand(serial, X_AXIS_COMMAND, X_PICKUP_COORDINATE + X_COORDINATE_ADJUSTMENT);
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP_COORDINATE + Y_COORDINATE_ADJUSTMENT);
            // Picking the part up
            sendCommand(serial, Z_AXIS_COMMAND, 0);
            sendCommand(serial, VACUUM_PUMP_COMMAND, 100);
            sleep(1);
            sendCommand(serial, Z_AXIS_COMMAND, Z_AXIS_COORDINATE_MAX);
            sleep(1);
            // Applying the c axis adjustment
            sendCommand(serial, C_AXIS_COMMAND, C_COORDINATE_ADJUSTMENT);
            sleep(1);
            // Moving to the part's storage coordinates
            sendCommand(serial, X_AXIS_COMMAND, PARTS_STORAGE_COORDINATES[i][0]);
            sendCommand(serial, Y_AXIS_COMMAND, PARTS_STORAGE_COORDINATES[i][1]);
            // Releasing the part
            sendCommand(serial, VACUUM_PUMP_COMMAND, 0);
            sleep(1);
            cout << "Moved the part to it's storage location successfully." << endl;
            // Moving the c axis back to it's start position and the head out of the picture
            sendCommand(serial, C_AXIS_COMMAND, 0);
            sendCommand(serial, Y_AXIS_COMMAND, PARTS_STORAGE_COORDINATES[i][1] + Y_TOOL_OFFSET);
            sleep(1);
            // Capturing and preprocessing a picture of the part for part matching
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cout << "Captured part image successfully." << endl;
            showImage(WINDOW_NAME, rawFrame);
            cvtColor(rawFrame, grayFrame, COLOR_BGR2GRAY);
            bilateralFilter(grayFrame, preprocessedFrame, 8, 64, 64);
            threshold(preprocessedFrame, preprocessedFrame, 0, 255, THRESH_OTSU);
            // Performing canny edge detection
            Canny(preprocessedFrame, cannyFrame, 128, 255);
            // Finding contours and selecting only the largest one together with it's bounding rectangle
            findContours(cannyFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (int j = 0; j < contours.size(); j++) {
                Rect buffer = boundingRect(contours[j]);
                static float largestArea = 0;
                if (buffer.area() > largestArea) {
                    boundRect = buffer;
                    boundRectContour = contours[j];
                    largestArea = buffer.area();
                }
            }
            cout << "Contour detection finished successfully." << endl;
            // Creating and showing the processed frame
            processedFrame = Mat::zeros(cannyFrame.size(), CV_8UC3);
            drawContours(processedFrame, vector<vector<Point>>(1, boundRectContour), 0, color);
            rectangle(processedFrame, boundRect.tl(), boundRect.br(), color);
            showImage(WINDOW_NAME, processedFrame);
            // Cropping the frame and storing it for future processing
            croppedFrame = rawFrame(Range(boundRect.tl().y, boundRect.tl().y + boundRect.height), Range(boundRect.tl().x, boundRect.tl().x + boundRect.width));
            PART_MATCHING_IMAGES.push_back(croppedFrame);
            showImage(WINDOW_NAME, croppedFrame);
            // Rotating the cropped frame to accomodate all possible alignments and storing the results as well
            for (int i = 1; i < 4; i++) {
                Mat rotatedCroppedFrame;
                Point2f centerPoint = Point2f(croppedFrame.size().width / 2.0, croppedFrame.size().height / 2.0);
                Mat rotationMatix = getRotationMatrix2D(centerPoint, 90 * i, 1.0);
                Rect2f boundBox = RotatedRect(cv::Point2f(), croppedFrame.size(), 90 * i).boundingRect2f();
                rotationMatix.at<double>(0, 2) += (boundBox.width / 2.0) - (croppedFrame.size().width / 2.0);
                rotationMatix.at<double>(1, 2) += (boundBox.height / 2.0) - (croppedFrame.size().height / 2.0);
                warpAffine(croppedFrame, rotatedCroppedFrame, rotationMatix, boundBox.size());
                PART_MATCHING_IMAGES.push_back(rotatedCroppedFrame);
                showImage(WINDOW_NAME, rotatedCroppedFrame);
            }
            PARTS_MATCHING_IMAGES.push_back(PART_MATCHING_IMAGES);
        }
        // Closing the camera
        camera.release();
        // Closing the serial port
        serial.Close();
        return 0;
    } else {
        // Throwing an error on invalid number of command line arguments
        cout << "Wrong number of arguments. Eight arguments containing the path of the camera calibration file, camera ID, serial port as well as reference image, column count, row count, width and height in mm of the jigsaw puzzle expected." << endl;
        cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0 reference.png 9 7 355 230" << endl;
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