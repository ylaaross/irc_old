#include <iostream>

class channels
{
	public:
		std::string name;
		int mode;
		std::string key;
		channels();
		channels(std::string name, int mode);
};