#ifndef SERVER_H
# define SERVER_H
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include "Utility.hpp"
#include "../headers/User.hpp"



class Server
{
    private:
    int     port;
    std::string password;
    void initializePollFds();


    public:
    ~Server();
    static const int maxSize = 11;
    int serverSocket;
    struct pollfd pollfds[10 + 1];

    void connectToServer(std::vector<User> &users);
    void disconnectClient(std::vector<User> &users, int index);

    void initializeSocket(std::string portStr, std::string password);

    int getIndexofUser(std::vector<User> users, int index);
    int getCurrentUsersToPoll();
    void checkPrivateMessage(std::vector<User> users, User user, std::string message);
    bool validateIncomingMessage (std::vector<User> &users, std::string &message, int index);

    std::string getPassword ();

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