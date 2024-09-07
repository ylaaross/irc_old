#include <iostream>
#include <vector>
class channels
{
	public:
		std::string					name;
		char						mode;
		int							op;
		std::string 				key;
		int							limit;
		std::vector<std::string>	invited;
		std::string					topic;
		channels();
		channels(std::string name, char mode, int op, std::vector<std::string>	invited);
		
};