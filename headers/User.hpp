#ifndef USER_H
# define USER_H
#include "Channel.hpp"

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>
# include <time.h>
# include <sys/time.h>
#include <string.h>
#include <vector>


class User
{
    private:

    public:
    User (void);
    std::vector<std::string> authIrssi;
    bool isAuthenticated;
    bool isPassAuthenticated;
    bool isNickAuthenticated;
    bool isUserAuthenticated;
    int clienNumber;
    std::string nickname;
};

#endif