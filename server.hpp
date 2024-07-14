#pragma once
#include <iostream>
#include "client.hpp"
#include <map>
#include <vector>
// enums
// {

// }
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
#define ERR_NEEDMOREPARAMS(nick, hostname, cmd) ":" + hostname + " 461 " + nick + " " + cmd + " :Not enough parameters\r\n"
#define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
#define RPL_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
class server
{
	public:
		std::map<int, client> clientServer;
	public:
		int channelMember();
		void brodcast(std::string msg, std::string channel, int fd);
		int channelname_used(std::string name);
		void searchAdd(int fd, std::string ip);
		int duplicatedNickname(std::string name);
		void commandApply(int fd,  std::vector<std::string>commandLine, std::string password);
		int searchForid(std::string name);
		~server();
};



