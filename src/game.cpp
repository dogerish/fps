#include "game.h"
#include <string>
#include <fstream>

GameData::GameData(const char* logfilename)
{
	logfile = std::ofstream(logfilename, std::ofstream::out);
}

GameData::~GameData()
{
	logfile.close();
}

void GameData::log(std::string message)
{
	logfile << message << std::endl;
}
