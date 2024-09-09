#include "client.hpp"

void    client::addFd(int fd)
{
    this->fd = fd;
    passB = true;
}

void    client::addNickname(std::string const nickname)
{
    
    if (!nickname.empty())
    {
        this->nickname =  nickname;
        nicknameB = 1;
        if (usernameB == 1)
        {
            connected = 1;
            std::cout << username << std::endl;
        }
    }
   
}

void    client::addUser(std::string const username)
{
    if (!username.empty())
    {
        this->username = username;
        usernameB = 1;
        if (nicknameB == 1)
        {
            connected = 1;
            std::cout << "this" << nickname  << std::endl;
        }
    }
}

client::client()
{
    duplicateNickname = 0;                                                                                          
    passB = 0;
    fd = 0;
    nicknameB = 0;
    usernameB = 0;
    connected = 0;
    nickname = "";
    username = "";
}