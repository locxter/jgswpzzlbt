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
const char MOTOR_SPEED_COMMAND = 'S';
const char X_AXIS_COMMAND = 'X';
const char Y_AXIS_COMMAND = 'Y';
const char Z_AXIS_COMMAND = 'Z';
const char C_AXIS_COMMAND = 'C';
const char VACUUM_PUMP_COMMAND = 'V';
const char LED_COMMAND = 'L';

// Defining robot control maximums
const int MOTOR_SPEED_MAX = 100;
const int X_AXIS_COORDINATE_MIN = 50;
const int X_AXIS_COORDINATE_MAX = 825;
const int Y_AXIS_COORDINATE_MAX = 725;
const int Z_AXIS_COORDINATE_MAX = 90;
const int VACUUM_PUMP_DUTY_CYCLE_MAX = 100;
const int LED_DUTY_CYCLE_MAX = 100;

// Declaring a text drawing function
Mat drawText(Mat image, string text);

// Declaring an image showing function
void showImage(String windowName, Mat image);

// Declaing a picture capturing function
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients);

// Declaring a command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter);

// Declaring helper functions for robot control
void moveTo(SerialStream& serial, int xCoordinate, int yCoordinate);
void pickPartFromPuzzleMat(SerialStream& serial);
void pickPartFromTurntable(SerialStream& serial);
void releasePartToPuzzleMat(SerialStream& serial);
void releasePartToTurntable(SerialStream& serial);
void rotatePart(SerialStream& serial, int angle);

