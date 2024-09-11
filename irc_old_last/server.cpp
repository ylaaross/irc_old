#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>

#include <fcntl.h> 
#include <sstream>
#include <cctype> 
#include "server.hpp"

std::vector<std::string> splitBySpaces(const std::string &str) 
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string word;
    while (iss >> word) 
        result.push_back(word);
    return result;
}
int server::searchForid(std::string name)
{
    std::map<int, client>::iterator it = clientServer.begin();
    while(it != clientServer.end())
    {
        if(it->second.nickname == name)
            return(it->first);
        it++;
    }
    return (-1);
}

std::string converUpper(std::string str)
{
    int i;

    i = 0;
    while(str[i])
    {
        str[i] = toupper(str[i]);
        i++;
    }
    return str;
}

int checkCommand(std::string f1)
{
    std::string command;
    std::string value;

    command = converUpper(f1);
    if (command == "PASS")
        return (1);
    else if(command == "NICK")
        return (2);
    else if(command == "USER")
        return (3);
    else if(command == "PRIVMSG")
        return (4);
    else if(command == "JOIN")
        return (5);
    else if(command == "TOPIC")
        return (6);
    else if(command == "KICK")
        return (7);
    else if(command == "INVITE")
        return (8);
    else if(command == "MODE")
        return (9);
    else if(command == "PART")
        return (10);
    return (0);
}

int convert_test(char *number)
{
    int num;
    int size;
    char *rest = 0;
  
    if (strlen(number) > 5)
        return(-1);
    num = std::strtod(number ,&rest);
    if (strlen(rest) >= 1)
        return (-1);
    if (num >= 0 && num <= 65535)
        return num;
    return (-1);
}

int convert(char *number)
{
    int num;
    char *rest = 0;
  
    num = std::strtod(number ,&rest);
    return (num);
}



int server::duplicatedNickname(std::string name)
{
    std::map<int, client>::iterator it  = clientServer.begin();
    while(it != clientServer.end())
    {
        if(it->second.nickname == name)
            return(1);
        it++;
    }
    return (0);
}

int checkChannelName(std::string name)
{
    if(name[0] == '#' || name[0] == '&')
        return (1);
    return(0);
}

char		server::modeChannel(std::string name)
{
    std::map<int, client>::iterator it  = clientServer.begin();
    int i;

    while (it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if(it->second.channel[i].name == name)
                return (it->second.channel[i].mode);
            i++;
        }
        it++;
    }
    return (0);
}

char		server::memberChannelNumbers(std::string name)
{
    std::map<int, client>::iterator it  = clientServer.begin();
    int i;
    int number;

    number = 0;

    while (it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if(it->second.channel[i].name == name)
            {
                std::cout << "id " << it->first << number << std::endl;
               number++;
            } 
            i++;
        }
        it++;
    }
    // std::cout << "number " << number << std::endl;
    return (number);
}

bool server::availableChannel(std::string name)
{
    std::map<int, client>::iterator it  = clientServer.begin();
    int i;
    while (it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if(it->second.channel[i].name == name)
                return (1);
            i++;
        }
        it++;
    }
    return (0);
}

int  server::idChannel(std::string name, int fd)
{

    std::map<int, client>::iterator it  = clientServer.begin();
    int i;

    i = 0;
    while (i < clientServer[fd].channel.size())
    {
        if(clientServer[fd].channel[i].name == name)
            return i;
        i++;
    }

   if(availableChannel(name))
        return (-1);
    else
        return (-2);
}

int  server::idChannelfd(std::string name, int *fd)
{

    std::map<int, client>::iterator it  = clientServer.begin();
    int i;

   
    while(it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if(it->second.channel[i].name == name)
            {
                *fd = it->first; 
                return i;
            }
            i++;
        }
        it++;
    }
    return (-1);
}

void message(std::string msg, int fd)
{
    std::string response = msg;
    int sendResult = send(fd, response.c_str(), response.length(), 0);
      if (sendResult < 0) {
        std::cerr << "Error sending response to client: " << strerror(errno) << std::endl;
    } else if (sendResult == 0) {
        std::cerr << "Client disconnected unexpectedly" << std::endl;
    }
}

void server::brodcast(std::string msg, std::string channel, int fd)
{
    int i;
    bool b;
    
    b = 0;
    std::map<int, client>::iterator it = clientServer.begin();
    while(it != clientServer.end())
    {
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channel && fd != it->first)
            {
                b = 1;
                message(PRIVMSG_FORMAT(clientServer[fd].nickname,it->second.username,it->second.ipclient,it->second.channel[i].name, msg),it->first);
            }
            i++;
        }
        it++;
    }
}

void		server::updateclients(std::string channel, int fd)
{
    int i;
    
    std::map<int, client>::iterator it = clientServer.begin();
    while (it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if (it->second.channel[i].name == channel)
               message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channel)  ,  channel, clientServer[fd].nickname) , it->first);
            i++;
        }
        it++;
    }
}

// void		server::updateTopic(std::string channel, int fd)
// {
//     int i;
    
//     std::map<int, client>::iterator it = clientServer.begin();
//     while (it != clientServer.end())
//     {
//         i = 0;
//         while (i < it->second.channel.size())
//         {
//             if (it->second.channel[i].name == channel)
//                message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channel)  ,  channel, clientServer[fd].nickname) , it->first);
//             i++;
//         }
//         it++;
//     }
// }

void		server::kickclients(std::string channel , int fd)
{
    int i;
    bool b;
    
  
    std::map<int, client>::iterator it = clientServer.begin();
    while(it != clientServer.end())
    {
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channel)
                message(RPL_KICK(clientServer[fd].nickname, clientServer[fd].username, clientServer[fd].ipclient, channel, clientServer[fd].nickname, ""),it->first);
            i++;
        }
        it++;
    }
}

