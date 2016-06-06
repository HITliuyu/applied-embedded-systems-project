#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img;
Mat templ;
Mat result;
Mat src_img;

char* image_window = "Source Image";
char* result_window = "Result window";
char* filtered_window = "Filtered window";

double THRESHOLD = 0.4;
int match_method = 1;	//CV_TM_SQDIFF_NORMED

/// Function Headers
void MatchingMethod(int, void*);
Mat filterImage(Mat);

const std::string usage =
		"Usage : shared=robovision <path_to_input_image> <path_to_template_image>\n";

/** @function main */

int main(int argc, char** argv) {

	if (argc < 3) {
		std::cerr << "No input image specified\n";
		std::cout << usage;
		return -1;
	}

	/// Load image and template
	img = imread(argv[1], 1);
	templ = imread(argv[2], 1);
	img.copyTo(src_img);

	/// Create windows
	namedWindow(image_window, CV_WINDOW_NORMAL);
	namedWindow(result_window, CV_WINDOW_NORMAL);
	namedWindow(filtered_window, CV_WINDOW_NORMAL);

	MatchingMethod(0, 0);

	waitKey(0);
	return 0;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod(int, void*) {

	/// Source image to display
	Mat img_display;

	// Resets images and keeps a copy of the original image [for testing purposes]
	src_img.copyTo(img);
	src_img.copyTo(img_display);

	// Apply filter to image
	Mat filtered_img = filterImage(img);
	filtered_img.copyTo(img);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	cout << "--------------------------------\n";
	int robot_counter = 0; // initializing counter

	bool keep_checking = true;

	clock_t begin = clock();
	/// Do the Matching and Normalize

	cout << "Matching: Starting...\n";
	matchTemplate(img, templ, result, match_method);

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "Matching: Completed!\n";
	cout << "It took "<< elapsed_secs << "s!\n";

	cout << "Normalization: Starting...\n";
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	cout << "Normalization: Completed!\n";

	while (keep_checking) {
		/// Localizing the best match with minMaxLoc
		double minVal;
		double maxVal;
		Point minLoc;
		Point maxLoc;
		Point matchLoc;

		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

		/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
		if (match_method == CV_TM_SQDIFF
				|| match_method == CV_TM_SQDIFF_NORMED) {
			matchLoc = minLoc;
		} else {
			matchLoc = maxLoc;
		}

		cout << "-\tMinVal = " << minVal << "\tMaxVal = " << maxVal << "\n";

		// TODO: Temporary fix for matchLoc
		matchLoc = Point(matchLoc.x + 60, matchLoc.y + 80);

		// For CV_TM_SQDIFF_NORMED the best matches are the lowest numbers
		if (minVal >= 0 && minVal <= THRESHOLD) {
			rectangle(img_display,
					Point(matchLoc.x - (templ.cols/2), matchLoc.y - (templ.rows/2)),
					cv::Point(matchLoc.x + (templ.cols/2), matchLoc.y + (templ.rows/2)),
					CV_RGB(0,255,0), 2);

			rectangle(result,
					Point(matchLoc.x - templ.cols ,
							matchLoc.y - templ.rows),
					Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows),
					Scalar::all(255), CV_FILLED, 8, 0);

			circle(img_display, matchLoc, 10, CV_RGB(0,255,0), -1);

			robot_counter++;
			cout << robot_counter << " at position:\t[" << matchLoc.x << "," << matchLoc.y << "]\n";
		} else
			keep_checking = false;

	}

	cout << "Found " << robot_counter << " robots in the picture!\n";

	imshow(filtered_window, img);
	imshow(image_window, img_display);
	imshow(result_window, result);

	return;
}

Mat filterImage(Mat src_img) {

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 1;
	int iHighV = 255;

	Mat imgHSV;

	cvtColor(src_img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	Mat imgThresholded;

	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV),
			imgThresholded); //Threshold the image

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

	cvtColor(imgThresholded, imgThresholded, CV_GRAY2BGR);

	// Save image: debug purposes
	//imwrite( "filtered_img.jpg", imgThresholded );

	return imgThresholded;

}
