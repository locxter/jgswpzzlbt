#ifndef OPENCV_HELPERS
#define OPENCV_HELPERS
#include <iostream>
#include <opencv2/opencv.hpp>

// Function to draw text on an image
cv::Mat drawText(cv::Mat image, std::string text);

// Function to show an image and wait for user response
void showImage(std::string windowName, cv::Mat image);

// Function to capture a picture and apply the needed preprocessing
cv::Mat capturePicture(cv::VideoCapture& camera, cv::Mat cameraMatrix, cv::Mat distortionCoefficients);
#endif
