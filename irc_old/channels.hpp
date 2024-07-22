#include <iostream>
#include <vector>
class channels
{
	public:
		std::string					name;
		int							mode;
		int							op;
		std::string 				key;
		std::vector<std::string>	invited;
		channels();
		channels(std::string name, int mode, int op);
		
};