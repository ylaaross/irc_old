#include <iostream>
#include <vector>
class channels
{
	public:
		std::string					name;
		char						mode;
		int							op;
		std::string 				key;
		std::vector<std::string>	invited;
		channels();
		channels(std::string name, char mode, int op);
		
};