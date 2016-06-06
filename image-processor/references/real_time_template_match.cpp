#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <sstream>


using namespace cv;
using namespace std;
/*
Point point1, point2; // vertical points of the bounding box
int drag = 0;
Rect rect; // bounding box
Mat img, roiImg; // roiImg - the part of the image in the bounding box
int select_flag = 0;
bool go_fast = false;

Mat mytemplate;

void track(cv::Mat &img, const cv::Mat &templ, const cv::Rect &r )
{
    static int n = 0;

    if (select_flag)
    {
        templ.copyTo(mytemplate);
        select_flag = false;
        go_fast = true;
    }


    cv::Mat result;
    /// Do the Matching and Normalize
    matchTemplate( img, mytemplate, result, CV_TM_SQDIFF_NORMED );
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    matchLoc = minLoc;

    rectangle( img, matchLoc, Point( matchLoc.x + mytemplate.cols , matchLoc.y + mytemplate.rows ), CV_RGB(255, 255, 255), 3 );

    std::cout << matchLoc << "\n";
}

///MouseCallback function

void mouseHandler(int event, int x, int y, int flags, void *param)
{
    if (event == CV_EVENT_LBUTTONDOWN && !drag)
    {
        // left button clicked. ROI selection begins
        point1 = Point(x, y);
        drag = 1;
    }

    if (event == CV_EVENT_MOUSEMOVE && drag)
    {
        // mouse dragged. ROI being selected
        Mat img1 = img.clone();
        point2 = Point(x, y);
        rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 3, 8, 0);
        imshow("image", img1);
    }

    if (event == CV_EVENT_LBUTTONUP && drag)
    {
        point2 = Point(x, y);
        rect = Rect(point1.x, point1.y, x - point1.x, y - point1.y);
        drag = 0;
        roiImg = img(rect);
    }

    if (event == CV_EVENT_LBUTTONUP)
    {
        // ROI selected
        select_flag = 1;
        drag = 0;
    }

}


///Main function

int mainx()
{
    int k;

     //   VideoCapture cap(0);
     //   if (!cap.isOpened())
     //   return 1;

    VideoCapture cap;
    //cap.open("~/Downloads/opencv-2.4.4/samples/cpp/tutorial_code/HighGUI/video-input-psnr-ssim/video/Megamind.avi");
    cap.open("./Megamind.avi");
    if (!cap.isOpened())
    {
        cout <<"Unable to open video file\n";
        return -1;
    }


        // Set video to 320x240
     //   cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
     //   cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);


    cap >> img;
    imshow("image", img);

    while (1)
    {
        cap >> img;
        if (img.empty())
            break;

        if (rect.width == 0 && rect.height == 0)
            cvSetMouseCallback("image", mouseHandler, NULL);
        else
            track(img, roiImg, rect);

        if (select_flag == 1)
            imshow("Template", roiImg);

        imshow("image", img);
        k = waitKey(go_fast ? 30 : 10000);
        if (k == 27)
            break;

    }


    return 0;
}*/
