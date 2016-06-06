#include "Config.h"
#include "Parser.h"
#include "BallFinder.h"
#include "FieldUtils.h"
#include "UDPImageFetcher.h"
#include "NaoController.h"
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <ctime>


template <typename T>
  string NumberToString ( T Number )
  {
	return boost::lexical_cast<std::string>(Number);
  }

int main(int argc, char **argv) {

	if (!readArguments(argc, argv))
		return 0;

	logger.open(logFile_path.c_str(), ofstream::out | ofstream::trunc);

	try {
		// Initializes the cameras
		if (!initCameras(DEBUG_CAMERA))
			return 0;

		//Create gui
		if (!initGUI())
			return 0;

		char key = 0;

		// Initializes frame count
		uint frame_count = 1;

		// Capture until press ESC
		do {

			if (logging)
				logger << "Frame:" << frame_count << "\n";

			field_original.copyTo(field);

			// Get next frame
			getNextFrame(inputImg_left, LEFT_CAMERA, DEBUG_CAMERA);
			getNextFrame(inputImg_right, RIGHT_CAMERA, DEBUG_CAMERA);

			rotate(inputImg_left,180, inputImg_left);
			rotate(inputImg_right, 180, inputImg_right);

			// Create virtual copy
			inputImg_left.copyTo(inputImgCopy_left);
			inputImg_right.copyTo(inputImgCopy_right);

			// Detect markers
			markerDetector.detect(inputImg_left, markers_left,
					cameraParams_left, MARKER_SIZE);
			markerDetector.detect(inputImg_right, markers_right,
					cameraParams_right, MARKER_SIZE);

			// Process the frames and show them
			if (!inputImgCopy_left.empty()) {
				processFrame(inputImgCopy_left, LEFT_CAMERA, markers_left);
				display(inputImgCopy_left, LEFT_CAMERA);
			}

			if (!inputImgCopy_right.empty()) {
				processFrame(inputImgCopy_right, RIGHT_CAMERA, markers_right);
				display(inputImgCopy_right, RIGHT_CAMERA);
			}

			if (logging) {
				logger << "#\n";
				frame_count++;

				// Show frame count
				Point text_pos;
				text_pos.x = FIELD_WIDTH - 120;
				text_pos.y = 40;

				// Converting frame count to str
				char fc[200];
				sprintf(fc, "%d", frame_count);
				putText(field, fc, text_pos, FONT_HERSHEY_PLAIN, 3,
						Scalar::all(0), 2, CV_AA);
			}

			// Show team names
			Point text_pos;
			text_pos.x = 10;
			text_pos.y = 20;

			putText(field, team1["name"] + " vs " + team2["name"], text_pos, FONT_HERSHEY_PLAIN, 1,
					Scalar::all(255), 1, CV_AA);

			imshow("field", field);

			key = waitKey(waitTime); //wait for key to be pressed
			processKey(key);
		} while (key != 27);

	} catch (exception &ex) {
		cout << "Exception :" << ex.what() << endl;
	}

	logger.close();

}

bool readArguments(int argc, char **argv) {

	if (argc < 4) {
		cerr << "Invalid number of arguments" << endl;
		cerr << "Usage: intrinsics_left.yml intrinsics_right.yml teams_info.yml [log_file.txt]"
				<< endl;
		return false;
	}
	intrisicsFile_left = argv[1];
	intrisicsFile_right = argv[2];

	readTeamsFromFile(argv[3], team1, team2, DEBUG);

	if (argc == 5)
		logFile_path = argv[4];

	return true;
}

void processKey(char k) {
	switch (k) {
	case 's':
		if (waitTime == 0)
			waitTime = 10;
		else
			waitTime = 0;
		break;
	case 'l':
		logging = !logging;
		if (logging)
			cout << "Logger is ON\n";
		else
			cout << "Logger is OFF\n";
		break;
	}
}

void onClick(int event, int x, int y, int flags, void* userdata)
{
     if  ( event == EVENT_LBUTTONDOWN  && !selected)
     {
          if(y < FIELD_HEIGHT/2)
        	  selected_id = getIdFromPoint(x, y, markers_left, PLAYER_RADIUS, FIELD_HEIGHT, IMAGE_HEIGHT, LEFT_CAMERA);
          else
        	  selected_id = getIdFromPoint(x, y, markers_right, PLAYER_RADIUS, FIELD_HEIGHT, IMAGE_HEIGHT, RIGHT_CAMERA);

          if(selected_id != -1)
              selected = true;
     }
     else if (event == EVENT_LBUTTONDOWN)
     {
    	 selected_id = -1;
    	 selected = false;
    	 target_selected = false;
     }

     // Select target position for nao
     else if  ( (event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) && selected)
     {
          target_location.x = x;
          target_location.y = y;
          target_selected = true;
          command_sent = false;

          cout << "Sending command to Robot: " << (NumberToString(selected_id) + "_ip") << " at IP " << getIPPlayer(selected_id) << "." << endl;

     }

     /*else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if ( event == EVENT_MOUSEMOVE )
     {
          cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }*/
}