// Main function
int main(int argc, char** argv) {
    // Checking for the right number of command line arguments
    if (argc == 9) {
        // Storing commandline arguments
        const string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = stoi(argv[2]);
        const string SERIAL_PORT = argv[3];
        const string REFERENCE_IMAGE_FILE = argv[4];
        const int COLUMN_COUNT = stoi(argv[5]);
        const int ROW_COUNT = stoi(argv[6]);
        const int PART_COUNT = COLUMN_COUNT * ROW_COUNT;
        const int WIDTH = stoi(argv[7]);
        const int HEIGHT = stoi(argv[8]);
        const string WINDOW_NAME = "jgswpzzlbt";
        // Creating objects for robot communcation
        VideoCapture camera;
        Mat cameraMatrix;
        Mat distortionCoefficients;
        FileStorage cameraCalibration;
        SerialStream serial;
        string serialInput;
        char serialResponse;
        // Defining tool offset
        const int X_TOOL_OFFSET = 0;
        const int Y_TOOL_OFFSET = 80;
        // Defining part storage related stuff
        const int STORAGE_COLUMN_WIDTH = round((((float)WIDTH / COLUMN_COUNT) * 2));
        const int STORAGE_COLUMN_COUNT = floor(((float)X_AXIS_COORDINATE_MAX - X_AXIS_COORDINATE_MIN) / STORAGE_COLUMN_WIDTH);
        const int STORAGE_ROW_HEIGHT = round((((float)HEIGHT / ROW_COUNT) * 2));
        const int STORAGE_ROW_COUNT = ceil((float)PART_COUNT / STORAGE_COLUMN_COUNT);
        vector<vector<int>> storageCoordinates;
        // Defining puzzle solving related stuff
        Mat referenceImage = imread(argv[1], IMREAD_COLOR);
        vector<vector<Mat>> solvingImages;
        vector<vector<int>> solvingResults;
        // Defining puzzle assembly related variables
        const int ASSEMBLY_COLUMN_WIDTH = round((((float)WIDTH / COLUMN_COUNT) * 1.25));
        const int ASSEMBLY_ROW_HEIGHT = round((((float)HEIGHT / ROW_COUNT) * 1.25));
        vector<vector<vector<int>>> assemblyCoordinates;
        // Filling the storage and assembly coordinates with data
        for (int i = 0; i < STORAGE_ROW_COUNT; i++) {
            for (int j = 0; j < STORAGE_COLUMN_COUNT; j++) {
                vector<int> partCoordinates;
                if (storageCoordinates.size() == PART_COUNT) {
                    break;
                }
                partCoordinates.push_back(floor((STORAGE_COLUMN_WIDTH * .5) + (STORAGE_COLUMN_WIDTH * j) + X_AXIS_COORDINATE_MIN));
                partCoordinates.push_back(floor(Y_AXIS_COORDINATE_MAX - (((float)STORAGE_ROW_HEIGHT * .5) + (STORAGE_ROW_HEIGHT * i))));
                storageCoordinates.push_back(partCoordinates);
            }
        }
        for (int i = 0; i < ROW_COUNT; i++) {
            vector<vector<int>> rowCoordinates;
            for (int j = 0; j < COLUMN_COUNT; j++) {
                vector<int> partCoordinates;
                partCoordinates.push_back(floor((ASSEMBLY_COLUMN_WIDTH * .5) + (ASSEMBLY_COLUMN_WIDTH * j) + X_AXIS_COORDINATE_MIN));
                partCoordinates.push_back(floor(((float)STORAGE_ROW_HEIGHT * .5) + (STORAGE_ROW_HEIGHT * i)));
                rowCoordinates.push_back(partCoordinates);
            }
            assemblyCoordinates.push_back(rowCoordinates);
        }
        // Initializing robot communication
        cameraCalibration.open("camera-calibration.xml", FileStorage::READ);
        if (cameraCalibration.isOpened()) {
            cout << "Opened camera calibration successfully" << endl;
        } else {
            cout << "Failed to open camera calibration" << endl;
            return 1;
        }
        cameraCalibration["camera-matrix"] >> cameraMatrix;
        cameraCalibration["distortion-coefficients"] >> distortionCoefficients;
        cameraCalibration.release();
        camera.open(CAMERA_ID, CAP_V4L);
        if (camera.isOpened()) {
            cout << "Opened camera successfully" << endl;
        } else {
            cout << "Failed to open camera" << endl;
            return 1;
        }
        camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
        camera.set(CAP_PROP_FRAME_WIDTH, 1920);
        camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
        camera.set(CAP_PROP_FPS, 25);
        camera.set(CAP_PROP_BUFFERSIZE, 1);
        serial.Open(SERIAL_PORT);
        if (serial.IsOpen()) {
            cout << "Opened serial port successfully" << endl;
        } else {
            cout << "Unable to open serial port" << endl;
        }
        serial.SetBaudRate(BaudRate::BAUD_115200);
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE) {
            cout << "Robot homed successfully" << endl;
        } else {
            cout << "Homing of the robot failed" << endl;
            return 1;
        }
        // Displaying a small help at startup
        namedWindow(WINDOW_NAME);
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));
        // Moving all parts to their storage position and capturing needed images on the way
        for (int i = 0; i < PART_COUNT; i++) {
            // Defining variables for pickup coordinates and position adjustments
            const int X_PICKUP_COORDINATE = X_AXIS_COORDINATE_MIN + 75;
            const int Y_PICKUP_COORDINATE = 75;
            int xAdjustment = 0;
            int yAdjustment = 0;
            int cAdjustment = 0;
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
            Scalar color = Scalar(255, 255, 255);
            Point2f minRectPoints[4];
            vector<Mat> partSolvingImages;
            Rect boundRect;
            vector<Point> boundRectContour;
            Mat croppedFrame;
            // Moving the head to the pickup coordinates
            moveTo(serial, X_PICKUP_COORDINATE, Y_PICKUP_COORDINATE);
            cout << "Moved the pickup coordinates successfully" << endl;
            // Prompting the user to lay down a jigsaw puzzle part
            showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Please lay down the next piece in the bottom left corner of the work area"));
            // Capturing and preprocessing a picture of the part for position correction
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cout << "Captured part image successfully" << endl;
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
            cout << "Contour detection finished successfully" << endl;
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
            xAdjustment = round(((minRect.center.x - 960) / 8) + X_TOOL_OFFSET);
            yAdjustment = round(((540 - minRect.center.y) / 8) - Y_TOOL_OFFSET);
            cAdjustment = round(90 - minRect.angle);
            cout << "X adjustment: " << xAdjustment << endl;
            cout << "Y adjustment: " << yAdjustment << endl;
            cout << "C adjustment: " << cAdjustment << endl;
            // Picking the part from the adjusted position and applying rotation adjustment afterwards
            moveTo(serial, X_PICKUP_COORDINATE + xAdjustment, Y_PICKUP_COORDINATE + yAdjustment);
            pickPartFromPuzzleMat(serial);
            rotatePart(serial, cAdjustment);
            // Moving the part to picture capture position
            sendCommand(serial, X_AXIS_COMMAND, X_PICKUP_COORDINATE + xAdjustment);
            releasePartToPuzzleMat(serial);
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP_COORDINATE + yAdjustment + Y_TOOL_OFFSET);
            // Capturing and preprocessing a picture of the part for puzzle solving
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cout << "Captured part image successfully" << endl;
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
            cout << "Contour detection finished successfully" << endl;
            // Creating and showing the processed frame
            processedFrame = Mat::zeros(cannyFrame.size(), CV_8UC3);
            drawContours(processedFrame, vector<vector<Point>>(1, boundRectContour), 0, color);
            rectangle(processedFrame, boundRect.tl(), boundRect.br(), color);
            showImage(WINDOW_NAME, processedFrame);
            // Cropping the frame and storing it for future processing
            croppedFrame = rawFrame(Range(boundRect.tl().y, boundRect.tl().y + boundRect.height), Range(boundRect.tl().x, boundRect.tl().x + boundRect.width));
            partSolvingImages.push_back(croppedFrame);
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
                partSolvingImages.push_back(rotatedCroppedFrame);
                showImage(WINDOW_NAME, rotatedCroppedFrame);
            }
            solvingImages.push_back(partSolvingImages);
            cout << "Created part matching images successfully" << endl;
            // Moving the part to it's storage coordinates
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP_COORDINATE + yAdjustment);
            pickPartFromPuzzleMat(serial);
            moveTo(serial, storageCoordinates[i][0], storageCoordinates[i][1]);
            releasePartToPuzzleMat(serial);
            cout << "Moved part " << (i + 1) << " to it's storage location successfully" << endl;
        }
        // Solving the jigsaw puzzle by utilitizing multi-scale template matching
        cout << "Solving the jigsaw puzzle" << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Solving the jigsaw puzzle.\nPlease wait..."));
        for (int i = 0; i < PART_COUNT; i++) {
            // Defining processing related variables
            vector<int> partSolvingResults;
            int xPosition = 0;
            int yPosition = 0;
            int angle = 0;
            float scale = 1;
            double matchVal = 0;
            Point matchLoc;
            Mat partImage;
            Mat processedImage;
            // Looping through all four orientations
            for (int j = 0; j < 4; i++) {
                Mat currentPartImage = solvingImages[i][j];
                // Looping through different scales
                for (float k = 1; k > 0.05; k -= 0.05) {
                    Mat result;
                    Mat resizedReferenceImage;
                    double maxVal;
                    Point maxLoc;
                    // Resizing the reference image
                    resize(referenceImage, resizedReferenceImage, Size(), k, k);
                    if (resizedReferenceImage.cols < currentPartImage.cols || resizedReferenceImage.rows < currentPartImage.rows) {
                        break;
                    }
                    // Doing the actual template matching
                    result.create(resizedReferenceImage.rows - currentPartImage.rows + 1, resizedReferenceImage.cols - currentPartImage.cols, CV_32FC1);
                    matchTemplate(resizedReferenceImage, currentPartImage, result, TM_CCOEFF_NORMED);
                    // Localizing the best match and updating the match values in that case
                    minMaxLoc(result, new double, &maxVal, new Point, &maxLoc);
                    if (maxVal > matchVal) {
                        scale = referenceImage.cols / resizedReferenceImage.cols;
                        matchVal = maxVal;
                        matchLoc = maxLoc;
                        xPosition = floor((matchLoc.x + (currentPartImage.rows / 2)) / (resizedReferenceImage.cols / COLUMN_COUNT));
                        yPosition = ROW_COUNT - 1 - floor((matchLoc.y + (currentPartImage.rows / 2)) / (resizedReferenceImage.rows / ROW_COUNT));
                        angle = j * 90;
                        partImage = currentPartImage;
                    }
                }
            }
            // Creating and showing the processed frame
            rectangle(processedImage, Point(round(matchLoc.x * scale), round(matchLoc.y * scale)), Point(round((matchLoc.x + partImage.cols) * scale), round((matchLoc.y + partImage.rows) * scale)), Scalar::all(0), 2, 8, 0);
            showImage(WINDOW_NAME, processedImage);
            // Storing the important data
            partSolvingResults.push_back(xPosition);
            partSolvingResults.push_back(yPosition);
            partSolvingResults.push_back(angle);
            solvingResults.push_back(partSolvingResults);
            cout << "Calculated the position of part " << (i + 1) << " successfully" << endl;
        }
        // Moving all the parts to their calculated positions
        cout << "Moving the parts to their final positions" << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Moving the parts to their final positions.\nPlease wait..."));
        for (int i = 0; i < PART_COUNT; i++) {
            moveTo(serial, storageCoordinates[i][0], storageCoordinates[i][1]);
            pickPartFromPuzzleMat(serial);
            if (solvingResults[i][2] != 0) {
                rotatePart(serial, solvingResults[i][2]);
            }
            moveTo(serial, assemblyCoordinates[solvingResults[i][0]][solvingResults[i][1]][0], assemblyCoordinates[solvingResults[i][0]][solvingResults[i][1]][1]);
            releasePartToPuzzleMat(serial);
            cout << "Moved part " << (i + 1) << " to it's final position successfully" << endl;
        }
        // Moving back to start and closing the program
        moveTo(serial, 0, 0);
        cout << "Finished solving the jigsaw puzzle!" << endl;
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1920, 1080), CV_8UC3), "Finished solving the jigsaw puzzle!"));
        camera.release();
        serial.Close();
        return 0;
    } else {
        cout << "Wrong number of arguments. Eight arguments containing the path of the camera calibration file, camera ID, serial port as well as reference image, column count, row count, width and height in mm of the jigsaw puzzle expected." << endl;
        cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0 reference.png 6 4 150 100" << endl;
        return 1;
    }
}

