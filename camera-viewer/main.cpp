// Including needed libraries
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

// Setting default namespace
using namespace cv;

// Defining camera ID
const int CAMERA_ID = 0;

int main()
{
    // Creating a camera object
    VideoCapture camera;
    // Creating image containers
    Mat rawFrame;
    Mat undistortedFrame;
    // Creating camera calibration objects;
    Mat cameraMatrix;
    Mat distCoeffs;
    FileStorage cameraCalibration;
    // Opening the camera calibration file
    cameraCalibration.open("camera-calibration.xml", FileStorage::READ);
    // Cheacking for success
    if (!cameraCalibration.isOpened())
    {
        std::cout << "Unable to open the camera calibration." << std::endl;
        return 1;
    }
    // Reading the calibration
    cameraCalibration["camera_matrix"] >> cameraMatrix;
    std::cout << "Camera matrix:" << std::endl << cameraMatrix << std::endl;
    cameraCalibration["distortion_coefficients"] >> distCoeffs;
    std::cout << "Distortion coefficients:" << std::endl << distCoeffs << std::endl;
    // Closing the file
    cameraCalibration.release();
    // Opening the camera
    camera.open(CAMERA_ID, CAP_V4L);
    // Checking for success
    if (!camera.isOpened())
    {
        std::cout << "Unable to open the camera." << std::endl;
        return 1;
    }
    // Changing some camera settings
    camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    camera.set(CAP_PROP_FRAME_WIDTH, 1920);
    camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
    // Creating a new window
    namedWindow("Camera viewer");
    // Endless frame capturing and displaying loop
    while (true)
    {
        // Reading a frame
        camera.read(rawFrame);
        // Checking for success
        if (rawFrame.empty())
        {
            std::cout << "Blank frame grabbed." << std::endl;
            return 1;
        }
        // Undistorting the frame
        undistort(rawFrame, undistortedFrame, cameraMatrix, distCoeffs);
        // Showing the frame
        imshow("Camera viewer", undistortedFrame);
        // Breaking the loop on user input
        if (waitKey(1000 / 30) >= 0)
        {
            break;
        }
    }
    // Closing the camera
    camera.release();
    return 0;
}