bool initGUI() {
	namedWindow("in_left", 1);
	namedWindow("in_right", 1);

	namedWindow("field", 1);
	setMouseCallback("field", onClick, NULL);

	cout << "Loading images...\n";
	field_original = imread("res/field.png");

	if (field_original.empty()) {
		cout
				<< "Error loading the images. Did you forget to include the folder?\n";
		return false;
	}
	cout << "Images loaded.\n";

	return true;
}

void rotate(cv::Mat& src, float angle, cv::Mat& dst) {
	cv::Point2f pt(src.cols / 2., src.rows / 2.);
	cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

	cv::warpAffine(src, dst, r, cv::Size(src.cols, src.rows));
}

vector<float> getMarkerData(Mat &Image, Marker &m, const CameraParameters &CP,
		int camera, bool setYperpendicular) {
	Mat objectPoints(8, 3, CV_32FC1);
	double halfSize = m.ssize / 2;

	if (setYperpendicular) {
		objectPoints.at<float>(0, 0) = -halfSize;
		objectPoints.at<float>(0, 1) = 0;
		objectPoints.at<float>(0, 2) = -halfSize;
		objectPoints.at<float>(1, 0) = halfSize;
		objectPoints.at<float>(1, 1) = 0;
		objectPoints.at<float>(1, 2) = -halfSize;
		objectPoints.at<float>(2, 0) = halfSize;
		objectPoints.at<float>(2, 1) = 0;
		objectPoints.at<float>(2, 2) = halfSize;
		objectPoints.at<float>(3, 0) = -halfSize;
		objectPoints.at<float>(3, 1) = 0;
		objectPoints.at<float>(3, 2) = halfSize;

		objectPoints.at<float>(4, 0) = -halfSize;
		objectPoints.at<float>(4, 1) = m.ssize;
		objectPoints.at<float>(4, 2) = -halfSize;
		objectPoints.at<float>(5, 0) = halfSize;
		objectPoints.at<float>(5, 1) = m.ssize;
		objectPoints.at<float>(5, 2) = -halfSize;
		objectPoints.at<float>(6, 0) = halfSize;
		objectPoints.at<float>(6, 1) = m.ssize;
		objectPoints.at<float>(6, 2) = halfSize;
		objectPoints.at<float>(7, 0) = -halfSize;
		objectPoints.at<float>(7, 1) = m.ssize;
		objectPoints.at<float>(7, 2) = halfSize;
	} else {
		objectPoints.at<float>(0, 0) = -halfSize;
		objectPoints.at<float>(0, 1) = -halfSize;
		objectPoints.at<float>(0, 2) = 0;
		objectPoints.at<float>(1, 0) = halfSize;
		objectPoints.at<float>(1, 1) = -halfSize;
		objectPoints.at<float>(1, 2) = 0;
		objectPoints.at<float>(2, 0) = halfSize;
		objectPoints.at<float>(2, 1) = halfSize;
		objectPoints.at<float>(2, 2) = 0;
		objectPoints.at<float>(3, 0) = -halfSize;
		objectPoints.at<float>(3, 1) = halfSize;
		objectPoints.at<float>(3, 2) = 0;

		objectPoints.at<float>(4, 0) = -halfSize;
		objectPoints.at<float>(4, 1) = -halfSize;
		objectPoints.at<float>(4, 2) = m.ssize;
		objectPoints.at<float>(5, 0) = halfSize;
		objectPoints.at<float>(5, 1) = -halfSize;
		objectPoints.at<float>(5, 2) = m.ssize;
		objectPoints.at<float>(6, 0) = halfSize;
		objectPoints.at<float>(6, 1) = halfSize;
		objectPoints.at<float>(6, 2) = m.ssize;
		objectPoints.at<float>(7, 0) = -halfSize;
		objectPoints.at<float>(7, 1) = halfSize;
		objectPoints.at<float>(7, 2) = m.ssize;
	}

	vector<Point2f> imagePoints;
	projectPoints(objectPoints, m.Rvec, m.Tvec, CP.CameraMatrix, CP.Distorsion,
			imagePoints);

	circle(Image, imagePoints[0], 10, Scalar(0, 0, 255, 255), 1, CV_AA);

	// Calculate Center

	float size = m.ssize * 3;
	Mat objectPointsCircle(4, 3, CV_32FC1);
	objectPointsCircle.at<float>(0, 0) = 0;
	objectPointsCircle.at<float>(0, 1) = 0;
	objectPointsCircle.at<float>(0, 2) = 0;
	objectPointsCircle.at<float>(1, 0) = size;
	objectPointsCircle.at<float>(1, 1) = 0;
	objectPointsCircle.at<float>(1, 2) = 0;
	objectPointsCircle.at<float>(2, 0) = 0;
	objectPointsCircle.at<float>(2, 1) = size;
	objectPointsCircle.at<float>(2, 2) = 0;
	objectPointsCircle.at<float>(3, 0) = 0;
	objectPointsCircle.at<float>(3, 1) = 0;
	objectPointsCircle.at<float>(3, 2) = size;

	vector<Point2f> imagePointsCircle;
	projectPoints(objectPointsCircle, m.Rvec, m.Tvec, CP.CameraMatrix,
			CP.Distorsion, imagePointsCircle);
	circle(Image, imagePointsCircle[0], 10, Scalar(0, 0, 255, 255), 1, CV_AA);

	// Get rotation
	float deltaX = imagePoints[0].x - imagePointsCircle[0].x;
	float deltaY = imagePoints[0].y - imagePointsCircle[0].y;

	float rot;
	if (camera == LEFT_CAMERA)
		rot = (((atan2(deltaY, deltaX) + CV_PI / 4) * 180 / CV_PI));
	else
		rot = (((atan2(deltaY, deltaX) + CV_PI / 4 + CV_PI) * 180 / CV_PI));

	vector<float> imageData;
	imageData.push_back(imagePointsCircle[0].x);
	imageData.push_back(imagePointsCircle[0].y);
	imageData.push_back(rot);

	return imageData;

}

