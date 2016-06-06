#include "NaoController.h"
#include <stdio.h>
#include <stdlib.h>

template<typename T>
string NumberToString(T Number) {
	return boost::lexical_cast<std::string>(Number);
}

bool moveRobot(string IP, float distance, float angle) {

	string dist;

	if(distance <= 0.55)
		dist = NumberToString(distance);
	else
		dist = "0.55";

	string cmd = "python ../image-processor/scripts/Controller.py --ip " + IP
			+ " --x " +  dist + " --y 0 --t "
			+ NumberToString(angle);

	cout << cmd << endl;
	cout << system(cmd.c_str()) << endl;
	return true;
}
