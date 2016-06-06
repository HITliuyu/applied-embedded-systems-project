#include "Parser.h"

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
    	if(!item.empty())
    		elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

void readTeamsFromFile(string filePath, map<string, string> &team1, map<string, string> &team2, bool debug) throw(cv::Exception)
{

	ifstream logReader(filePath.c_str(), ofstream::in);

	string line;
		while (getline(logReader,line))
		{
			line.erase(remove(line.begin(), line.end(), ' '), line.end());

			// First Team
			if (line.find("team_1") != string::npos)
			{
				vector<string> team = split(line, '=');
				team1["name"] = team[1];

				if(debug)
					cout << "Team1: " << team1["name"] << "\n";
			}
			else // Second Team
				if (line.find("team_2") != string::npos)
				{
					vector<string> team = split(line, '=');
					team2["name"] = team[1];

					if(debug)
						cout << "Team2: " << team2["name"] << "\n";
				}
			else // First Team's colour
				if (line.find("colour_1") != string::npos)
				{
					vector<string> colour = split(line, '=');
					team1["colour"] = colour[1];

					if(debug)
						cout << "Team1 colour: " << team1["colour"] << "\n";
				}
			else // Second Team's colour
				if (line.find("colour_2") != string::npos)
				{
					vector<string> colour = split(line, '=');
					team2["colour"] = colour[1];

					if(debug)
						cout << "Team2 colour: " << team2["colour"] << "\n";
				}
			else // First Team's short name
				if (line.find("short_1") != string::npos)
				{
					vector<string> s_name = split(line, '=');
					team1["s_name"] = s_name[1];

					if(debug)
						cout << "Team1 short name: " << team1["s_name"] << "\n";
				}
			else // Second Team's short name
				if (line.find("short_2") != string::npos)
				{
					vector<string> s_name = split(line, '=');
					team2["s_name"] = s_name[1];

					if(debug)
						cout << "Team2 short name: " << team2["s_name"] << "\n";
				}
			else // First Team's players
				if (line.find("players_1") != string::npos)
				{
					vector<string> players = split(split(line, '=')[1],';');

					uint i;
					for(i = 0; i < players.size(); i++)
					{
						vector<string> p = split(players[i], ',');
						team1[p[0]] = p[1];

						if(p.size() == 3)
						{
							team1[p[0] + "_ip"] = p[2];

							if(debug)
								cout << "Player: " << p[0] << " has IP address: " << p[2] << endl;
						}
					}
				}
			else // Second Team's players
				if (line.find("players_2") != string::npos)
				{
					vector<string> players = split(split(line, '=')[1],';');

					uint i;
					for(i = 0; i < players.size(); i++)
					{
						vector<string> p = split(players[i], ',');
						team2[p[0]] = p[1];

						if(p.size() == 3)
						{
							team2[p[0] + "_ip"] = p[2];
							if(debug)
								cout << "Player: " << p[0] << " has IP address: " << team2[p[0] + "_ip"] << endl;
						}
					}
				}
			// End of the file
			else if (line.find("##") != string::npos)
					break;

		}


}

