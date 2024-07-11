#include "channels.hpp"

channels::channels()
{
	name = "";
	mode = 0;
}

channels::channels(std::string name, int mode)
{
	this->name = name;
	this->mode = mode;
}