void server::brodcastMode (std::string channel, std::string mode, int fd, std::vector<std::string> args)
{
    int i;
    bool b;

    std::string msg = "";
    std::string arg = "";
    b = 0;
    i = 0;

    if (mode[1] == 'k')
        arg = args[0];
    else if (mode[1] == 'l')
        arg = args[1];
    else
        arg = args[2];

    std::map<int, client>::iterator it = clientServer.begin();
    if (mode[0] == '+')
        msg = RPL_ADDMODE(clientServer[fd].ipclient, clientServer[fd].nickname, channel, mode ,arg, clientServer[fd].username);
    else if (mode[0] == '-' && mode[1] != 'o')
        msg = RPL_DELMODE(clientServer[fd].ipclient, clientServer[fd].nickname, channel, mode , clientServer[fd].username);
    else if (mode[0] == '-' && mode[1] == 'o')
        msg = RPL_DELOP(clientServer[fd].ipclient, clientServer[fd].nickname, channel, mode , args[2]);
    while (it != clientServer.end())
    {
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channel)
            {
                b = 1;
                message(msg, it->first);
            }
            i++;
        }
        it++;
    }
}


server::~server()
{

}

std::string erased(std::string str)
{

    int i = 0;

    while(i < str.size())
    {
        if(str[i] == '\n')
            str.erase(i, 1);
        i++;
    }
    return str;
}

int server::channelname_used(std::string name)
{
    int i;
    std::map<int, client>::iterator it = clientServer.begin();
    while(it != clientServer.end())
    {   
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == name)
                return (1);
            i++;
        }
        it++;
    }
    return (0);
}

std::vector<std::string> split(std::string str, char delimiter)
{
    std::string s;

    std::vector<std::string>  splitedString;
    std::stringstream bs(str);
    while (std::getline(bs, s, delimiter))
        splitedString.push_back(s);
    return splitedString;
}

std::vector<std::string> splitByCR(const std::string& data) {
    std::vector<std::string> lines;
    std::istringstream ss(data);
    std::string line;

    // Read each line using getline with '\n' delimiter
    while (std::getline(ss, line, '\n')) {
        // Split each line based on '\r' character
        std::istringstream lineStream(line);
        std::string segment;
        while (std::getline(lineStream, segment, '\r')) {
            if (!segment.empty()) {
                lines.push_back(segment);
            }
        }
    }
    return lines;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r"); // Find the first non-whitespace character
    size_t last = str.find_last_not_of(" \t\n\r");   // Find the last non-whitespace character
    
    if (first == std::string::npos) // No non-whitespace characters found
        return ""; 
    
    return str.substr(first, last - first + 1);
}

bool server::on_channel(std::string name, int fd)
{
    int i;

    i = 0;
    while (i < clientServer[fd].channel.size())
    {
        if (clientServer[fd].channel[i].name == name)
            return (1);
        i++;
    }
    return (0);
}

void server::searchAdd(int fd, std::string ip)
{
    std::map<int, client>::iterator it = clientServer.find(fd);
    if (it == clientServer.end())
    {
        clientServer[fd] = client();
        clientServer[fd].ipclient = ip;
        // std::cout << fd;
    }
}

int server::find_channel_id(std::string name, int fd)
{
    int i;
    
    i = 0;

    while (i < clientServer[fd].channel.size())
    {
        if (clientServer[fd].channel[i].name == name)
            return (i);
        i++;
    }
    return (-1);
}

bool server::operator_user(std::string name, int fd)
{
    int pos;
    
    pos = find_channel_id(name, fd);
   
    if(pos != -1)
    {
        if(clientServer[fd].channel[pos].op)
            return (1);
        return(0);
    }
    return(0);
}

bool server::topicMode(std::string name, int fd)
{
    int pos;
    
    pos = find_channel_id(name, fd);
   
    if(pos != -1)
    {
        if(!(clientServer[fd].channel[pos].mode & (1 << TOPIC)))
            return (1);
        return(0);
    }
    return(0);
}

bool server::channelMember(std::string channel, int fd)
{
    int i;
    
    i = 0;

    while (i < clientServer[fd].channel.size())
    {
        if (clientServer[fd].channel[i].name == channel)
            return (1);
        i++;
    }
    return (0);
}
std::string server::reason(std::string str, int fd)
{
    std::vector<std::string> splitedString;
    std::string line = clientServer[fd].nickname;
    splitedString = split(str,':');
    if(splitedString.size() > 1)
        line = splitedString[1];
    return line;
}
void server::kickUser(int fd, int index, std::string name,std::string reason)
{
    bool b;
    int i;

    i = 0;
    b = 0;
    std::map<int, client>::iterator it = clientServer.begin();
    while (i < clientServer[index].channel.size())
    {
        if (clientServer[index].channel[i].name == name)
        {
            b = 0;
            int j;
            while(it != clientServer.end())
            {
                j = 0;
                while(j < it->second.channel.size())
                {
                    if(it->second.channel[j].name == name)
                        message(RPL_KICK(clientServer[fd].nickname, clientServer[fd].username,clientServer[fd].ipclient,name,clientServer[index].nickname,reason),it->first);
                    j++;
                }
                it++;
            }
            clientServer[index].channel.erase(clientServer[index].channel.begin() + i);
            break;
        }
        i++;
    }
    if(b == 0)
        message(ERR_NOSUCHNICK(clientServer[fd].ipclient, clientServer[fd].nickname), fd);
}

 bool limitNumber(std::string str)
{
    int i;
    bool nbr = 0;
    i = 0;
                    
    while(str[i])
    {
        if(!(str[i] >= '0'  && str[i] <= '9'))
            return 0;
        else
             std::cout<< "number" <<std::endl;
            return 1;
        i++;
    }
    return 1;
}

void    server::updateMode (std::string channel,  int wich, char sign, std::string arg)
{
    int i;
    int sh;
    std::map<int, client>::iterator it = clientServer.begin();

    
    while (it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if (it->second.channel[i].name == channel)
            {
                if(wich == KEY)
                   it->second.channel[i].key = arg; 
                else if(wich == LIMIT)
                {
                    const char* c_str = arg.c_str();
                    it->second.channel[i].limit = convert((char*)  c_str);
                }
                if(sign == '+')
                    it->second.channel[i].mode |= (1 << wich);
                else
                    it->second.channel[i].mode &= ~(1 << wich);
            }
            i++;
        }
        it++;
    }
}

