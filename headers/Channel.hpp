#ifndef CHANNEL_H
# define CHANNEL_H
#include "User.hpp"
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


class Channel
{
    private:

    public:
    Channel (void);
    std::string name;
    std::string topic;
    // std::vector<User> users;    

};

#endif