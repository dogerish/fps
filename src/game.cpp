#include "game.h"
#include <string>
#include <fstream>
#include <iostream>

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
	std::cout << message << std::endl;
	logfile << message << std::endl;
}
