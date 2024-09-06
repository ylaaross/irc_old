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

int		server::modeChannel(std::string name)
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
            if(it->second.channel[i].name == channel && it->second.nickname != clientServer[fd].nickname)
            {
                b = 1;
                message(PRIVMSG_FORMAT(clientServer[fd].nickname,it->second.username,it->second.ipclient,it->second.channel[i].name, msg),it->first);
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
        std::cout << fd;
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
             exit(1);
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

int check_mode(char c)
{

    return (0);
}
void server::commandApply(int fd,  std::vector<std::string>commandLine, std::string password)
{
    int i = 0;
    while (i < commandLine.size())
    {
        std::vector<std::string>firstSplit = split(commandLine[i], ' ');
        if (firstSplit.size() > 1 && checkCommand(firstSplit[0]) == 1)
        {
            if(firstSplit[1] == password)
            {
                message("Correct password\n\r", fd);
                clientServer[fd].passB = 1;
            }
            else
                message("Wrong password\n", fd);
        }
    
        else if(checkCommand(firstSplit[0]) == 2)
        {
            if(clientServer[fd].passB)
            {
                clientServer[fd].addNickname(firstSplit[1]);
                message("nick connected\n", fd);
            }
        }
        else if(checkCommand(firstSplit[0]) == 3)
        {
            if (clientServer[fd].passB)
            {
                clientServer[fd].addUser(firstSplit[1]);
                message("User connected\n", fd);
            }
        }
        else if(checkCommand(firstSplit[0]) == 4)
        {
            if (clientServer[fd].connected)
            {
                std::vector<std::string>messagesSplit = split(commandLine[i], ':');
                if (checkChannelName(firstSplit[1]))
                {
                    if(availableChannel(firstSplit[1]))
                    {
                        if(channelMember(firstSplit[1],fd))
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
        else if (checkCommand(firstSplit[0]) == 5)
        {
            bool b = 0;
            int i = 0;
            int mode = 0;
            
            std::vector<std::string>channelSplited = split(firstSplit[1], ',');
            while(i < channelSplited.size())
            {
                if (checkChannelName(channelSplited[i]))
                {
                    if(availableChannel(channelSplited[i]))
                        clientServer[fd].channel.push_back(channels(channelSplited[i], modeChannel(channelSplited[i]), 0));
                    else
                        clientServer[fd].channel.push_back(channels(channelSplited[i], 0 , 1));
                    message(RPL_JOIN(clientServer[fd].nickname, clientServer[fd].username, channelSplited[i], clientServer[fd].ipclient) , fd);
                }
                else
                    message(ERR_NOSUCHCHANNEL(clientServer[fd].ipclient, clientServer[fd].nickname, channelSplited[i]), fd);
                i++;
            }
        }
        else if(checkCommand(firstSplit[0]) == 6)
        {
            int error_ch = 0;
            int error_rights = 0;
            if (clientServer[fd].connected)
            {
                error_ch = on_channel(firstSplit[1],fd);
                error_rights = operator_user(firstSplit[1], fd);
                if(error_ch && error_rights)
                {
                    std::vector<std::string>messagesSplit = split(commandLine[i], ':');
                    message(RPL_TOPIC(clientServer[fd].ipclient, clientServer[fd].nickname, firstSplit[1], messagesSplit[1]),fd);
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
        else if(checkCommand(firstSplit[0]) == 7)
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
          else if(checkCommand(firstSplit[0]) == 8)
        {
            
            if (clientServer[fd].connected)
            {
                if(firstSplit.size() < 3)
                {
                    std::cout << "lbts akhouya";
                    ERR_NEEDMOREPARAMS(clientServer[fd].nickname ,clientServer[fd].ipclient,firstSplit[0]);
                }
                else
                {
                    if (firstSplit.size() < 3)
                        message(ERR_NEEDMOREPARAMS(clientServer[fd].nickname, clientServer[fd].ipclient , firstSplit[0]), fd);
                    int index = -1;
                    index = searchForid(firstSplit[1]);
                    if (availableChannel(firstSplit[2]))
                    {
                        if (!on_channel(firstSplit[2], fd))
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
        else if (checkCommand(firstSplit[0]) == 9)
            {
                std::map<int ,int> modes;

                if (firstSplit.size() < 3)
                    message(ERR_NEEDMOREPARAMS(clientServer[fd].nickname, clientServer[fd].ipclient , firstSplit[0]), fd);
                else
                {
                    int i;
                    int j;
                    int size;
                    int oi;
                    bool n = 0;
                    bool p = 0;
                    i = 2;
                    j = 0;
                    
                    
                    while (i < firstSplit.size())
                    {
                        j = 0;
                        size = firstSplit[i].size();
                        oi = i;

                        while (j < size)
                        {
                            if (firstSplit[oi][j] == '-' || firstSplit[oi][j] == '+')
                            {
                                if (firstSplit[oi][j] == '-')
                                {
                                    n = 1;
                                    p = 0;
                                }
                                else
                                {
                                    p = 1;
                                    n = 0;
                                }
                            }
                            if (firstSplit[oi][j] == 'k' || firstSplit[oi][j] == 'o')
                            {
                                i++;
                                if (i < firstSplit.size())
                                    std::cout << "'" << firstSplit[oi][j] << "'" <<  " key " << firstSplit[i] << std::endl;
                                else
                                    std::cout  << "'" << firstSplit[oi][j] << "'" << " no key" << std::endl;
                            }
                            else
                                std::cout << firstSplit[oi][j] << std::endl;
                            j++;
                        }
                        i++;
                    }
                    std::cout << "--" << std::endl;
                }
            }
          i++;
    }
}


int main(int argc, char **argv) 
{
    server sobj;

    if (argc == 3)
    {
         
        std::string ip_client;
        std::string password;
        std::string f1;
        std::string f2;
        std::string f3;
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
        if (fcntl(serv_socket, F_SETFL, O_NONBLOCK) < 0) {
            perror("Failed to set server socket to non-blocking");
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
                if (client_socket < 0) {
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
                        close(fds[i].fd);
                        std::swap(fds[i], fds.back());
                        fds.pop_back();
                        i--;
                    } 
                    else 
                    {
                        buffer[recvResult] = '\0';
                        sobj.searchAdd(fds[i].fd, ip_client);
                        b = buffer;
                        std::vector<std::string> vt = splitByCR(b);
                        int j = 0;
                        while(j < vt.size())
                        {
                            std::cout << vt[j] << std::endl;
                            j++;
                        }
                        sobj.commandApply(fds[i].fd, vt, password); 
                    }
                }
            }
        }
        close(serv_socket);
    }
    return 0;
}
