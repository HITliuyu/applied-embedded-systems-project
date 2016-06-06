#include <unistd.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>
using namespace cv;
using namespace std;


///////////////////////////////////////////////////////////////////////////////////
///// Internal Macros
///////////////////////////////////////////////////////////////////////////////////
#define FIELD_HEIGHT 780	// height of the field image (in pixels)
#define FIELD_WIDTH 496		// width of the field image (in pixels)

///////////////////////////////////////////////////////////////////////////////////
///// Variables
///////////////////////////////////////////////////////////////////////////////////

bool DEBUG = true;

int SLEEP_TIMER = 20 * 1000;	// in u-seconds

Mat field_original;
Mat field;

// Log file
String logFile_path = "";
ifstream logReader;

float waitTime = 1;
float waitTime_old = 1;

// Team data
map<string, string> team1;
map<string, string> team2;

///////////////////////////////////////////////////////////////////////////////////
///// Function Declarations
///////////////////////////////////////////////////////////////////////////////////

int initGUI();
void readTeamsFromFile(string filePath, map<string, string> &team1, map<string, string> &team2, bool debug) throw(cv::Exception);
int getTeamNumber(String id);
Scalar getColourFromString(String colour);
