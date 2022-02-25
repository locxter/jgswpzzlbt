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

// Defining robot axis minimums and maximums
const int X_AXIS_MIN_COORDINATE = 50;
const int X_AXIS_MAX_COORDINATE = 825;
const int Y_AXIS_MAX_COORDINATE = 725;
const int Z_AXIS_MAX_COORDINATE = 90;

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
        // Storing commandline arguments and program name
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
        // Creating objects for robot communication
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
        int storageCellSize;
        if (((float)WIDTH / COLUMN_COUNT) < ((float)HEIGHT / ROW_COUNT)) {
            storageCellSize = round(((float)HEIGHT / ROW_COUNT) + 20);
        } else {
            storageCellSize = round(((float)WIDTH / COLUMN_COUNT) + 20);
        }
        const int STORAGE_COLUMN_COUNT = floor(((float)X_AXIS_MAX_COORDINATE - X_AXIS_MIN_COORDINATE) / storageCellSize);
        const int STORAGE_ROW_COUNT = ceil((float)PART_COUNT / STORAGE_COLUMN_COUNT);
        vector<vector<int>> storageCoordinates;
        // Defining puzzle solving related stuff
        Mat referenceImage;
        vector<Mat> solvingImages;
        vector<vector<int>> solvingResults;
        // Defining puzzle assembly related variables
        const int ASSEMBLY_COLUMN_WIDTH = round(((float)WIDTH / COLUMN_COUNT) + 20);
        const int ASSEMBLY_ROW_HEIGHT = round(((float)HEIGHT / ROW_COUNT) + 20);
        vector<vector<vector<int>>> assemblyCoordinates;
        // Filling the storage and assembly coordinates with data
        for (int i = 0; i < STORAGE_ROW_COUNT; i++) {
            for (int j = 0; j < STORAGE_COLUMN_COUNT; j++) {
                vector<int> partCoordinates;
                if (storageCoordinates.size() == PART_COUNT) {
                    break;
                }
                partCoordinates.push_back(round((storageCellSize * .5) + (storageCellSize * j) + X_AXIS_MIN_COORDINATE));
                partCoordinates.push_back(round(Y_AXIS_MAX_COORDINATE - ((storageCellSize * .5) + (storageCellSize * i))));
                storageCoordinates.push_back(partCoordinates);
            }
        }
        for (int i = 0; i < COLUMN_COUNT; i++) {
            vector<vector<int>> columnCoordinates;
            for (int j = 0; j < ROW_COUNT; j++) {
                vector<int> partCoordinates;
                partCoordinates.push_back(round((ASSEMBLY_COLUMN_WIDTH * .5) + (ASSEMBLY_COLUMN_WIDTH * i) + X_AXIS_MIN_COORDINATE));
                partCoordinates.push_back(round((ASSEMBLY_ROW_HEIGHT * .5) + (ASSEMBLY_ROW_HEIGHT * j)));
                columnCoordinates.push_back(partCoordinates);
            }
            assemblyCoordinates.push_back(columnCoordinates);
        }
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
        namedWindow(WINDOW_NAME, WINDOW_NORMAL);
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));
        // Moving all parts to their storage position and capturing needed images on the way
        for (int i = 0; i < PART_COUNT; i++) {
            // Defining variables for pickup coordinates and position adjustments
            static const int X_PICKUP_COORDINATE = X_AXIS_MIN_COORDINATE + X_TOOL_OFFSET + 75;
            static const int Y_PICKUP_COORDINATE = Y_TOOL_OFFSET + 75;
            int xAdjustment = 0;
            int yAdjustment = 0;
            int cAdjustment = 0;
            // Creating image containers
            Mat rawFrame;
            Mat preprocessedFrame;
            Mat cannyFrame;
            Mat processedFrame;
            // Creating image processing variables
            vector<vector<Point>> contours;
            RotatedRect minRect;
            Rect boundRect;
            Mat croppedFrame;
            // Moving the head to the pickup coordinates
            moveTo(serial, X_PICKUP_COORDINATE, Y_PICKUP_COORDINATE);
            cout << "Moved the pick up coordinates successfully." << endl;
            // Prompting the user to lay down a jigsaw puzzle part
            showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Please lay down the next piece in the bottom left corner of the work area..."));
            // Capturing and preprocessing a picture of the part for position correction
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Moving the part to it's storage position. Please wait..."));
            waitKey(1000);
            cvtColor(rawFrame, preprocessedFrame, COLOR_BGR2GRAY);
            medianBlur(preprocessedFrame, preprocessedFrame, 15);
            threshold(preprocessedFrame, preprocessedFrame, 0, 255, THRESH_TRIANGLE);
            cout << "Captured and preprocessed part image successfully." << endl;
            // Performing canny edge detection
            Canny(preprocessedFrame, cannyFrame, 128, 255);
            // Finding contours and selecting only the largest one together with it's minimum bounding rectangle
            findContours(cannyFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (int j = 0; j < contours.size(); j++) {
                const RotatedRect BUFFER = minAreaRect(contours[j]);
                static float largestArea = 0;
                if (j == 0) {
                    largestArea = 0;
                }
                if (BUFFER.size.area() > largestArea) {
                    minRect = BUFFER;
                    largestArea = BUFFER.size.area();
                }
            }
            cout << "Contour detection finished successfully." << endl;
            // Extracting the important information
            xAdjustment = round(((minRect.center.x - 960) / 8) + X_TOOL_OFFSET);
            yAdjustment = round(((540 - minRect.center.y) / 8) - Y_TOOL_OFFSET);
            cAdjustment = round(90 - minRect.angle);
            cout << "Finished position and rotation adjustment calculation successfully:" << endl;
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
            sleep(1);
            // Capturing and preprocessing a picture of the part for puzzle solving
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cvtColor(rawFrame, preprocessedFrame, COLOR_BGR2GRAY);
            medianBlur(preprocessedFrame, preprocessedFrame, 15);
            threshold(preprocessedFrame, preprocessedFrame, 0, 255, THRESH_TRIANGLE);
            cout << "Captured and preprocessed part image successfully." << endl;
            // Performing canny edge detection
            Canny(preprocessedFrame, cannyFrame, 128, 255);
            // Finding contours and selecting only the largest one together with it's bounding rectangle
            findContours(cannyFrame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            for (int j = 0; j < contours.size(); j++) {
                const Rect BUFFER = boundingRect(contours[j]);
                static float largestArea = 0;
                if (j == 0) {
                    largestArea = 0;
                }
                if (BUFFER.area() > largestArea) {
                    boundRect = BUFFER;
                    largestArea = BUFFER.area();
                }
            }
            cout << "Contour detection finished successfully." << endl;
            // Cropping the frame and storing it for future processing
            croppedFrame = rawFrame(Range(boundRect.tl().y, boundRect.tl().y + boundRect.height), Range(boundRect.tl().x, boundRect.tl().x + boundRect.width));
            solvingImages.push_back(croppedFrame);
            cout << "Created part image for puzzle solving successfully." << endl;
            // Moving the part to it's storage coordinates
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP_COORDINATE + yAdjustment);
            pickPartFromPuzzleMat(serial);
            moveTo(serial, storageCoordinates[i][0], storageCoordinates[i][1]);
            releasePartToPuzzleMat(serial);
            cout << "Moved part " << (i + 1) << " to it's storage location successfully." << endl;
        }
        sendCommand(serial, X_AXIS_COMMAND, 0);
        // Solving the jigsaw puzzle by utilitizing multi-scale template matching
        cout << "Solving the jigsaw puzzle." << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Solving the jigsaw puzzle. Please wait..."));
        waitKey(1000);
        referenceImage = imread(REFERENCE_IMAGE_FILE, IMREAD_COLOR);
        for (int i = 0; i < PART_COUNT; i++) {
            // Defining processing related variables
            Mat partImage = solvingImages[i];
            vector<int> partSolvingResults;
            int xPosition = 0;
            int yPosition = 0;
            int orientation = 0;
            const int MAX_FEATURES = 1024;
            const float GOOD_MATCH_PERCENT = 0.1f;
            Ptr<Feature2D> partOrb;
            Ptr<Feature2D> referenceOrb;
            vector<KeyPoint> partKeypoints;
            vector<KeyPoint> referenceKeypoints;
            Mat partDescriptors;
            Mat referenceDescriptors;
            Ptr<DescriptorMatcher> matcher;
            vector<DMatch> matches;
            vector<Point2f> partPoints;
            vector<Point2f> referencePoints;
            vector<Point2f> partCorners(4);
            vector<Point2f> referenceCorners(4);
            Rect boundRect;
            // Detecting ORB features and computing descriptors
            partOrb = ORB::create(MAX_FEATURES);
            referenceOrb = ORB::create(MAX_FEATURES * 16);
            partOrb->detectAndCompute(partImage, Mat(), partKeypoints, partDescriptors);
            referenceOrb->detectAndCompute(referenceImage, Mat(), referenceKeypoints, referenceDescriptors);
            // Matching features
            matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);
            matcher->match(partDescriptors, referenceDescriptors, matches, Mat());
            // Removing not so good matches
            sort(matches.begin(), matches.end());
            matches.erase(matches.begin() + (matches.size() * GOOD_MATCH_PERCENT), matches.end());
            // Extracting location of good matches
            for (size_t i = 0; i < matches.size(); i++) {
                partPoints.push_back(partKeypoints[matches[i].queryIdx].pt);
                referencePoints.push_back(referenceKeypoints[matches[i].trainIdx].pt);
            }
            // Finding homography
            Mat homography = findHomography(partPoints, referencePoints, RANSAC);
            // Mapping part image onto reference image
            partCorners[0] = Point2f(0, 0);
            partCorners[1] = Point2f((float)partImage.cols, 0);
            partCorners[2] = Point2f((float)partImage.cols, (float)partImage.rows);
            partCorners[3] = Point2f(0, (float)partImage.rows);
            perspectiveTransform(partCorners, referenceCorners, homography);
            // Extracting important data
            for (int i = 0; i < 4; i++) {
                static float xAverage = (referenceCorners[0].x + referenceCorners[1].x + referenceCorners[2].x + referenceCorners[3].x) / 4;
                static float yAverage = (referenceCorners[0].y + referenceCorners[1].y + referenceCorners[2].y + referenceCorners[3].y) / 4;
                if (i == 0) {
                    xAverage = (referenceCorners[0].x + referenceCorners[1].x + referenceCorners[2].x + referenceCorners[3].x) / 4;
                    yAverage = (referenceCorners[0].y + referenceCorners[1].y + referenceCorners[2].y + referenceCorners[3].y) / 4;
                }
                if (referenceCorners[i].x < xAverage && referenceCorners[i].y < yAverage) {
                    // Swapping 1 and 3 as the robot in reality always messes them up for some reason
                    if (i == 1) {
                        orientation = 3;
                    } else if (i == 3) {
                        orientation = 1;
                    } else {
                        orientation = i;
                    }
                }
            }
            boundRect = boundingRect(referenceCorners);
            xPosition = floor((boundRect.tl().x + (boundRect.width / 2)) / (referenceImage.cols / COLUMN_COUNT));
            yPosition = ROW_COUNT - 1 - floor((boundRect.tl().y + (boundRect.width / 2)) / (referenceImage.rows / ROW_COUNT));
            // Storing the important data
            partSolvingResults.push_back(xPosition);
            partSolvingResults.push_back(yPosition);
            partSolvingResults.push_back(orientation);
            solvingResults.push_back(partSolvingResults);
            cout << "Calculated the final position of part " << (i + 1) << " successfully:" << endl;
            cout << "X position: " << xPosition << endl;
            cout << "Y position: " << yPosition << endl;
            cout << "Orientation: " << orientation * 90 << endl;
        }
        // Moving all the parts to their calculated positions
        cout << "Moving the parts to their final positions." << endl;
        imshow(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Moving the parts to their final positions. Please wait..."));
        waitKey(1000);
        for (int i = 0; i < PART_COUNT; i++) {
            const int X_POSITION = solvingResults[i][0];
            const int Y_POSITION = solvingResults[i][1];
            const int ORIENTATION = solvingResults[i][2];
            moveTo(serial, storageCoordinates[i][0], storageCoordinates[i][1]);
            pickPartFromPuzzleMat(serial);
            if (ORIENTATION != 0) {
                rotatePart(serial, ORIENTATION * 90);
            }
            moveTo(serial, assemblyCoordinates[X_POSITION][Y_POSITION][0], assemblyCoordinates[X_POSITION][Y_POSITION][1]);
            releasePartToPuzzleMat(serial);
            cout << "Moved part " << (i + 1) << " to it's final position successfully." << endl;
        }
        // Moving back to start and closing the program
        moveTo(serial, 0, Y_AXIS_MAX_COORDINATE);
        cout << "Finished solving the jigsaw puzzle!" << endl;
        showImage(WINDOW_NAME, drawText(Mat::zeros(Size(1280, 720), CV_8UC3), "Finished solving the jigsaw puzzle!"));
        camera.release();
        serial.Close();
        return 0;
    } else {
        // Throwing an error on invalid number of command line arguments
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

// Defining helper functions for robot control
void moveTo(SerialStream& serial, int xCoordinate, int yCoordinate) {
    sendCommand(serial, X_AXIS_COMMAND, xCoordinate);
    sendCommand(serial, Y_AXIS_COMMAND, yCoordinate);
}

void pickPartFromPuzzleMat(SerialStream& serial) {
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 100);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void pickPartFromTurntable(SerialStream& serial) {
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 100);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 50);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void releasePartToPuzzleMat(SerialStream& serial) {
    sendCommand(serial, Z_AXIS_COMMAND, 30);
    sleep(1);
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void releasePartToTurntable(SerialStream& serial) {
    sendCommand(serial, Z_AXIS_COMMAND, 80);
    sleep(1);
    sendCommand(serial, VACUUM_SYSTEM_COMMAND, 0);
    sleep(1);
    sendCommand(serial, Z_AXIS_COMMAND, 90);
    sleep(1);
}

void rotatePart(SerialStream& serial, int angle) {
    sendCommand(serial, X_AXIS_COMMAND, 0);
    sleep(1);
    releasePartToTurntable(serial);
    sendCommand(serial, C_AXIS_COMMAND, angle);
    sleep(1);
    pickPartFromTurntable(serial);
}