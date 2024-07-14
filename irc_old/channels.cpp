#include "channels.hpp"

channels::channels()
{
	name = "";
	key = "";
	mode = 0;
}

channels::channels(std::string name, int mode)
{
	this->name = name;
	this->mode = mode;
}