// Defining text drawing function
Mat drawText(Mat image, string text) {
    string lineBuffer;
    vector<string> lines;
    stringstream textStream(text);
    Mat imageBuffer = image.clone();
    while (getline(textStream, lineBuffer, '\n')) {
        lines.push_back(lineBuffer);
    }
    for (int i = 0; i < lines.size(); i++) {
        putText(imageBuffer, lines[i], Point(0, (50 * i) + 40), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 2);
    }
    return imageBuffer;
}

// Defining an image showing function
void showImage(String windowName, Mat image) {
    while (true) {
        imshow(windowName, image);
        int keyPressed = waitKey(1000 / 25);
        if (keyPressed == 113) {
            exit(0);
        } else if (keyPressed == 114) {
            break;
        }
    }
}

// Defining picture capturing function
Mat capturePicture(VideoCapture& camera, Mat cameraMatrix, Mat distortionCoefficients) {
    Mat rawFrame;
    Mat undistortedFrame;
    Mat flippedFrame;
    camera.grab();
    camera.read(rawFrame);
    if (rawFrame.empty()) {
        cout << "Blank frame grabbed" << endl;
        exit(1);
    }
    undistort(rawFrame, undistortedFrame, cameraMatrix, distortionCoefficients);
    flip(undistortedFrame, flippedFrame, -1);
    return flippedFrame;
}

