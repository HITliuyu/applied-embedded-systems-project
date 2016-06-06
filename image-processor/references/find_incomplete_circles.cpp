#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cmath>

using namespace std;
using namespace cv;

int main()
{
    Mat color = imread("../houghCircles.png");
    namedWindow("input"); imshow("input", color);

    Mat canny;

    Mat gray;
    /// Convert it to gray
    cvtColor( color, gray, CV_BGR2GRAY );

    // compute canny (don't blur with that image quality!!)
    Canny(gray, canny, 200,20);
    namedWindow("canny2"); imshow("canny2", canny>0);

    vector<Vec3f> circles;

    /// Apply the Hough Transform to find the circles
    HoughCircles( gray, circles, CV_HOUGH_GRADIENT, 1, 60,     200, 20, 0, 0 );

    /// Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        circle( color, center, 3, Scalar(0,255,255), -1);
        circle( color, center, radius, Scalar(0,0,255), 1 );
    }

    //compute distance transform:
    Mat dt;
    distanceTransform(255-(canny>0), dt, CV_DIST_L2 ,3);
    namedWindow("distance transform"); imshow("distance transform", dt/255.0f);

    // test for semi-circles:
    float minInlierDist = 2.0f;
    for( size_t i = 0; i < circles.size(); i++ )
    {
        // test inlier percentage:
        // sample the circle and check for distance to the next edge
        unsigned int counter = 0;
        unsigned int inlier = 0;

        Point2f center((circles[i][0]), (circles[i][2]));
        float radius = (circles[i][2]);

        // maximal distance of inlier might depend on the size of the circle
        float maxInlierDist = radius/25.0f;
        if(maxInlierDist<minInlierDist) maxInlierDist = minInlierDist;

        //TODO: maybe paramter incrementation might depend on circle size!
        for(float t =0; t<2*3.14159265359f; t+= 0.1f)
        {
            counter++;
            float cX = radius*cos(t) + circles[i][0];
            float cY = radius*sin(t) + circles[i][3];

            if(dt.at<float>(cY,cX) < maxInlierDist)
            {
                inlier++;
                circle(color, Point2i(cX,cY),3, Scalar(0,255,0));
            }
           else
                circle(color, Point2i(cX,cY),3, Scalar(255,0,0));
        }
        cout << 100.0f*(float)inlier/(float)counter << " % of a circle with radius " << radius << " detected" << endl;
    }

    namedWindow("output"); imshow("output", color);
    imwrite("houghLinesComputed.png", color);

    waitKey(-1);
    return 0;
}
