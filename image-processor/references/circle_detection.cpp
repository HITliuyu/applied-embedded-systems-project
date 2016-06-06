/**
 * @file HoughCircle_Demo.cpp
 * @brief Demo code for Hough Transform
 * @author OpenCV team
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

namespace {
// windows and trackbars name
const std::string windowName = "Hough Circle Detection Demo";
const std::string cannyThresholdTrackbarName = "Canny threshold";
const std::string accumulatorThresholdTrackbarName = "Accumulator Threshold";
const std::string usage =
		"Usage : tutorial_HoughCircle_Demo <path_to_input_image>\n";

// initial and max values of the parameters of interests.
const int cannyThresholdInitialValue = 200;
const int accumulatorThresholdInitialValue = 10;
const int maxAccumulatorThreshold = 200;
const int maxCannyThreshold = 255;

void HoughDetection(const Mat& src_filtered, const Mat& src_display,
		int cannyThreshold, int accumulatorThreshold) {
	// will hold the results of the detection
	std::vector<Vec3f> circles;
	// runs the actual detection
	HoughCircles(src_filtered, circles, CV_HOUGH_GRADIENT, 1,
			src_filtered.rows / 8, cannyThreshold, accumulatorThreshold, 0,
			0);

	// clone the colour, input image for displaying purposes
	Mat display = src_display.clone();
	for (size_t i = 0; i < circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(display, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(display, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	}

	// shows the results
	imshow(windowName, display);
}
}

int main(int argc, char** argv) {
	Mat src, src_gray;

	if (argc < 2) {
		std::cerr << "No input image specified\n";
		std::cout << usage;
		return -1;
	}

	// Read the image
	src = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);

	if (src.empty()) {
		std::cerr << "Invalid input image\n";
		std::cout << usage;
		return -1;
	}

	Mat imgHSV;

	cvtColor(src, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	Mat imgThresholded;

	// Threshold the HSV image, keep only the red pixels
	Mat lower_red_hue_range;
	Mat upper_red_hue_range;
	inRange(imgHSV, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255),
			lower_red_hue_range);
	inRange(imgHSV, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255),
			upper_red_hue_range);

	// Combine the above two images
	cv::Mat red_hue_image;
	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0,
			imgThresholded);

	//morphological opening (remove small objects from the foreground)
	erode(imgThresholded, imgThresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(imgThresholded, imgThresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (fill small holes in the foreground)
	dilate(imgThresholded, imgThresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded,
			getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	// Convert it to gray
	//  cvtColor( src, src_gray, CV_LOAD_IMAGE_UNCHANGED );

	// Reduce the noise so we avoid false circle detection
	 GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

	//declare and initialize both parameters that are subjects to change
	int cannyThreshold = cannyThresholdInitialValue;
	int accumulatorThreshold = accumulatorThresholdInitialValue;

	// create the main window, and attach the trackbars
	namedWindow(windowName, WINDOW_AUTOSIZE);
	createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold,
			maxCannyThreshold);
	createTrackbar(accumulatorThresholdTrackbarName, windowName,
			&accumulatorThreshold, maxAccumulatorThreshold);

	// infinite loop to display
	// and refresh the content of the output image
	// until the user presses q or Q
	int key = 0;
	while (key != 'q' && key != 'Q') {
		// those paramaters cannot be =0
		// so we must check here
		cannyThreshold = std::max(cannyThreshold, 1);
		accumulatorThreshold = std::max(accumulatorThreshold, 1);

		//runs the detection, and update the display
		HoughDetection(imgThresholded, src, cannyThreshold,
				accumulatorThreshold);
		// get user key
		key = waitKey(10);
	}

	return 0;
}