void getNextFrame(Mat &image, int camera, bool debug) {

	if(debug)
	{
		if (camera == LEFT_CAMERA) {
			videoCap_left.grab();
			videoCap_left.retrieve(image);
		} /*else if (debug && camera == RIGHT_CAMERA) {
			inputImg_left.copyTo(image);
		} */else if (camera == RIGHT_CAMERA) {

			if(videoCap_right.isOpened()){
				videoCap_right.grab();
				videoCap_right.retrieve(image);
			}
			else
			{
				videoCap_left.grab();
				videoCap_left.retrieve(image);
			}
		}
	}
	else
	{
		getCurrentFrame(image, camera);
	}

}

bool initCameras(bool debug) {


	if (debug){
		videoCap_left.open(1);
		videoCap_right.open(2);  // << CHANGE FOR DEMO

		//check video is open
		if (!videoCap_left.isOpened()) {
			cerr << "Could not open Left video" << endl;
			return false;
		}

		if (!debug && !videoCap_right.isOpened()) {
			cerr << "Could not open Right video" << endl;
			return false;
		}
	}
	else
	{
		if(!initUDPFetcher(DEBUG))
			return false;
	}

	//read first image to get the dimensions
	getNextFrame(inputImg_left, LEFT_CAMERA, DEBUG_CAMERA);
	getNextFrame(inputImg_right, RIGHT_CAMERA, DEBUG_CAMERA);

	IMAGE_HEIGHT = inputImg_left.size().height;
	IMAGE_WIDTH = inputImg_left.size().width;

	if (DEBUG) {
		cout << "Image Height: " << IMAGE_HEIGHT << "\n";
		cout << "Image Width: " << IMAGE_WIDTH << "\n";
	}

	//read LEFT camera parameters
	cameraParams_left.readFromXMLFile(intrisicsFile_left);
	cameraParams_left.resize(inputImg_left.size());

	//read RIGHT camera parameters
	cameraParams_right.readFromXMLFile(intrisicsFile_right);
	cameraParams_right.resize(inputImg_right.size());

	return true;
}