// Defining command sending function
void sendCommand(SerialStream& serial, char command, int commandParameter) {
    for (int numberOfTries = 0; numberOfTries < 3; numberOfTries++) {
        string serialInput;
        char serialResponse;
        serial << command << commandParameter << endl;
        getline(serial, serialInput);
        serialResponse = serialInput.at(0);
        if (serialResponse == AVAILABILITY_MESSAGE) {
            break;
        } else if (serialResponse == ERROR_MESSAGE) {
            cout << "Robot failed to execute the command " << command << commandParameter << endl;
            exit(1);
        }
    }
}

// Defining helper functions for robot control
void moveTo(SerialStream& serial, int xCoordinate, int yCoordinate) {
    sendCommand(serial, X_AXIS_COMMAND, xCoordinate);
    sendCommand(serial, Y_AXIS_COMMAND, yCoordinate);
}

void pickPartFromPuzzleMat(SerialStream& serial) {
    sendCommand(serial, VACUUM_PUMP_COMMAND, 100);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 15);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void pickPartFromTurntable(SerialStream& serial) {
    sendCommand(serial, VACUUM_PUMP_COMMAND, 100);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 60);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void releasePartToPuzzleMat(SerialStream& serial) {
    sendCommand(serial, Z_AXIS_COMMAND, 45);
    sleep(1);
    sendCommand(serial, VACUUM_PUMP_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void releasePartToTurntable(SerialStream& serial) {
    sendCommand(serial, VACUUM_PUMP_COMMAND, 0);
    sleep(1);
}

void rotatePart(SerialStream& serial, int angle) {
    sendCommand(serial, X_AXIS_COMMAND, 0);
    sleep(1);
    releasePartToTurntable(serial);
    sleep(1);
    sendCommand(serial, C_AXIS_COMMAND, angle);
    sleep(1);
    pickPartFromTurntable(serial);
    sleep(1);
}