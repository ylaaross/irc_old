#include "channels.hpp"

channels::channels()
{
	name = "";
	key = "";
	mode = 0;
	op = 0;
}

channels::channels(std::string name, char mode, int op , std::vector<std::string>	invited)
{
	this->name = name;
	this->mode = mode;
	this->op = op;
	this->invited = invited;
}