void processFrame(Mat &image, int camera, vector<Marker> markers) {

	uint i;
	int team_number;
	Scalar team_colour;
	String team_name;
	String player_number;

	// Find ball
	Point c = findBall(image);

	if(c.x != -1 && c.y != -1){
		Point ball_center = getFieldPoint(c.x, c.y, FIELD_HEIGHT, IMAGE_HEIGHT, camera);
		circle(image, c, 10, Scalar(0,0,255), -1, CV_AA);
		circle(field, ball_center, 10, Scalar(0,0,255), -1, CV_AA);
		if(DEBUG)
			cout << "Found ball at: (" << ball_center <<").\n";

		if(logging)
			logger << "ball" << ";" << ball_center.x << ";" << ball_center.y << ";" << "\n";
	}

	// Checking camera markers
	for (i = 0; i < markers.size(); i++) {

		if(DEBUG)
			markers[i].draw(image, Scalar(0, 0, 255), 1);

		vector<float> imageData = getMarkerData(image, markers[i],
				cameraParams_left, camera);

		if (DEBUG) {
			cout << "Found ID: " << markers[i].id << ".\n";
			cout << "Center is: (" << imageData[0] << "," << imageData[1]
					<< ").\n";
			cout << "Rotation is: " << imageData[2] << ".\n";
		}

		team_number = getTeamNumber(NumberToString(markers[i].id));

		if(team_number == 1)
		{
			team_name = team1["s_name"];
			team_colour = getColourFromString(team1["colour"]);
			player_number = team1[NumberToString(markers[i].id)];
		}
		else if(team_number == 2)
		{
			team_name = team2["s_name"];

			team_colour = getColourFromString(team2["colour"]);
			player_number = team2[NumberToString(markers[i].id)];
		}
		else
		{
			team_name = "UNK";
			team_colour = Scalar(255,255,255);
			player_number = "00";
		}

		// Simple drawing of the center of the marker
		Point center = getFieldPoint(imageData[0], imageData[1], FIELD_HEIGHT, IMAGE_HEIGHT, camera);

		// Logging the data
		if (logging)
			logger << markers[i].id << ";" << center.x << ";" << center.y << ";"
					<< imageData[2] << "\n";

		// Check if the current marker has been selected by the user
		if(markers[i].id == selected_id)
			{
				circle(field, center, PLAYER_RADIUS, team_colour, -1, CV_AA);

				// If the user has selected a target for the robot
				if(target_selected)
					{
						if(!isTargetReached(center, PLAYER_RADIUS, target_location, FIELD_HEIGHT, IMAGE_HEIGHT, camera))
						{
							line(field, center, target_location,Scalar::all(0), 1, CV_AA);
							circle(field, target_location, 2, Scalar::all(0), -1, CV_AA);

							try{
									// If the command to the Nao has not been sent yet
									if(!command_sent && !local_only)
									{

										// Get the real distance in meters and the angle in radians
										// And send it to the Nao Controller
										vector<float> movement_data = getDistanceAngleRealField(center.x, center.y, target_location.x, target_location.y, IMAGE_HEIGHT, IMAGE_WIDTH, REAL_FIELD_HEIGHT, REAL_FIELD_WIDTH);

										cout << "Actual angle: " << movement_data[1] << " - angle marker: " << getRadiansFromDegree(imageData[2]) << endl;

										// Sends command to move the selected robot at the target location, taking into account the rotation the robot
										moveRobot(getIPPlayer(selected_id), movement_data[0], (movement_data[1] - getRadiansFromDegree(imageData[2])));

										command_sent = true;
									}
									else if(local_only)
										command_sent = true;
									}
							catch (int e)
							  {
								cout << "An exception occurred sending Packet to Nao." << endl;

							  }
						}
						else
							target_selected = false;
					}
			}
		else
			circle(field, center, PLAYER_RADIUS, team_colour, 1, CV_AA);



		Point text_center = center;
		text_center.x -= 20;

		putText(field, player_number + ":" + team_name, text_center, FONT_HERSHEY_PLAIN, 1, Scalar::all(255),
				1, CV_AA);

		// Create point for direction
		Point direction;
		direction.x = center.x + 25 * cos(CV_PI * imageData[2] / 180);
		direction.y = center.y + 25 * sin(CV_PI * imageData[2] / 180);

		line(field, center, direction, Scalar(255, 0, 0, 255), 1, CV_AA);

	}
}

void display(Mat &image, int camera) {

	if (camera == LEFT_CAMERA)
		imshow("in_left", image);
	else if (camera == RIGHT_CAMERA)
		imshow("in_right", image);
}

int getTeamNumber(String id){

	if(!team1[id].empty())
		return 1;
	else if (!team2[id].empty())
		return 2;

	return -1;
}

int getTeamNumber(int idn){

	string id = boost::lexical_cast<string>(idn);

	if(!team1[id].empty())
		return 1;
	else if (!team2[id].empty())
		return 2;

	return -1;
}

Scalar getColourFromString(String colour){

	vector<String> colours = split(colour, ',');
	// Convert string to RGB colours and handle out of range values
	int R = max(min(atoi(colours[0].c_str()), 255), 0);
	int G =  max(min(atoi(colours[1].c_str()), 255), 0);
	int B = max(min(atoi(colours[2].c_str()), 255), 0);

	return Scalar(R,G,B);

}

String getIPPlayer(int id){

    if(getTeamNumber(id) == 1)
  	  return team1[NumberToString(id) + "_ip"];
    else
  	  return team2[NumberToString(id) + "_ip"];

}

