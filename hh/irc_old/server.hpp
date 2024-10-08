#pragma once
#include <iostream>
#include "client.hpp"
#include <map>
#include <vector>

// : Set/remove Invite-only channel
// · t: Set/remove the restrictions of the TOPIC command to channel
// operators
// · k: Set/remove the channel key (password)
// · o: Give/take channel operator privilege
// l: Set/remove the user limit to channel


enum EnumName
{
    INVITE_ONLY,
	TOPIC,
    KEY,
    LIMIT,
	OPERATOR,
	POSITIF
};
#define RPL_DELOP(hostname, nick, channel, username, target) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " -o " + target + "\r\n"
#define ERR_NOTOP(hostname, channel) ":" + hostname + " 482 " + channel + " " + ":You're not a channel operator\r\n"
#define ERR_CHANNELISFULL(hostname, nick, channelName) ":" + hostname + nick + " 471 " + channelName + " :Cannot join channel (+l) - channel is full, try again later\r\n"
#define RPL_NICK_SET(hostname, new_nick) ":" + new_nick + "!" + new_nick + "@" + hostname + " NICK :" + new_nick + "\r\n"
#define ERR_UNKNOWNMODE(hostname, nick, c) ":" + hostname + " 472 " + nick + " " + c + " :is unknown mode char to me\r\n"
#define RPL_ADDMODE(hostname, nick, channel, mode, arg, username) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " " + mode + " " + arg + "\r\n"
#define RPL_DELMODE(hostname, nick, channel, mode, username) ":" + nick + "!~" + username + "@" + hostname + " MODE " + channel + " " + mode + " " + "\r\n"
#define RPL_INVITE(nick, username, clienthostname, invited, channel) ":" + nick + "!" + username + "@" + clienthostname + " INVITE " + invited + " :" + channel + "\r\n"
#define RPL_INVITING(hostname, nick, invited, chann) ":" + hostname + " 341 " + nick + " " + invited + " " + chann + "\r\n"
#define ERR_CHANOPRIVSNEEDED(hostname, nick, chann) ":" + hostname + " 482 " + nick + " " + chann + " :You're not channel operator\r\n"
#define ERR_USERNOTINCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 441 " + nick + " " + nick2 + " " + chann + " " + " :They aren't on that channel\r\n"
#define RPL_KICK(kicker, username, host, channel, targetuser, reason) ":" + kicker + "!" + username + "@" + host + " KICK " + channel + " " + targetuser + " :" + reason + "\r\n"
#define ERR_NOTONCHANNEL(hostname, chann) ":" + hostname + " 442 " + chann + " " + ":You're not on that channel\r\n"
#define RPL_TOPIC(hostname, nick, chann, topic) ":" + hostname + " 332 " + nick + " " + chann + " :" + topic + "\r\n"
#define ERR_NOSUCHNICK(hostname, nick) ":" + hostname + " 401 " + nick + " "  + " :No such nick\r\n"
#define ERR_CANNOTSENDTOCHAN(hostname, nick, channel) ":" + hostname + " 404 " + nick + " " + channel + " :Cannot send to channel\r\n"
#define ERR_NORECIPIENT(hostname, nick ,cmd)  ":" + hostname  + " 411 " + nick + " :No recipient given (" + cmd + ")\r\n"
#define ERR_NOTEXTTOSEND(hostname, nick)  ":" + hostname  + " 412 " + nick + " :No text to send\r\n"
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
#define ERR_NEEDMOREPARAMS(nick, hostname, cmd) ":" + hostname + " 461 " + nick + " " + cmd + " :Not enough parameters\r\n"
#define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
#define RPL_JOIN(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) ":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n"
#define ERR_CHANOPRIVSNEEDED(hostname, nick, chann) ":" + hostname + " 482 " + nick + " " + chann + " :You're not channel operator\r\n"
#define ERR_USERONCHANNEL(hostname, nick, nick2, chann) ":" + hostname + " 443 " + nick + " " + nick2 + " " + chann + " :is already on channel\r\n"
#define ERR_NOTOP(hostname, channel) ":" + hostname + " 482 " + channel + " " + ":You're not a channel operator\r\n"
#define ERR_INVITEONLY(nick, hostname, channelName) ":" + hostname + " 473 " + nick + " " + channelName + " :Cannot join channel, you must be invited (+i)\r\n"
#define ERR_BADCHANNELKEY(nick, hostname, channelName) ":" + hostname + " 475 " + nick + " " + channelName + " :Cannot join channel (+K) - bad key\r\n"
#define RPL_QUIT(hostname, nick, quitMessage) ":" + hostname + "  QUIT " + nick + " :" + quitMessage + "\r\n"

class server
{
	public:
		std::map<int, client> clientServer;
	public:
		bool		checkInvitedPersonnes(std::string name, int channelid, int fd);
		int			idChannelfd(std::string name, int *fd);
		char		memberChannelNumbers(std::string name);
		int			idChannel(std::string name, int fd);
		void		updateMode(std::string channel, int  mode, char sign, std::string arg);
		void		brodcastMode(std::string channel,std::string mode, int fd, std::vector<std::string> arg);
		void		applicateMode(char mode, std::string channel,int id,char used, std::vector<std::string> args);
		std::string reason(std::string str, int fd);
		void		inserUser(std::string nickname, std::string channel);
		void		kickUser(int fd, int index, std::string name,std::string reason); 
		int			find_channel_id(std::string name, int fd);
		bool		operator_user(std::string name,int fd);
		bool		on_channel(std::string name,int fd);
		bool		availableChannel(std::string name);
		char		modeChannel(std::string name);
		int			channelMember();
		void		brodcast(std::string msg, std::string channel, int fd);
		int			channelname_used(std::string name);
		void		searchAdd(int fd, std::string ip);
		int			duplicatedNickname(std::string name);
		void		commandApply(int fd,  std::vector<std::string>commandLine, std::string password);
		bool		channelMember(std::string channel, int fd);
		int			searchForid(std::string name);
		int			new_channel(std::string name);
					~server();
};



