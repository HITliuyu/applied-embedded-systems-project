#include "Config.h"
#include "Parser.h"

char key = 0;

template <typename T>
  string NumberToString ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }

bool readArguments(int argc, char **argv) {

	if (argc < 3) {
		cerr << "Invalid number of arguments" << endl;
		cerr << "Usage: log_file.txt teams_info.yml [time_between_frames_in_ms/step]" << endl;
		return false;
	}

	logFile_path = argv[1];

	readTeamsFromFile(argv[2], team1, team2, DEBUG);

	if(argc > 3){
		if(strcmp(argv[3],"step") == 0)
			waitTime = 0;
		else
			SLEEP_TIMER = atoi(argv[3]) * 1000;
	}

	return true;
}

int initGUI() {

	namedWindow("field", 1);
	cout << "Loading images...\n";
	field_original = imread("res/field.png");

	if (field_original.empty()) {
		cout
				<< "Error loading the images. Did you forget to include the folder?\n";
		return -1;
	}
	cout << "Images loaded.\n";

	cout << "Press P to speed up the replay\n";
	cout << "Press O to slow down the replay\n";
	return 0;
}

void processKey(char k) {
	switch (k) {
	case 's':
		if (waitTime == 0)
			waitTime = waitTime_old;
		else
			{
				waitTime_old = waitTime;
				waitTime = 0;
			}
		break;
	case 'p':
			if((SLEEP_TIMER - 10000) >= 0)
				SLEEP_TIMER -= 10000;
			else
				SLEEP_TIMER = 0;

			if(DEBUG)
				cout << "New sleep Time: " << SLEEP_TIMER << "\n";
			break;

	case 'o':
			SLEEP_TIMER += 10000;

			if(DEBUG)
				cout << "New sleep Time: " << SLEEP_TIMER << "\n";
			break;
	}
}


int main(int argc, char **argv) {

	if (readArguments(argc, argv) == false)
		return 0;
	logReader.open(logFile_path.c_str(), ofstream::in);

	//Create gui
	if (initGUI() == -1)
		return -1;

	string line;
	int team_number;
	Scalar team_colour;
	String team_name;
	String player_number;

	while (getline(logReader,line) && key != 27)
	{
		// Beginning of the frame
		if (line.find("Frame") != string::npos)
		{
			field_original.copyTo(field);

			vector<string> frame = split(line, ':');
			Point text_pos;
			text_pos.x = FIELD_WIDTH - 120;
			text_pos.y = 40;
			putText(field, frame[1].c_str(), text_pos, FONT_HERSHEY_PLAIN, 3, Scalar::all(0), 2, CV_AA);

			// Show team names
			text_pos.x = 10;
			text_pos.y = 20;

			putText(field, team1["name"] + " vs " + team2["name"], text_pos, FONT_HERSHEY_PLAIN, 1,
					Scalar::all(255), 1, CV_AA);
		}
		// End of the frame
		else if (line.find("#") != string::npos)
			{
				imshow("field", field);
				key = waitKey(waitTime);
				processKey(key);
				usleep(SLEEP_TIMER);

			}
		else if (line.find("ball") != string::npos)
		{
			vector<string> ball = split(line, ';');

			Point center;
			center.x = atof(ball[1].c_str());
			center.y = atof(ball[2].c_str());

			// Showing ball
			circle(field, center, 10, Scalar(0,0,255), -1, CV_AA);

		}
		else
		{
			// id;x;y;rot
			vector<string> player = split(line, ';');

			Point center;
			center.x = atof(player[1].c_str());
			center.y = atof(player[2].c_str());

			float rot = atof(player[3].c_str());

			// Showing player
			circle(field, center, 10, team_colour, 1, CV_AA);

			// Showing player
			team_number = getTeamNumber(player[0]);

			if(team_number == 1)
			{
				team_name = team1["s_name"];
				team_colour = getColourFromString(team1["colour"]);
				player_number = team1[player[0]];
			}
			else if(team_number == 2)
			{
				team_name = team2["s_name"];

				team_colour = getColourFromString(team2["colour"]);
				player_number = team2[player[0]];
			}
			else
			{
				team_name = "UNK";
				team_colour = Scalar(255,255,255);
				player_number = "00";
			}

			Point text_center = center;
			text_center.x += 2;
			putText(field, player_number + ":" + team_name, text_center, FONT_HERSHEY_PLAIN, 1, Scalar::all(255),
							1, CV_AA);
			// Showing direction
			// Create point for direction
			Point direction;
			direction.x = center.x + 25 * cos(CV_PI * rot / 180);
			direction.y = center.y + 25 * sin(CV_PI * rot / 180);

			cv::line(field, center, direction, Scalar(255, 0, 0, 255), 1, CV_AA);


		}

	}
}

int getTeamNumber(String id){

	if(!team1[id].empty())
		return 1;
	else if (!team2[id].empty())
		return 2;

	return -1;
}

Scalar getColourFromString(String colour){

	vector<String> colours = split(colour, ',');
	int R = atoi(colours[0].c_str());
	int G = atoi(colours[1].c_str());
	int B = atoi(colours[2].c_str());

	return Scalar(R,G,B);

}