void    server::applicateMode(char mode, std::string channel,int fd, char used ,std::vector<std::string> args)
{
    std::map<int, client>::iterator it = clientServer.begin();

    int i;
    while(it != clientServer.end())
    {
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channel)
            {
                if((mode & (1 << INVITE_ONLY)) && (it->second.channel[i].mode & (1 << INVITE_ONLY)) &&  used & (1 << INVITE_ONLY))
                    std::cout<< "already INVITE " <<std::endl;
                else if((mode & (1 << INVITE_ONLY)) && !(it->second.channel[i].mode & (1 << INVITE_ONLY))&&  used & (1 << INVITE_ONLY))
                {
                    updateMode(channel, INVITE_ONLY, '+', "");
                    brodcastMode(channel,"+i", fd, args);
                    std::cout<< "INVITE " <<std::endl;
                }
                else if(!(mode & (1 << INVITE_ONLY)) && (it->second.channel[i].mode & (1 << INVITE_ONLY))&&  used & (1 << INVITE_ONLY))
                {
                    updateMode(channel, INVITE_ONLY, '-' , "");
                    brodcastMode(channel, "-i", fd, args);
                    std::cout<< "DELETE INVITE " << std::endl;
                }
                else if(!(mode & (1 << INVITE_ONLY)) && !(it->second.channel[i].mode & (1 << INVITE_ONLY))&&  (used & (1 << INVITE_ONLY) ))
                {
                    std::cout<< "cannot DELETE invite " <<std::endl;
                }

                if((mode & (1 << TOPIC)) && (it->second.channel[i].mode & (1 << TOPIC)) &&  (used & (1 << TOPIC) ))
                {
                   
                    std::cout<< "already topic " <<std::endl;
                }
                else if((mode & (1 << TOPIC)) && !(it->second.channel[i].mode & (1 << TOPIC))&&  used & (1 << TOPIC))
                {
                    updateMode(channel, TOPIC, '+' , "");
                    brodcastMode(channel, "+t", fd, args);
                    std::cout<< "INVITE topic" <<std::endl;
                }
                else if(!(mode & (1 << TOPIC)) && (it->second.channel[i].mode & (1 << TOPIC))&&  used & (1 << TOPIC))
                {
                    updateMode(channel, TOPIC, '-' , "");
                    brodcastMode(channel, "-t", fd, args);
                    std::cout<< "DELETE INVITE topic " << std::endl;
                }
                else if(!(mode & (1 << TOPIC)) && !(it->second.channel[i].mode & (1 << TOPIC))&&  used & (1 << TOPIC))
                    std::cout<< "cannot DELETE topic " <<std::endl;
                if(!args[0].empty())
                {
                    if((mode & (1 << KEY)) && (it->second.channel[i].mode & (1 << KEY)))
                        std::cout<< "already key" <<std::endl;
                    else if((mode & (1 << KEY)) && !(it->second.channel[i].mode & (1 << KEY)))
                    {
                        updateMode(channel, KEY, '+', args[0]);
                        brodcastMode(channel, "+k", fd, args);
                        std::cout<< "INVITE key" <<std::endl;
                    }
                    else if(!(mode & (1 << KEY)) && (it->second.channel[i].mode & (1 << KEY)))
                    {
                        if(it->second.channel[i].key == args[0])
                        {
                            updateMode(channel,KEY,'-', "");
                            brodcastMode(channel,"-k", fd, args);
                            std::cout<< "DELETE key " << std::endl;
                        }
                        else
                        {
                            std::cout<< "invalid key " << std::endl;
                        }
                    }
                    else if(!(mode & (1 << KEY)) && !(it->second.channel[i].mode & (1 << KEY)))
                        std::cout<< "cannot DELETE key " <<std::endl;   
                }
                //limit
            //    std::cout << "hna" <<args[1] << std::endl ;
                if (!args[1].empty() && limitNumber(args[1]))
                {
                    if((mode & (1 << LIMIT)) && (it->second.channel[i].mode & (1 << LIMIT))  )
                        std::cout<< "already limit" <<std::endl;
                    else if((mode & (1 << LIMIT)) && !(it->second.channel[i].mode & (1 << LIMIT)) )
                    {
                        const char* c_str = args[1].c_str();
                        int num = convert((char *) c_str);
                        std::stringstream ss;
                        ss << num;
                        std::string str = ss.str();
                        args[1] = str;
                        updateMode(channel, LIMIT, '+', args[1]);
                        brodcastMode(channel,"+l", fd, args);
                        std::cout<< "INVITE limit" <<std::endl;
                    }
                    else if(!(mode & (1 << LIMIT)) && (it->second.channel[i].mode & (1 << LIMIT)))
                    {
                        const char* c_str = args[1].c_str();

                        if(convert((char *) c_str) == it->second.channel[i].limit)
                        {
                            
                            updateMode(channel, LIMIT, '-', args[1]);
                            std::stringstream ss;
                            ss << c_str;
                            std::string str = ss.str();
                            args[1] = str;
                            brodcastMode(channel, "-l", fd, args);
                            std::cout<< "DELETE limit " << std::endl;
                        }
                    
                    }
                    else if(!(mode & (1 << LIMIT)) && !(it->second.channel[i].mode & (1 << LIMIT)))
                        std::cout<< "cannot DELETE limit " <<std::endl;   
                }
                if (!args[2].empty())
                {
                    int f = searchForid(args[2]);
                    int b = 0;
                    // int channel = clientServer[f].channel;
                    int k = 0;
                    while(k < clientServer[f].channel.size())
                    {
                        if(clientServer[f].channel[k].name == it->second.channel[i].name)
                        {
                            b = 1;
                            break;
                        }
                        k++;
                    }
                    
                   if(b)
                   {
                    if ((mode & (1 << OPERATOR)) && (clientServer[f].channel[k].op == 1))
                    {
                         std::cout << "nickname" << it->second.nickname <<std::endl;
                         std::cout << "op" << it->second.channel[i].op <<std::endl;
                        std::cout<< "already operator" <<std::endl;
                    }
                    else if ((mode & (1 << OPERATOR)) && (clientServer[f].channel[k].op == 0))
                    {
                        // std::cout <<"this is the user"  << it->second.nickname << std::endl;
                        clientServer[f].channel[k].op = 1;
                        brodcastMode(channel,"+o", fd, args);
                        updateclients(channel, fd);
                        std::cout<< "set operator" <<std::endl;
                    }
                    else if (!(mode & (1 << OPERATOR)) && (clientServer[f].channel[k].op == 1))
                    {
                        clientServer[f].channel[k].op= 0;
                        brodcastMode(channel,"-o", fd, args);
                        updateclients(channel, fd);
                        std::cout<< "DELETE operator " << std::endl;
                    }
                    else if (!(mode & (1 << OPERATOR)) && (clientServer[f].channel[k].op == 0))
                    {
                        std::cout<< "cannot DELETE operator" <<std::endl;
                    }
                   }
                    
                }
                return ;

            }
            i++;
        }
        it++;
    }
}

