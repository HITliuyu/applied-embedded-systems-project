// Credit to: https://github.com/Itseez/opencv/blob/master/samples/cpp/tutorial_code/ImgTrans/HoughCircle_Demo.cpp

#include "BallFinder.h"
#include <iostream>

int iLowH = 0;
int iHighH = 179;

int iLowS = 0;
int iHighS = 187;

int iLowV = 0;
int iHighV = 237;

namespace {

// initial and max values of the parameters of interests.
const int cannyThresholdInitialValue = 200;
const int accumulatorThresholdInitialValue = 10;
const int maxAccumulatorThreshold = 200;
const int maxCannyThreshold = 255;

Point HoughDetection(const Mat& src_filtered, int cannyThreshold,
		int accumulatorThreshold) {
	// will hold the results of the detection
	vector<Vec3f> circles;
	// runs the actual detection
	HoughCircles(src_filtered, circles, CV_HOUGH_GRADIENT, 1,
			src_filtered.rows / 8, cannyThreshold, accumulatorThreshold, 10,
			40);

	Point ball_center;

	if(circles.size() > 0)
		{
			ball_center.x = cvRound(circles[0][0]);
			ball_center.y = cvRound(circles[0][1]);
		}
	else
	{
		ball_center.x = -1;
		ball_center.y = -1;
	}

	return ball_center;
}
}

Point findBall(Mat &img) {

	Mat imgHSV, imgThresholded;

	cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

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

	// Reduce the noise so we avoid false circle detection
	GaussianBlur(imgThresholded, imgThresholded, Size(9, 9), 2, 2);

	// Declare and initialize both parameters that are subjects to change
	int cannyThreshold = cannyThresholdInitialValue;
	int accumulatorThreshold = accumulatorThresholdInitialValue;

	// so we must check here
	cannyThreshold = max(cannyThreshold, 1);
	accumulatorThreshold = max(accumulatorThreshold, 1);

	// find ball
	return HoughDetection(imgThresholded, cannyThreshold, accumulatorThreshold);

}
