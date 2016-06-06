#include "FieldUtils.h"
#include "math.h"

int getIdFromPoint(int x, int y, vector<Marker> &markers, int player_radius, int field_height, int image_height, int camera){

	uint i;
	Point player;
	for (i = 0; i < markers.size(); i++) {

		player = getFieldPoint(markers[i].getCenter().x, markers[i].getCenter().y, field_height, image_height, camera);

		if(x > (player.x - player_radius) && x < (player.x + player_radius) && y > (player.y - player_radius) && y < (player.y + player_radius))
			return  markers[i].id;

	}

	return -1;
}

bool isTargetReached(Point p, int player_radius, Point t, int field_height, int image_height, int camera){

	Point player = getFieldPoint(p.x, p.y, field_height, image_height, camera);
	Point target = getFieldPoint(t.x, t.y, field_height, image_height, camera);

	if(target.x > (player.x - player_radius) && target.x < (player.x + player_radius) && target.y > (player.y - player_radius) && target.y < (player.y + player_radius))
		return true;

	return false;
}

Point getFieldPoint(float x, float y, int field_height, int image_height, int camera) {

	Point pt;

	pt.x = ((field_height / 2) * x / image_height);
	pt.y = ((field_height / 2) * y / image_height);

	if (camera == RIGHT_CAMERA)
		pt.y += field_height / 2;

	return pt;
}

vector<float> getDistanceAngleRealField(float x1, float y1, float x2, float y2, int image_height, int image_width, float real_height, float real_width) {

	// Calculating first point in real life
	float real_x1 = x1 * real_width / image_width;
	float real_y1 = x1 * real_height / image_height;

	// Calculating second point in real life
	float real_x2 = x2 * real_width / image_width;
	float real_y2 = y2 * real_height / image_height;

	vector<float> dist_angle;
	cout << real_x1 << ";"<< real_y1 <<";"<< real_x2 <<";"<< real_y2 << endl;
	dist_angle.push_back(calculateDistance(real_x1, real_y1, real_x2, real_y2));
	dist_angle.push_back(calculateRadians(real_x1, real_y1, real_x2, real_y2));
	return dist_angle;
}

// Calculates the distance between the two points
float calculateDistance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x2-x1,2) + pow(y2-y1,2));
}

// Calculate the angle of the line between the 2 points in radians
float calculateRadians(float x1, float y1, float x2, float y2) {

	float delta_x = x2 - x1;
	float delta_y = y2 - y1;

	return atan2(delta_y, delta_x);
}

float getRadiansFromDegree(float deg){
	return deg/ 180 * CV_PI;
}