bool	server::alreadyUsedNickname(std::string nickname)
{
    std::map<int, client>::iterator it  = clientServer.begin();
    
    int i;
    while(it != clientServer.end())
    {
        if(it->second.nickname == nickname)
            return (1);
        it++;
    }
    return (0);
}

std::string     adminOrNot(bool state)
{
    if(state)
        return "@";
    return "";
}

std::string server::clientChannels(std::string channel)
{
    std::string contenated = "";
    std::map<int, client>::iterator it = clientServer.begin();
    int i;
    bool b = 0;
    while (it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if (it->second.channel[i].name == channel)
            {
                if (!b)
                    contenated += adminOrNot(it->second.channel[i].op) +  it->second.nickname;
                else
                    contenated += " " +adminOrNot(it->second.channel[i].op) +  it->second.nickname;
                b = 1;
                break;
            }
            i++;
        }
        it++;
    }
    return contenated;
}

bool server::checkInvitedPersonnes(std::string name, int channelid, int fd)
{
    int i;

    i = 0;
    
    while(i < clientServer[fd].channel[channelid].invited.size())
    {
        if(clientServer[fd].channel[channelid].invited[i] == name)
            return (1);
        i++;
    }
    return (0);
}

void server::inserUser(std::string nickname, std::string channel)
{
    std::map<int, client>::iterator it = clientServer.begin();
    
    int i;
    while(it != clientServer.end())
    {
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channel)
                it->second.channel[i].invited.push_back(nickname);
            i++;
        }
        it++;
    }
}

bool    endOfCommand(std::string str)
{
    int     i;

    i = 0;
    while (i < str.size())
    {
        if (str[i] == '\n' || str[i] == '\r')
            return (1);
        i++;
    }
    return (0);
}

void		server::updateChannelTopic(std::string topic, std::string channelname)
{
     std::map<int, client>::iterator it = clientServer.begin();
    
    int i;
    while(it != clientServer.end())
    {
        i = 0;
      
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channelname)
            {
                it->second.channel[i].topic = topic;
                break;
            }
            i++;
        }
        it++;
    }
}


void		server::displayTopic(std::string topic, std::string channelname)
{
     std::map<int, client>::iterator it = clientServer.begin();
    
    int i;
    //  std::cout << "topic name " << topic << std::endl;
    // std::cout << "channel " << channelname << std::endl;
    // std::cout << "topic" << topic;
    while(it != clientServer.end())
    {
        i = 0;
        while (i < it->second.channel.size())
        {
            if (it->second.channel[i].name == channelname)
            {
                
                message(RPL_TOPIC(it->second.ipclient, it->second.nickname, channelname, topic),it->first);
            }
            i++;
        }
        it++;
    }
}

std::string		server::topicName(std::string channelname)
{
     std::map<int, client>::iterator it = clientServer.begin();
    
    int i;
    while(it != clientServer.end())
    {
        i = 0;
        while(i < it->second.channel.size())
        {
            if(it->second.channel[i].name == channelname)
                return it->second.channel[i].topic;
            i++;
        }
        it++;
    }
    return "";
}

int     count_words(std::string str, bool *tr)
{
    int i;

    i = 0;
    bool newword = 1;
    int count = 0;
    while (i < str.size() && str[i] != ':')
    {
        if(str[i] != ' ' && newword == 1)
        {
            count++;
            newword = 0;
        }
        else if(str[i] == ' ' && newword == 0)
            newword = 1;
        i++;
    }
    while (i < str.size())
    {
        if (str[i] != ':')
        {
            *tr = 1; 
            count++;
            break;
        }
        i++;
    }
    return (count);
}

