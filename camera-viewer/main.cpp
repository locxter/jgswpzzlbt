#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int main()
{
    // Creating a new image container and camera object
    Mat frame;
    VideoCapture camera;
    // Opening the camera
    camera.open(0, CAP_V4L);
    // Check for success
    if (!camera.isOpened())
    {
        std::cout << "Unable to open the camera." << std::endl;
        return 1;
    }
    std::cout << "Camera opened successfully." << std::endl;
    // Changing some camera settings
    camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    camera.set(CAP_PROP_FRAME_WIDTH, 1920);
    camera.set(CAP_PROP_FRAME_HEIGHT, 1080);
    camera.set(CAP_PROP_FPS, 30);
    // Creating a new window
    namedWindow("Camera viewer");
    // Endless fram capturing and displaying loop
    while (true)
    {
        // Reading a frame
        camera.read(frame);
        // Check for success
        if (frame.empty()) {
            std::cout << "Blank frame grabbed." << std::endl;
            return 1;
        }
        // Showing the frame
        imshow("Camera", frame);
        // Breaking the loop on user input
        if (waitKey(1000 / 30) >= 0)
        {
            break;
        }
    }
    return 0;
}
