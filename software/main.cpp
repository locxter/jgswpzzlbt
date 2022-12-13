#include <iostream>
#include "lib/opencv-helpers.hpp"
#include "lib/libserial-helpers.hpp"

// Main function
int main(int argc, char** argv) {
    // Check for the right number of command line arguments
    if (argc == 9) {
        // Variables for command line arguments and some values on them
        const std::string CAMERA_CALIBRATION_FILE = argv[1];
        const int CAMERA_ID = std::stoi(argv[2]);
        const std::string SERIAL_PORT = argv[3];
        const std::string REFERENCE_IMAGE_FILE = argv[4];
        const int COLUMN_COUNT = std::stoi(argv[5]);
        const int ROW_COUNT = std::stoi(argv[6]);
        const int PART_COUNT = COLUMN_COUNT * ROW_COUNT;
        const int WIDTH = std::stoi(argv[7]);
        const int HEIGHT = std::stoi(argv[8]);
        const std::string WINDOW_NAME = "jgswpzzlbt";
        // Robot axis minimums and maximums as well as tool offset
        const int X_TOOL_OFFSET = 0;
        const int Y_TOOL_OFFSET = 80;
        const int X_AXIS_MIN = 50;
        const int X_AXIS_MAX = 825;
        const int Y_AXIS_MAX = 725;
        // Objects for robot communication
        cv::VideoCapture camera;
        cv::Mat cameraMatrix;
        cv::Mat distortionCoefficients;
        cv::FileStorage cameraCalibration;
        LibSerial::SerialStream serial;
        std::string response;
        // Part storage related variables
        int storageCellSize;
        if (((float) WIDTH / COLUMN_COUNT) < ((float) HEIGHT / ROW_COUNT)) {
            storageCellSize = std::round(((float) HEIGHT / ROW_COUNT) + 20);
        } else {
            storageCellSize = std::round(((float) WIDTH / COLUMN_COUNT) + 20);
        }
        const int STORAGE_COLUMN_COUNT = std::floor(((float) X_AXIS_MAX - X_AXIS_MIN) / storageCellSize);
        const int STORAGE_ROW_COUNT = std::ceil((float) PART_COUNT / STORAGE_COLUMN_COUNT);
        std::vector<std::vector<int>> storageCoordinates;
        // Puzzle solving related variables
        cv::Mat referenceImage;
        std::vector<cv::Mat> solvingImages;
        std::vector<std::vector<int>> solvingResults;
        // Puzzle assembly related variables
        const int ASSEMBLY_COLUMN_WIDTH = std::round(((float) WIDTH / COLUMN_COUNT) + 20);
        const int ASSEMBLY_ROW_HEIGHT = std::round(((float) HEIGHT / ROW_COUNT) + 20);
        std::vector<std::vector<std::vector<int>>> assemblyCoordinates;
        // Fill the storage and assembly coordinates with data
        for (int i = 0; i < STORAGE_ROW_COUNT; i++) {
            for (int j = 0; j < STORAGE_COLUMN_COUNT; j++) {
                std::vector<int> partCoordinates;
                if (storageCoordinates.size() == PART_COUNT) {
                    break;
                }
                partCoordinates.push_back(std::round((storageCellSize * .5) + (storageCellSize * j) + X_AXIS_MIN));
                partCoordinates.push_back(std::round(Y_AXIS_MAX - ((storageCellSize * .5) + (storageCellSize * i))));
                storageCoordinates.push_back(partCoordinates);
            }
        }
        for (int i = 0; i < COLUMN_COUNT; i++) {
            std::vector<std::vector<int>> columnCoordinates;
            for (int j = 0; j < ROW_COUNT; j++) {
                std::vector<int> partCoordinates;
                partCoordinates.push_back(std::round((ASSEMBLY_COLUMN_WIDTH * .5) + (ASSEMBLY_COLUMN_WIDTH * i) + X_AXIS_MIN));
                partCoordinates.push_back(std::round((ASSEMBLY_ROW_HEIGHT * .5) + (ASSEMBLY_ROW_HEIGHT * j)));
                columnCoordinates.push_back(partCoordinates);
            }
            assemblyCoordinates.push_back(columnCoordinates);
        }
        // Initialize robot
        cameraCalibration.open("camera-calibration.xml", cv::FileStorage::READ);
        if (cameraCalibration.isOpened()) {
            std::cout << "Opened camera calibration successfully." << std::endl;
        } else {
            std::cout << "Failed to open camera calibration." << std::endl;
            return 1;
        }
        cameraCalibration["camera-matrix"] >> cameraMatrix;
        cameraCalibration["distortion-coefficients"] >> distortionCoefficients;
        cameraCalibration.release();
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
        std::getline(serial, response);
        if (response.at(0) == AVAILABILITY_MESSAGE) {
            std::cout << "Robot homed successfully." << std::endl;
        } else {
            std::cout << "Homing of the robot failed." << std::endl;
            return 1;
        }
        // Display a small help at start
        cv::namedWindow(WINDOW_NAME);
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "This program is fully keyboard driven. Here is a full list of all available actions:\nQ: Quit the program\nR: Indicate that the next operation can be performed"));
        // Move all parts to their storage position and capture needed images on the way
        for (int i = 0; i < PART_COUNT; i++) {
            // Variables for pickup coordinates and position adjustments
            const int X_PICKUP = X_AXIS_MIN + X_TOOL_OFFSET + 125;
            const int Y_PICKUP = Y_TOOL_OFFSET + 75;
            int xAdjustment = 0;
            int yAdjustment = 0;
            int cAdjustment = 0;
            // Image containers
            cv::Mat rawFrame;
            cv::Mat preprocessedFrame;
            cv::Mat cannyFrame;
            cv::Mat processedFrame;
            // Image processing variables
            std::vector<std::vector<cv::Point>> contours;
            cv::RotatedRect minRect;
            cv::Rect boundRect;
            cv::Mat croppedFrame;
            // Move the head to the pickup coordinates
            moveTo(serial, X_PICKUP, Y_PICKUP);
            std::cout << "Moved the pick up coordinates successfully." << std::endl;
            // Prompt the user to lay down a jigsaw puzzle part
            showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Please lay down the next piece in the center of the camera viewport.\nThe next screen will help you with the alignment..."));
            // Show a life camera feed with alignment helps
            while (true) {
                // Image containers and variable for storing the key pressed
                cv::Mat rawFrame;
                cv::Mat resizedFrame;
                int keyPressed;
                // Show a frame
                rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
                cv::resize(rawFrame, resizedFrame, cv::Size(1280, 720));
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
            // Capture and preprocess a picture of the part for position correction
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cv::imshow(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Moving the part to it's storage position. Please wait..."));
            cv::waitKey(1000);
            cv::cvtColor(rawFrame, preprocessedFrame, cv::COLOR_BGR2GRAY);
            cv::medianBlur(preprocessedFrame, preprocessedFrame, 25);
            cv::adaptiveThreshold(preprocessedFrame, preprocessedFrame, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 75, 0);
            cv::medianBlur(preprocessedFrame, preprocessedFrame, 25);
            std::cout << "Captured and preprocessed part image successfully." << std::endl;
            // Perform canny edge detection
            cv::Canny(preprocessedFrame, cannyFrame, 0, 0);
            // Find contours and select only the largest one together with it's minimum bounding rectangle
            cv::findContours(cannyFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            for (int j = 0; j < contours.size(); j++) {
                cv::RotatedRect buffer = cv::minAreaRect(contours[j]);
                static float largestArea = 0;
                if (j == 0) {
                    largestArea = 0;
                }
                if (buffer.size.area() > largestArea) {
                    minRect = buffer;
                    largestArea = buffer.size.area();
                }
            }
            std::cout << "Contour detection finished successfully." << std::endl;
            // Extract and print the important information
            xAdjustment = std::round(((minRect.center.x - 960) / 8) + X_TOOL_OFFSET);
            yAdjustment = std::round(((540 - minRect.center.y) / 8) - Y_TOOL_OFFSET);
            cAdjustment = std::round(90 - minRect.angle);
            std::cout << "Finished position and rotation adjustment calculation successfully:" << std::endl;
            std::cout << "X adjustment: " << xAdjustment << std::endl;
            std::cout << "Y adjustment: " << yAdjustment << std::endl;
            std::cout << "C adjustment: " << cAdjustment << std::endl;
            // Pick the part from the adjusted position and apply rotation adjustment afterwards
            moveTo(serial, X_PICKUP + xAdjustment, Y_PICKUP + yAdjustment);
            pickPartFromPuzzleMat(serial);
            rotatePart(serial, cAdjustment);
            // Move the part to picture capture position
            sendCommand(serial, X_AXIS_COMMAND, X_PICKUP + xAdjustment);
            releasePartToPuzzleMat(serial);
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP + yAdjustment + Y_TOOL_OFFSET);
            sleep(1);
            // Capture and preprocess a picture of the part for puzzle solving
            rawFrame = capturePicture(camera, cameraMatrix, distortionCoefficients);
            cv::cvtColor(rawFrame, preprocessedFrame, cv::COLOR_BGR2GRAY);
            cv::medianBlur(preprocessedFrame, preprocessedFrame, 25);
            cv::adaptiveThreshold(preprocessedFrame, preprocessedFrame, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 75, 0);
            cv::medianBlur(preprocessedFrame, preprocessedFrame, 25);
            std::cout << "Captured and preprocessed part image successfully." << std::endl;
            // Perform canny edge detection
            cv::Canny(preprocessedFrame, cannyFrame, 0, 0);
            // Find contours and select only the largest one together with it's bounding rectangle
            cv::findContours(cannyFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            for (int j = 0; j < contours.size(); j++) {
                cv::Rect buffer = cv::boundingRect(contours[j]);
                static float largestArea = 0;
                if (j == 0) {
                    largestArea = 0;
                }
                if (buffer.area() > largestArea) {
                    boundRect = buffer;
                    largestArea = buffer.area();
                }
            }
            std::cout << "Contour detection finished successfully." << std::endl;
            // Crop the frame and store it for future processing
            croppedFrame = rawFrame(cv::Range(boundRect.tl().y, boundRect.tl().y + boundRect.height), cv::Range(boundRect.tl().x, boundRect.tl().x + boundRect.width));
            solvingImages.push_back(croppedFrame);
            std::cout << "Created part image for puzzle solving successfully." << std::endl;
            // Move the part to it's storage coordinates
            sendCommand(serial, Y_AXIS_COMMAND, Y_PICKUP + yAdjustment);
            pickPartFromPuzzleMat(serial);
            moveTo(serial, storageCoordinates[i][0], storageCoordinates[i][1]);
            releasePartToPuzzleMat(serial);
            std::cout << "Moved part " << (i + 1) << " to it's storage location successfully." << std::endl;
        }
        sendCommand(serial, X_AXIS_COMMAND, 0);
        // Solve the jigsaw puzzle by utilitizing multi-scale template matching
        std::cout << "Solving the jigsaw puzzle." << std::endl;
        cv::imshow(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Solving the jigsaw puzzle. Please wait..."));
        cv::waitKey(1000);
        referenceImage = cv::imread(REFERENCE_IMAGE_FILE);
        for (int i = 0; i < PART_COUNT; i++) {
            // Processing related variables
            cv::Mat partImage = solvingImages[i];
            std::vector<int> partSolvingResults;
            int xPosition = 0;
            int yPosition = 0;
            int orientation = 0;
            const int MAX_FEATURES = 1024;
            const float GOOD_MATCH_PERCENT = 0.1;
            cv::Ptr<cv::Feature2D> partOrb;
            cv::Ptr<cv::Feature2D> referenceOrb;
            std::vector<cv::KeyPoint> partKeypoints;
            std::vector<cv::KeyPoint> referenceKeypoints;
            cv::Mat partDescriptors;
            cv::Mat referenceDescriptors;
            cv::Ptr<cv::DescriptorMatcher> matcher;
            std::vector<cv::DMatch> matches;
            std::vector<cv::Point2f> partPoints;
            std::vector<cv::Point2f> referencePoints;
            std::vector<cv::Point2f> partCorners(4);
            std::vector<cv::Point2f> referenceCorners(4);
            cv::Rect boundRect;
            // Detect ORB features and compute descriptors
            partOrb = cv::ORB::create(MAX_FEATURES);
            referenceOrb = cv::ORB::create(MAX_FEATURES * 16);
            partOrb->detectAndCompute(partImage, cv::Mat(), partKeypoints, partDescriptors);
            referenceOrb->detectAndCompute(referenceImage, cv::Mat(), referenceKeypoints, referenceDescriptors);
            // Match features
            matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
            matcher->match(partDescriptors, referenceDescriptors, matches, cv::Mat());
            // Remove not so good matches
            std::sort(matches.begin(), matches.end());
            matches.erase(matches.begin() + (matches.size() * GOOD_MATCH_PERCENT), matches.end());
            // Extract location of good matches
            for (int j = 0; j < matches.size(); j++) {
                partPoints.push_back(partKeypoints[matches[j].queryIdx].pt);
                referencePoints.push_back(referenceKeypoints[matches[j].trainIdx].pt);
            }
            // Find homography
            cv::Mat homography = cv::findHomography(partPoints, referencePoints, cv::RANSAC);
            // Map part image onto reference image
            partCorners[0] = cv::Point2f(0, 0);
            partCorners[1] = cv::Point2f((float) partImage.cols, 0);
            partCorners[2] = cv::Point2f((float) partImage.cols, (float) partImage.rows);
            partCorners[3] = cv::Point2f(0, (float) partImage.rows);
            cv::perspectiveTransform(partCorners, referenceCorners, homography);
            // Extract important data
            for (int j = 0; j < 4; j++) {
                static float xAverage = (referenceCorners[0].x + referenceCorners[1].x + referenceCorners[2].x + referenceCorners[3].x) / 4.0;
                static float yAverage = (referenceCorners[0].y + referenceCorners[1].y + referenceCorners[2].y + referenceCorners[3].y) / 4.0;
                if (j == 0) {
                    xAverage = (referenceCorners[0].x + referenceCorners[1].x + referenceCorners[2].x + referenceCorners[3].x) / 4.0;
                    yAverage = (referenceCorners[0].y + referenceCorners[1].y + referenceCorners[2].y + referenceCorners[3].y) / 4.0;
                }
                if (referenceCorners[j].x < xAverage && referenceCorners[j].y < yAverage) {
                    // Swap 1 and 3 (aka -1) as the robot in reality always messes them up for some reason
                    if (j == 1) {
                        orientation = -1;
                    } else if (j == 3) {
                        orientation = 1;
                    } else {
                        orientation = j;
                    }
                }
            }
            boundRect = cv::boundingRect(referenceCorners);
            xPosition = std::floor((boundRect.tl().x + (boundRect.width / 2.0)) / ((float) referenceImage.cols / COLUMN_COUNT));
            yPosition = ROW_COUNT - (1 + std::floor((boundRect.tl().y + (boundRect.height / 2.0)) / ((float) referenceImage.rows / ROW_COUNT)));
            // Store and print the important data
            partSolvingResults.push_back(xPosition);
            partSolvingResults.push_back(yPosition);
            partSolvingResults.push_back(orientation);
            solvingResults.push_back(partSolvingResults);
            std::cout << "Calculated the final position of part " << (i + 1) << " successfully:" << std::endl;
            std::cout << "X position: " << xPosition << std::endl;
            std::cout << "Y position: " << yPosition << std::endl;
            std::cout << "Orientation: " << orientation * 90 << std::endl;
        }
        // Move all the parts to their calculated positions
        std::cout << "Moving the parts to their final positions." << std::endl;
        cv::imshow(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Moving the parts to their final positions. Please wait..."));
        cv::waitKey(1000);
        for (int i = 0; i < PART_COUNT; i++) {
            int xPosition = solvingResults[i][0];
            int yPosition = solvingResults[i][1];
            int orientation = solvingResults[i][2];
            moveTo(serial, storageCoordinates[i][0], storageCoordinates[i][1]);
            pickPartFromPuzzleMat(serial);
            if (orientation != 0) {
                rotatePart(serial, orientation * 90);
            }
            moveTo(serial, assemblyCoordinates[xPosition][yPosition][0], assemblyCoordinates[xPosition][yPosition][1]);
            releasePartToPuzzleMat(serial);
            std::cout << "Moved part " << (i + 1) << " to it's final position successfully." << std::endl;
        }
        // Move out of the way and close the program
        moveTo(serial, 0, Y_AXIS_MAX);
        std::cout << "Finished solving the jigsaw puzzle!" << std::endl;
        showImage(WINDOW_NAME, drawText(cv::Mat::zeros(cv::Size(1280, 720), CV_8UC3), "Finished solving the jigsaw puzzle!"));
        camera.release();
        serial.Close();
        return 0;
    } else {
        // Throw an error on invalid number of command line arguments
        std::cout << "Wrong number of arguments. Eight arguments containing the path of the camera calibration file, camera ID, serial port as well as reference image, column count, row count, width and height in mm of the jigsaw puzzle expected." << std::endl;
        std::cout << "Example: " << argv[0] << " camera-calibration.xml 0 /dev/ttyUSB0 reference.png 6 4 300 200" << std::endl;
        return 1;
    }
}
