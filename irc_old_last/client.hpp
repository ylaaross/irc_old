#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "channels.hpp"
class client
{
	public:
		bool passB;
		bool nicknameB;
		bool usernameB;
		bool connected;
		bool duplicateNickname;
		int fd;
		std::string nickname;
		std::string username;
		std::string ipclient;
		std::vector<channels> channel;
	public:
		client();
		void addFd(int fd);
		void addNickname(std::string const nickname);
		void addUser(std::string const username);
};