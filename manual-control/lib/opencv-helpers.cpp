#include "opencv-helpers.hpp"

// Function to draw text on an image
cv::Mat drawText(cv::Mat image, std::string text) {
    std::string lineBuffer;
    std::vector<std::string> lines;
    std::stringstream textStream(text);
    // Clone the input image for non-destructive drawing
    cv::Mat imageBuffer = image.clone();
    // Splitt the text into lines and write them on the image one by one
    while (std::getline(textStream, lineBuffer, '\n')) {
        lines.push_back(lineBuffer);
    }
    for (int i = 0; i < lines.size(); i++) {
        cv::putText(imageBuffer, lines[i], cv::Point(0, 30 * (i + 1)), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255));
    }
    return imageBuffer;
}

// Function to show an image and wait for user response
void showImage(std::string windowName, cv::Mat image) {
    cv::Mat imageBuffer;
    // Resize the image to fit inside the 1280 by 720 bounding box if needed
    if (image.cols > 1280 || image.rows > 720) {
        float scale;
        if (image.cols > image.rows) {
            scale = 1280.0 / image.cols;
        } else {
            scale = 720.0 / image.rows;
        }
        cv::resize(image, imageBuffer, cv::Size(), scale, scale);
    } else {
        imageBuffer = image.clone();
    }
    // Show the image until the user reacts
    while (true) {
        cv::imshow(windowName, imageBuffer);
        const int KEY_PRESSED = cv::waitKey(1000 / 25);
        if (KEY_PRESSED == 113) {
            exit(0);
        } else if (KEY_PRESSED == 114) {
            break;
        }
    }
}

// Function to capture a picture and apply the needed preprocessing
cv::Mat capturePicture(cv::VideoCapture& camera, cv::Mat cameraMatrix, cv::Mat distortionCoefficients) {
    cv::Mat rawFrame;
    cv::Mat undistortedFrame;
    cv::Mat flippedFrame;
    // Clean the buffer and read a frame
    camera.grab();
    camera.read(rawFrame);
    if (rawFrame.empty()) {
        std::cout << "Blank frame grabbed." << std::endl;
        exit(1);
    }
    // Basic preprocessing
    cv::undistort(rawFrame, undistortedFrame, cameraMatrix, distortionCoefficients);
    cv::flip(undistortedFrame, flippedFrame, -1);
    return flippedFrame;
}

// Function to save a picture
void savePicture(cv::Mat image, std::string filename) {
    std::ifstream testFile;
    int counter = 0;
    std::string completeFilename = filename + '-' + std::to_string(counter) + ".jpeg";
    testFile.open(completeFilename);
    while (testFile.is_open()) {
        testFile.close();
        counter++;
        completeFilename = filename + '-' + std::to_string(counter) + ".jpeg";
        testFile.open(completeFilename);
    }
    testFile.close();
    cv::imwrite(completeFilename, image);
}