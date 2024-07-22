#include "channels.hpp"

channels::channels()
{
	name = "";
	key = "";
	mode = 0;
	op = 0;
}

channels::channels(std::string name, int mode, int op)
{
	this->name = name;
	this->mode = mode;
	this->op = op;
}