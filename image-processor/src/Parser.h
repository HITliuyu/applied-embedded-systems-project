#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;

void readTeamsFromFile(string filePath, map<string, string> &team1, map<string, string> &team2, bool debug) throw(cv::Exception);
vector<string> split(const string &s, char delim);

#endif /* PARSER_H_ */
