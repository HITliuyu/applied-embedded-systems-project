#ifndef FIELDUTILS_H_
#define FIELDUTILS_H_

#include <vector>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "aruco.h"

using namespace cv;
using namespace aruco;

// Internal Macros
#define LEFT_CAMERA 0
#define RIGHT_CAMERA 1

int getIdFromPoint(int x, int y, vector<Marker> &markers, int player_radius, int field_height, int image_height, int camera);
bool isTargetReached(Point p, int player_radius, Point t, int field_height, int image_height, int camera);
Point getFieldPoint(float x, float y, int field_height, int image_height, int camera);
vector<float> getDistanceAngleRealField(float x1, float y1, float x2, float y2, int image_height, int image_width, float real_height, float real_width);
float calculateDistance(float x1, float y1, float x2, float y2);
float calculateRadians(float x1, float y1, float x2, float y2);
float getRadiansFromDegree(float deg);

#endif /* FIELDUTILS_H_ */