void server::commandApply(int fd,  std::vector<std::string>commandLine, std::string password)
{
    int i = 0;
    while (i < commandLine.size())
    {
        std::vector<std::string>firstSplit = splitBySpaces(commandLine[i]);
        if(clientServer[fd].duplicateNickname && !(checkCommand(firstSplit[0])>= 1 && checkCommand(firstSplit[0]) <= 3))
            message("Your nickname is duplicated you should create another account\n\r", fd);
        
        else if (checkCommand(firstSplit[0]) == 1)
        {
            if(firstSplit.size() != 2)
                message("more or less argument \n\r", fd);
            else
            {
                if(firstSplit[1] == password)
                {
                    message("Correct password\n\r", fd);
                    clientServer[fd].passB = 1;
                    // std::cout << "passb" << clientServer[fd].passB << std::endl;
                    // std::cout << "nicknameb" << clientServer[fd].nicknameB<< std::endl;
                    // std::cout << "usernameB" << clientServer[fd].usernameB<< std::endl;
                    if(clientServer[fd].passB && clientServer[fd].nicknameB && clientServer[fd].usernameB)
                    {
                        std::string msg = ":ircserv_KAI.chat 001 " + clientServer[fd].nickname + " :Welcome to the IRC Network, " + clientServer[fd].nickname + " \r\n";        
                        message(msg, fd);
                    }
                }
                else
                    message("Wrong password\n", fd);
            }
        }
    
        else if(checkCommand(firstSplit[0]) == 2)
        {
            if(firstSplit.size() != 2)
            {
                std::string msg = "Error :more or less parameter to use NICK \n\r";
                message(msg, fd);
            }
            else
            {
                if(!alreadyUsedNickname(firstSplit[1]))
                {
                    clientServer[fd].addNickname(firstSplit[1]);
                    message(RPL_NICK_SET(clientServer[fd].ipclient, firstSplit[1]), fd);
                    clientServer[fd].nicknameB = 1;
                    clientServer[fd].duplicateNickname = 0;
                    if(clientServer[fd].passB && clientServer[fd].nicknameB && clientServer[fd].usernameB)
                    {
                        std::string msg = ":ircserv_KAI.chat 001 " + clientServer[fd].nickname + " :Welcome to the IRC Network, " + clientServer[fd].nickname + " \r\n";        
                        message(msg, fd);
                    }
                }
                else
                {
                    clientServer[fd].duplicateNickname = 1;
                    std::string msg = "Error :Duplicated nickname \n\r";
                    message(msg, fd);
                }
            }
        }
        else if(checkCommand(firstSplit[0]) == 3)
        {
            bool tr=0;
            std::vector<std::string>username = split(commandLine[i], ':');
            if(count_words(commandLine[i], &tr) == 5 && tr)
            {
                // std::cout << username[1] << std::endl;
                clientServer[fd].addUser(username[1]);
                clientServer[fd].usernameB = 1;
                if(clientServer[fd].passB && clientServer[fd].nicknameB && clientServer[fd].usernameB)
                {
                    std::string msg = ":ircserv_KAI.chat 001 " + clientServer[fd].nickname + " :Welcome to the IRC Network, " + clientServer[fd].nickname + " \r\n";        
                    message(msg, fd);
                }
            }
            else
                message("You need more or less parameters \r\n", fd);
        }
        else if(checkCommand(firstSplit[0]) == 4 && clientServer[fd].connected)
        {
            if (clientServer[fd].connected)
            {
                std::cout << clientServer[fd].nickname << std::endl;
                std::vector<std::string>messagesSplit = split(commandLine[i], ':');
                if (checkChannelName(firstSplit[1]))
                {
                    if(availableChannel(firstSplit[1]))
                    {
                        if(channelMember(firstSplit[1], fd))
                            brodcast(messagesSplit[1], firstSplit[1], fd);
                        else
                            message(ERR_CANNOTSENDTOCHAN(clientServer[fd].ipclient, clientServer[fd].nickname, commandLine[i]),fd);
                    }
                    else
                        message(ERR_NOSUCHCHANNEL(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[1]),fd);
                }
                else
                {
                    if(firstSplit.size() == 1)
                        message(ERR_NORECIPIENT(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[0]),fd);
                    else if(firstSplit.size() == 2)
                        message(ERR_NOTEXTTOSEND(clientServer[fd].ipclient, clientServer[fd].nickname), fd);
                    else
                    {
                        int fd2;
                        fd2 = searchForid(firstSplit[1]);
                        if(fd2 != -1)
                            message(PRIVMSG_FORMAT(clientServer[fd].nickname , clientServer[fd].username, clientServer[fd].ipclient,firstSplit[1] , messagesSplit[1]), fd2);
                        else
                            message(ERR_NOSUCHNICK(clientServer[fd].ipclient,clientServer[fd].nickname),fd);
                    }
                }
            }
        }
        else if (checkCommand(firstSplit[0]) == 5 && clientServer[fd].connected)
        {
            bool b = 0;
            int i = 0;
            char mode = 0;
            if(firstSplit.size() < 2)
                message( "less argument\n\r" , fd);
            else
            {
                std::vector<std::string>channelSplited = split(firstSplit[1], ',');

                while(i < channelSplited.size())
                {
                    if (checkChannelName(channelSplited[i]))
                    {
                        if(availableChannel(channelSplited[i]))
                        {
                            mode = modeChannel(channelSplited[i]);
                            if (mode & (1 << LIMIT))
                            {
                                
                                int fd1;
                                int id;
                                id = idChannelfd(channelSplited[i], &fd1);
                                if (memberChannelNumbers(channelSplited[i]) < clientServer[fd1].channel[id].limit)
                                {
                                // std::cout << "1" << std::endl;

                                    // std::cout << "bnumber" << memberChannelNumbers(channelSplited[i]) << std::endl;
                                    // std::cout << "limit" <<clientServer[fd1].channel[id].limit << std::endl;
                                    if (mode & (1 << INVITE_ONLY) && checkInvitedPersonnes(clientServer[fd].nickname, id, fd1 ))
                                    {
                                        if(mode & (1 << KEY) && firstSplit.size() >2)
                                        {
                                            std::vector<std::string>keySplited = split(firstSplit[1], ',');
                                            if(i < firstSplit.size() && keySplited[i] == clientServer[fd1].channel[id].key)
                                            {
                                                clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0, clientServer[fd1].channel[id].invited));
                                                message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                                                updateclients(channelSplited[i], fd);
                                                displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                                                message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channelSplited[i])  , channelSplited[i], clientServer[fd].nickname) , fd);
                                            }
                                            // else
                                            //     std::cout << "hnaNOO" << std::endl;
                                        }
                                        else if(mode & (1 << KEY) && firstSplit.size() <= 2)
                                        {
                                            std::cout << "--NOO" << std::endl;
                                        }
                                        else
                                        {
                                            clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0, clientServer[fd1].channel[id].invited));
                                            message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                                            updateclients(channelSplited[i], fd);
                                            displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                                        }
                                    }
                                    else
                                    {
                                        clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0, clientServer[fd1].channel[id].invited));
                                        message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                                        updateclients(channelSplited[i], fd);
                                        displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                                    }
                                }
                                else
                                    message(ERR_CHANNELISFULL(clientServer[fd].ipclient, clientServer[fd].nickname, channelSplited[i]), fd);
                            }
                            else
                            {  
                                int fd1;
                                int id;
                                id = idChannelfd(channelSplited[i], &fd1);
                                // std::cout << "2" << std::endl;
                                if (mode & (1 << INVITE_ONLY) && checkInvitedPersonnes(clientServer[fd].nickname, id, fd1))
                                {
                                    // std::cout << "INVITE ONLY" << std::endl;
                                    clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0, clientServer[fd1].channel[id].invited));
                                    message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                                    
                                    // message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channelSplited[i])  , channelSplited[i], clientServer[fd].nickname) , fd);
                                    updateclients(channelSplited[i], fd);
                                    displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                                }
                                else if(!(mode & (1 << INVITE_ONLY)))
                                {
                                    if(mode & (1 << KEY) && firstSplit.size() > 2)
                                    {
                                        // std::cout << "key ONLY" << std::endl;
                                        std::vector<std::string>keySplited = split(firstSplit[2], ',');
                                        if (i <= keySplited.size() && keySplited[i] == clientServer[fd1].channel[id].key)
                                        {
                                            clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0, clientServer[fd1].channel[id].invited));
                                            message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                                            
                                            // message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channelSplited[i])  , channelSplited[i], clientServer[fd].nickname) , fd);
                                            updateclients(channelSplited[i], fd);
                                             displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                                        }
                                        else
                                            message("Wrong password\n\r" , fd);
                                    }
                                    else if(mode & (1 << KEY) && firstSplit.size() <= 2)
                                    {
                                        message(ERR_BADCHANNELKEY(clientServer[fd].nickname,clientServer[fd].ipclient,channelSplited[i]),fd);
                                    }
                                    else
                                    {
                                        // std::cout << "no ONLY" << std::endl;
                                        clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0, clientServer[fd1].channel[id].invited));
                                        message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                                       
                                            // message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channelSplited[i])  , channelSplited[i], clientServer[fd].nickname) , fd);
                                        updateclients(channelSplited[i], fd);
                                        displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                                    }
 
                                }
                                else 
                                    message(ERR_INVITEONLY(clientServer[fd].nickname, clientServer[fd].ipclient, channelSplited[i]), fd);
                                

                            }

                        }
                        else
                        {
                            // std::cout << "3" << std::endl;
                            std::vector<std::string> invited;
                            clientServer[fd].channel.push_back(channels(channelSplited[i], 1 << TOPIC, 1, invited));
                            message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                           
                            message(RPL_NAMREPLY(clientServer[fd].ipclient, clientChannels(channelSplited[i])  , channelSplited[i], clientServer[fd].nickname) , fd);
                            displayTopic(topicName(channelSplited[i]), channelSplited[i]);
                        }
                    }
                    else
                        message(ERR_NOSUCHCHANNEL(clientServer[fd].ipclient, clientServer[fd].nickname, channelSplited[i]), fd);
                        
                        
                        
                        std::map<int, client>::iterator it = clientServer.begin();
                        while(it != clientServer.end())
                        {
                            int i = 0;
                            while(i < it->second.channel.size())
                            {
                                int j = 0;
                                while(j < it->second.channel[i].invited.size())
                                {
                                    // std::cout << it->second.channel[i].invited[j] << std::endl;
                                    j++;
                                }
                                
                                i++;
                            }
                        
                            it++;
                        }
                    i++;
                }
            }   
        }
        else if(checkCommand(firstSplit[0]) == 6 && clientServer[fd].connected)
        {
            int error_ch = 0;
            int error_rights = 0;
            int topic = 0;
            if (clientServer[fd].connected)
            {
                error_ch = on_channel(firstSplit[1],fd);
                error_rights = operator_user(firstSplit[1], fd);
                topic = topicMode(firstSplit[1], fd);
                if(error_ch && (error_rights || topic))
                {
                    std::vector<std::string>messagesSplit = split(commandLine[i], ':');
                    if(messagesSplit.size() == 2)
                    {
                        updateChannelTopic(messagesSplit[1], firstSplit[1]);
                        displayTopic(messagesSplit[1], firstSplit[1]);
                    }
                    else if(messagesSplit.size() == 1)
                    {
                        message(RPL_TOPIC(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[1],topicName(firstSplit[1])),fd);
                        std::cout << topicName(firstSplit[1]) << std::endl;
                        std::cout << firstSplit[1] << std::endl;
                    }
                }
                else
                {
                    if(error_ch == 0)
                        message(ERR_NOTONCHANNEL(clientServer[fd].ipclient, firstSplit[1]), fd);
                    else if(error_ch == 1 && error_rights == 0)
                        message(ERR_CHANOPRIVSNEEDED(clientServer[fd].ipclient,clientServer[fd].nickname,firstSplit[1]), fd);
                }
            }
        }
        else if(checkCommand(firstSplit[0]) == 7 && clientServer[fd].connected)
        {
            std::string line = "";
            int index;

            index = -1;
            if (clientServer[fd].connected)
            {
                if(firstSplit.size() < 2)
                    ERR_NEEDMOREPARAMS(clientServer[fd].nickname ,clientServer[fd].ipclient,firstSplit[0]);
                else
                {
                    if(operator_user(firstSplit[1], fd))
                    {
                        int index = searchForid(firstSplit[2]);
                        if(index == -1)
                            message(ERR_NOSUCHNICK(clientServer[fd].ipclient,clientServer[fd].nickname),fd);
                        else
                            kickUser(fd, index, firstSplit[1], reason(commandLine[i],fd));
                    }
                    else
                        message(ERR_CHANOPRIVSNEEDED(clientServer[fd].ipclient,clientServer[fd].nickname,firstSplit[1]),fd);
                }
            }
        }
          else if(checkCommand(firstSplit[0]) == 8 && clientServer[fd].connected)
        {
            
            if (clientServer[fd].connected)
            {
                if(firstSplit.size() < 2)
                    ERR_NEEDMOREPARAMS(clientServer[fd].nickname ,clientServer[fd].ipclient,firstSplit[0]);
                else
                {
                    if (firstSplit.size() < 3)
                        message(ERR_NEEDMOREPARAMS(clientServer[fd].nickname, clientServer[fd].ipclient , firstSplit[0]), fd);
                    int index = -1;
                    index = searchForid(firstSplit[1]);
                    std::cout << "fs1 " << firstSplit[1] << std::endl;
                    std::cout << "fs2 " << firstSplit[2] << std::endl;
                    if (availableChannel(firstSplit[2]))
                    {
                        if (!on_channel(firstSplit[1], fd))
                        {
                            if (operator_user(firstSplit[2], fd))
                            {
                                    
                                if (index == -1)
                                    message(ERR_NOSUCHNICK(clientServer[fd].ipclient,clientServer[fd].nickname), fd);
                                else
                                {
                                    message(RPL_INVITING(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[1], firstSplit[2]), fd);
                                    message(RPL_INVITE(clientServer[fd].nickname, clientServer[fd].username, clientServer[fd].ipclient, firstSplit[1], firstSplit[2]), index);
                                    inserUser(firstSplit[1], firstSplit[2]);
                                }
                            }
                            else
                                message(ERR_CHANOPRIVSNEEDED(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[1]), fd);
                        }
                        else
                            message(ERR_USERONCHANNEL(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[1], firstSplit[2]), fd);

                    }
                    else
                        message(ERR_NOSUCHCHANNEL(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[2]),fd);

                }
                
            }
            
        }
        else if (checkCommand(firstSplit[0]) == 9 && clientServer[fd].connected)
            {
                std::map<int ,int> modes;

                if (firstSplit.size() < 3)
                    message(ERR_NEEDMOREPARAMS(clientServer[fd].nickname, clientServer[fd].ipclient , firstSplit[0]), fd);
                else
                {
                    int pos;
                    pos = idChannel(firstSplit[1], fd);
                    if (pos == -1)
                    {
                        message(ERR_NOTONCHANNEL(clientServer[fd].ipclient, firstSplit[1]),fd);
                        return;
                    }
                    if (pos == -2)
                    {
                        message(ERR_NOSUCHCHANNEL(clientServer[fd].ipclient,clientServer[fd].nickname, firstSplit[1]),fd);
                        return;
                    }
                    if (clientServer[fd].channel[pos].op != 1)
                    {
                        message(ERR_NOTOP(clientServer[fd].ipclient, firstSplit[1]), fd);
                        return ;
                    }   
                    std::string     key = "";
                    std::string     nick = "";
                    std::string     limit = "";
                    std::vector<std::string> args(3);
                    int i;
                    int j = 0;
                    int size = 0;
                    int oi = 0;
                    char mode = 0;
                    mode = 1 << POSITIF;
                    char used = 0;
                    bool n = 0;
                    bool p = 0;
                    bool enter = 0;
                    i = 2;
                    j = 0;
                    int pkey = -1;
                    
                    while (i < firstSplit.size())
                    {
                        j = 0;
                        size = firstSplit[i].size();
                        oi = i;
                        
                        while (j < size)
                        {
                            if(!(firstSplit[oi][j] == 't' || firstSplit[oi][j] == 'i' || firstSplit[oi][j] == 'k' ||  firstSplit[oi][j] == 'o' || firstSplit[oi][j] == 'l' || firstSplit[oi][j] == '+' || firstSplit[oi][j] == '-' ))
                                message(ERR_UNKNOWNMODE(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[oi][j]), fd);
                            else if (firstSplit[oi][j] == '-' || firstSplit[oi][j] == '+')
                            {
                                if (firstSplit[oi][j] == '-')
                                    mode &= ~(1 << POSITIF);
                                else
                                    mode |= (1 << POSITIF);
                            }
                            else if(firstSplit[oi][j] == 'i')
                            {
                                used |= 1 << INVITE_ONLY;
                                enter = 1;
                                if(mode & (1 << POSITIF))
                                    mode |= 1 << INVITE_ONLY;
    
                                else
                                    mode &= ~ (1 << INVITE_ONLY);
                                mode |= 1 << POSITIF;
                            }
                            else if(firstSplit[oi][j] == 't')
                            {
                                used |= 1 << TOPIC;
                                enter = 1;
                                if(mode & (1 << POSITIF))
                                    mode |= 1 << TOPIC;
                                else
                                    mode&= ~ (1 << TOPIC);
                                mode |= 1 << POSITIF;
                            }
                            else if (firstSplit[oi][j] == 'k' || firstSplit[oi][j] == 'o' || firstSplit[oi][j] == 'l')
                            {
                               
                                enter = 1;

                                i++;
                                while(i <= pkey)
                                    i++;
                                // std::cout << "pkey" << pkey << std::endl;
                                //  std::cout << "i" << i << std::endl;
                                //  std::cout << "the key "<< firstSplit[i] << std::endl;
                                if(firstSplit[oi][j] == 'k')
                                {
                                    
                                    if(mode & (1 << POSITIF))
                                        mode |= 1 << KEY;
                                    else
                                        mode &= ~ (1<< KEY);
                                }
                                else if(firstSplit[oi][j] == 'o')
                                {
                                    if(mode & (1 << POSITIF))
                                        mode |= 1 << OPERATOR;
                                    else
                                        mode &= ~ (1 << OPERATOR);
                                }
                                else if(firstSplit[oi][j] == 'l')
                                {
                                    used |= 1 << LIMIT;
                                    if(mode & (1 << POSITIF))
                                        mode |= 1 << LIMIT;
                                    else
                                        mode &= ~ (1 << LIMIT);
                                }
                                if (i < firstSplit.size())
                                {
                                    if (firstSplit[oi][j] == 'k') 
                                    {
                                        used |= 1 << KEY;
                                        args[0] = firstSplit[i];
                                        std::cout << "key " << firstSplit[i] << std::endl;
                                    }   
                                       
                                    else if(firstSplit[oi][j] == 'l')
                                    {
                                        used |= 1 << LIMIT;
                                        args[1] = firstSplit[i];
                                        std::cout << args[1] << std::endl;
                                        std::cout << "limit " << firstSplit[i] << std::endl;
                                    }                                       
                                    else if(firstSplit[oi][j] == 'o')
                                    {
                                        used |= 1 << OPERATOR;
                                        args[2] = firstSplit[i];
                                    }
                                }
                                    
                                else
                                {
                                     if (firstSplit[oi][j] == 'k') 
                                        args[0] = "";
                                    else if(firstSplit[oi][j] == 'l')
                                        args[1] = "";
                                    else if(firstSplit[oi][j] == 'o')
                                        args[2] = "";
                                }
                                pkey = i;
                                mode |= 1 << POSITIF;
                            }
                            j++;
                        }
                        i++;
                    }
                    
                    if(enter)
                        applicateMode(mode, firstSplit[1], fd, used, args);
                    // if(mode & (1<<POSITIF))
                    //    std::cout << " +p" <<std::endl;
                    // else
                    //     std::cout << " -p" <<std::endl;
                    // if(mode & (1<<INVITE_ONLY))
                    //    std::cout << " +i" <<std::endl;
                    // else
                    //     std::cout << " -i" <<std::endl;
                    // if(mode & (1 << LIMIT))
                    //     std::cout << " +l" <<std::endl;
                    // else
                    //     std::cout << " -l" <<std::endl;
                    // if(mode & (1 << TOPIC))
                    //    std::cout << " +t" <<std::endl;
                    // else
                    //     std::cout << " -t" <<std::endl;
                    //  if(mode & (1 << KEY))
                    //    std::cout << " +k" <<std::endl;
                    // else
                    //     std::cout << " -k" <<std::endl;
                    //   if(mode & (1 << LIMIT))
                    //    std::cout << " +l" <<std::endl;
                    // else
                    //     std::cout << " -l" <<std::endl;
                    // std::cout << "--" << std::endl;
                }
            }
            else if(checkCommand(firstSplit[0]) == 10 && clientServer[fd].connected)
            {
                if (firstSplit.size() == 3)
                {
                    int j = 0;
                    while (j < clientServer[fd].channel.size())
                    {
                        if (clientServer[fd].channel[j].name == firstSplit[1])
                        {
                            clientServer[fd].channel.erase(clientServer[fd].channel.begin() + j);
                            message(RPL_QUIT(clientServer[fd].nickname, clientServer[fd].nickname, firstSplit[2]), fd);
                            kickclients(firstSplit[1], fd);
                            break;
                        }
                        j++;
                    }
                }
            }
            else if(checkCommand(firstSplit[0]) == 0)
                message(RPL_COMMAND_NOT_FOUND( clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[0]),fd);
            else 
                message("you should be connected first\r\n", fd);
          i++;
    }
}



