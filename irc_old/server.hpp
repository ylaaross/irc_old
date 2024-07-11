#pragma once
#include <iostream>
#include "client.hpp"
#include <map>
#include <vector>
// enums
// {

// }
class server
{
	public:
		std::map<int, client> clientServer;
	public:
		int channelMember();
		void brodcast(std::string msg, std::string channel);
		void searchAdd(int fd);
		int duplicatedNickname(std::string name);
		void commandApply(int fd,  std::vector<std::string>commandLine, std::string password);
		int searchForid(std::string name);
		~server();
};



