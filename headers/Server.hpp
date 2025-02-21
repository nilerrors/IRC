#ifndef SERVER_H
# define SERVER_H
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
#include "Utility.hpp"
#include "../headers/User.hpp"



class Server
{
    private:
    int     port;
    std::string password;
    void initializePollFds();


    public:
    static const int maxSize = 11;
    int clientAmount;
    int serverSocket;
    struct pollfd pollfds[10 + 1];

    
    void connectToServer(std::vector<User> &users);
    void disconnectClient(std::vector<User> &users, int index);
    void initializeSocket(std::string portStr, std::string password);
    int getIndexofUser(std::vector<User> users, int index);
    void authenticate(std::vector<User> &users, User &user, std::string message, int index);
    bool irssiAuthenticate(std::vector<User> &users, User &user, std::string message);
    bool netcatAuthenticate(std::vector<User> &users, User &user, std::string message);
    std::string parseNickname(std::vector<User> users, std::string nickname);
    void checkPrivateMessage(std::vector<User> users, User user, std::string message);
    bool validateIncomingMessage (std::vector<User> &users, std::string &message, int index);
    bool checkInvalidCharacters (std::string message);


    class ServerSocketFail: public std::exception 
    {
        public:
            virtual const char* what() const throw();
    };

    class PortOutofRange: public std::exception 
    {
        public:
            virtual const char* what() const throw();
    };
};

#endif