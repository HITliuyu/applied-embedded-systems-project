#ifndef BALLFINDER_H_
#define BALLFINDER_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace cv;

Point findBall(Mat &img);

#endif /* BALLFINDER_H_ */
