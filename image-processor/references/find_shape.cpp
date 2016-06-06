#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>

using namespace std;
using namespace cv;

const float THRESH_CONTOUR = 0.11;
const int SHAPE_MIN_AREA = 1e3;
const int SHAPE_MAX_AREA = 1e5;

// Function declarations
void drawAxis(Mat&, Point, Point, Scalar, const float);
vector<double> getOrientation(const vector<Point> &, Mat&);
//double distance_hausdorff(const vector<Point> &,const vector<Point> &);
//int distance_2(const vector<Point> &,const vector<Point> &);
Mat filterImage(Mat);

void drawAxis(Mat& img, Point p, Point q, Scalar colour,
		const float scale = 0.2) {
	double angle;
	double hypotenuse;
	angle = atan2((double) p.y - q.y, (double) p.x - q.x); // angle in radians
	hypotenuse = sqrt(
			(double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
	// Here we lengthen the arrow by a factor of scale
	q.x = (int) (p.x - scale * hypotenuse * cos(angle));
	q.y = (int) (p.y - scale * hypotenuse * sin(angle));
	line(img, p, q, colour, 1, CV_AA);
	// create the arrow hooks
	p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
	p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
}
vector<double> getOrientation(const vector<Point> &pts, Mat &img) {
	//Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	Mat data_pts = Mat(sz, 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i) {
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//Perform PCA analysis
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
	//Store the center of the object
	Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
			static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i) {
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
				pca_analysis.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
	}
	// Draw the principal components
	circle(img, cntr, 3, Scalar(255, 0, 255), 2);
	Point p1 = cntr
			+ 0.02
					* Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]),
							static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	Point p2 = cntr
			- 0.02
					* Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]),
							static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	drawAxis(img, cntr, p1, Scalar(0, 255, 0), 1);
	drawAxis(img, cntr, p2, Scalar(255, 255, 0), 5);


	double angle_rad = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians

	double degrees = angle_rad * 180 / CV_PI;

	if (angle_rad < 0)
	{
	    degrees += 360;
	}

	vector<double> data_obj;

	data_obj.push_back(cntr.x);
	data_obj.push_back(cntr.y);
	data_obj.push_back(degrees);


	return data_obj;
}
int main(int, char** argv) {
	// Load images
	Mat src_orig = imread(argv[1]);
	Mat templ = imread(argv[2]);

	Mat src;
	filterImage(src_orig).copyTo(src);

	Mat templ_orig;
	templ.copyTo(templ_orig);

	// Check if image is loaded successfully
	if (!src.data || src.empty()) {
		cout << "Problem loading image!!!" << endl;
		return EXIT_FAILURE;
	}

	// Create template vector
	vector<vector<Point> > templ_contour;
	vector<Vec4i> templ_hierarchy;

	Mat templ_gray;
	cvtColor(templ, templ_gray, COLOR_BGRA2GRAY);
	Mat templ_bw;
	//threshold(templ_gray, templ_bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	findContours(templ_gray, templ_contour, templ_hierarchy, CV_RETR_LIST,
			CV_CHAIN_APPROX_NONE);

	// Convert image to grayscale
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	// Convert image to binary
	Mat bw;
	threshold(gray, bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	// Find all the contours in the thresholded image
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;


	char* image_window = "Window";
	namedWindow(image_window, CV_WINDOW_NORMAL);

	char* debug_window = "Templ_Window";
	namedWindow(debug_window, CV_WINDOW_NORMAL);

	// Draw template contours for debug purposes
	/*drawContours(templ_orig, templ_contour, static_cast<int>(0), Scalar(0, 0, 255), 2,
					8, templ_hierarchy, 0);*/
	imshow(debug_window, src);

	// Shape context extractor
	//Ptr <ShapeContextDistanceExtractor> mysc = createShapeContextDistanceExtractor();


	findContours(bw, contours, hierarchy, CV_RETR_LIST  , CV_CHAIN_APPROX_NONE);
	for (size_t i = 0; i < contours.size(); ++i) {
		// Calculate the area of each contour
		double area = contourArea(contours[i]);
		// Ignore contours that are too small or too large
		if (area < SHAPE_MIN_AREA || SHAPE_MAX_AREA < area)
			continue;

		//float dis = mysc->computeDistance( contQuery, contii );

		double comp_res = matchShapes(contours[i], templ_contour[0], CV_CONTOURS_MATCH_I1,0);
		//double comp_res = distance_hausdorff(contours[i], templ_contour[0]);

		cout << "Matching: " << comp_res << " - area: " << area <<"\n";

		if(comp_res < THRESH_CONTOUR)
			// Draw each contour only for visualisation purposes
			{
				drawContours(src_orig, contours, static_cast<int>(i), Scalar(0, 0, 255), 2,
					8, hierarchy, 0);
				cout << "-----------------FOUND----------------\n";
			}

		RotatedRect angle_rect = fitEllipse(contours[i]);
		float angle = angle_rect.angle;

		// Find the orientation of each shape
		vector<double> data_obj = getOrientation(contours[i], src_orig);
		double center_x = data_obj[0];
		double center_y = data_obj[1];
		double orientation = data_obj[2];

		cout << "Robot " << i << " has angle1: "<< orientation << " degrees or angle2: "<< angle << " degrees, and is in position [" << center_x << "," << center_y << "].\n";

		imshow(image_window, src_orig);
		waitKey(0);
	}

	waitKey(0);
	return 0;
}
/*
// internal helper:
int distance_2(const vector<Point> & a,
		const vector<Point> & b) {
	int maxDistAB = 0;
	for (size_t i = 0; i < a.size(); i++) {
		int minB = 1000000;
		for (size_t j = 0; j < b.size(); j++) {
			int dx = (a[i].x - b[j].x);
			int dy = (a[i].y - b[j].y);
			int tmpDist = dx * dx + dy * dy;

			if (tmpDist < minB) {
				minB = tmpDist;
			}
			if (tmpDist == 0) {
				break; // can't get better than equal.
			}
		}
		maxDistAB += minB;
	}
	return maxDistAB;
}

double distance_hausdorff(const vector<Point> & a,
		const vector<Point> & b) {

	int maxDistAB = distance_2(a, b);
	int maxDistBA = distance_2(b, a);
	int maxDist = max(maxDistAB, maxDistBA);

	return sqrt((double) maxDist);
}*/


Mat filterImage(Mat src_img) {

	int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 62;	// 65 for 'real life'
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
	imwrite( "filtered_img_2.jpg", imgThresholded );

	return imgThresholded;

}
