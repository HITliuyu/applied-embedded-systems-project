#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <vector>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "aruco.h"

using namespace cv;
using namespace aruco;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
///// Internal Macros
///////////////////////////////////////////////////////////////////////////////////

// Internal Macros
#define LEFT_CAMERA 0
#define RIGHT_CAMERA 1

// Markers
#define MARKER_SIZE 0.025	// size of the marker (in meters)

#define PLAYER_RADIUS 10	// size of the player shown in the map

// Technical Params
#define ROBOT_HEIGHT 0 	// distance of the marker from the ground (in meters)
#define FIELD_HEIGHT 780	// height of the field image (in pixels)
#define FIELD_WIDTH 496		// width of the field image (in pixels)
#define CAMERA_HEIGHT 3.5	// distance of the camera from the ground (in meters)
#define REAL_FIELD_HEIGHT 4.2	// height of the field (in meters)
#define REAL_FIELD_WIDTH 1.3	// width of the field (in meters)

///////////////////////////////////////////////////////////////////////////////////
///// Variables
///////////////////////////////////////////////////////////////////////////////////

bool DEBUG = true;
bool DEBUG_CAMERA = true;	// use only local cameras

int IMAGE_WIDTH;
int IMAGE_HEIGHT;

Mat field_original;
Mat field;

MarkerDetector markerDetector;

// Left Camera
string intrisicsFile_left;
VideoCapture videoCap_left;
Mat inputImg_left, inputImgCopy_left;
CameraParameters cameraParams_left;
vector<Marker> markers_left;

// Right Camera
string intrisicsFile_right;
VideoCapture videoCap_right;
Mat inputImg_right, inputImgCopy_right;
CameraParameters cameraParams_right;
vector<Marker> markers_right;

int waitTime = 10;

// Log file
String logFile_path = "";
ofstream logger;
bool logging = false;

// Team data
map<string, string> team1;
map<string, string> team2;

// Commanding robots
int selected_id = -1;
bool selected = false;
Point target_location;
bool target_selected = false;
bool command_sent = false;
bool local_only = false;	// true -> don't send command to Nao

///////////////////////////////////////////////////////////////////////////////////
///// Function Declarations
///////////////////////////////////////////////////////////////////////////////////

void rotate(cv::Mat& src, float angle, cv::Mat& dst);
void getNextFrame(Mat &image, int camera, bool debug = false);
void processFrame(Mat &image, int camera, vector<Marker> markers);
void display(Mat &image, int camera);
void processKey(char k);
bool initCameras(bool debug = false);
bool initGUI();
bool readArguments(int argc, char **argv);
vector<float> getMarkerData(Mat &Image, Marker &M, const CameraParameters &CP,
		int camera, bool setYperpendicular = false);
int getTeamNumber(String id);
int getTeamNumber(int id);
Scalar getColourFromString(String colour);
int getIdFromPoint(int x, int y, vector<Marker> &markers, int player_radius, int camera);
String getIPPlayer(int id);

#endif // CONFIG_H_INCLUDED