int main(int argc, char **argv) 
{
    server sobj;

    if (argc == 3)
    {
         
        std::string     ip_client;
        std::string     password;
        std::string     f1;
        std::string     f2;
        std::string     f3;
        int s;
        char *c;
        int serv_socket;
        struct sockaddr_in ServerAddr, ClientAddr;

        socklen_t len = sizeof(ClientAddr);

        if ((s = convert_test(argv[1])) == -1 || convert_test(argv[2]) == -1   || argv[2][0] == 0)
        {
            std::cout << "Empty password or wrong port number " << std::endl;
            exit(1);
        }
        password = argv[2];
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_addr.s_addr = INADDR_ANY;
        ServerAddr.sin_port = htons(s);
        serv_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (serv_socket == -1) 
        {
            std::cout << "error creating socket" << std::endl;
            return 1;
        }
        if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &ServerAddr, sizeof(ServerAddr)) < 0) 
        {
            perror("Setsockopt failed");
            exit(EXIT_FAILURE);
        }
        if (bind(serv_socket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) == -1) 
        {
            std::cout << "error binding address" << std::endl;
            return 1;
        }
        if (listen(serv_socket, 5) == -1) 
        {
            std::cout << "error" << std::endl;
            return 1;
        }
//i and o the last one .
        std::cout << "Server listening" << std::endl;

        // Set server socket to non-blocking
        if (fcntl(serv_socket, F_SETFL, O_NONBLOCK) < 0) 
        {
            perror("Failed to set server socket to non-blocking");
            close(serv_socket);
            exit(EXIT_FAILURE);
        }

        // Vector to hold pollfd structs
        std::vector<pollfd> fds;
        pollfd server_pollfd;
        server_pollfd.fd = serv_socket;
        server_pollfd.events = POLLIN;
        std::string b;
        fds.push_back(server_pollfd);

        while (1) 
        {
            // Call poll to wait for events
            int pollResult = poll(fds.data(), fds.size(), -1); 
            if (pollResult == -1) 
            {
                std::cerr << "Error in poll()" << std::endl;
                break;
            }
            if (fds[0].revents & POLLIN)
            {
                int client_socket = accept(serv_socket, (struct sockaddr *)&ClientAddr, &len);
                if (client_socket < 0) 
                {
                    std::cerr << "Error accepting connection" << std::endl;
                    continue;
                }
                std::cout << "New client connected. Client socket: " << client_socket << std::endl;
                ip_client = inet_ntoa(ClientAddr.sin_addr);
                if (!ip_client.c_str()) 
                std::cerr << "inet_ntoa failed.\n";
                // Set client socket to non-blocking
                if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0) 
                {
                    perror("Failed to set client socket to non-blocking");
                    close(client_socket);
                    continue;
                }
                
                // Add client socket to vector
                pollfd client_pollfd;
                client_pollfd.fd = client_socket;
                client_pollfd.events = POLLIN;
                fds.push_back(client_pollfd);
                
            }
            for (size_t i = 1; i < fds.size(); ++i) 
            {
                if (fds[i].revents & POLLIN) 
                {
                    char buffer[1024];
                    int recvResult = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (recvResult < 0)
                        std::cerr << "Error receiving data from client" << std::endl;
                    else if (recvResult == 0) 
                    {
                        std::cout << "Client disconnected. Socket: " << fds[i].fd << std::endl;
                        sobj.clientServer.erase(fds[i].fd);
                        close(fds[i].fd);
                        std::swap(fds[i], fds.back());
                        fds.pop_back();
                        i--;
                    } 
                    else 
                    {
                        buffer[recvResult] = '\0';
                        sobj.searchAdd(fds[i].fd, ip_client);
                        b += buffer;
                        if(endOfCommand(b))
                        {
                            std::vector<std::string> vt = splitByCR(b);
                            // int j = 0;
                            // while(j < vt.size())
                            // {
                            //     std::cout << vt[j] << std::endl;
                            //     j++;
                            // }
                            sobj.commandApply(fds[i].fd, vt, password);
                            b = "";
                        }
                    }
                }
            }
        }
        close(serv_socket);
    }
    return 0;
}
