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
#include "Channel.hpp"
#include "User.hpp"
#include <map>

#define MAX_CONNECTIONS 11


class Server
{
    private:
    int     port;
    std::string password;
    void initializePollFds();
    std::vector<User*> users;
    std::map<std::string, Channel*> channels;

    public:
    ~Server();
    static const int maxSize = 11;
    int clientAmount;
    int serverSocket;
    struct pollfd pollfds[10 + 1];

    
    void connectToServer(std::vector<User> &users);
    void disconnectClient(std::vector<User> &users, int index);

    void initializeSocket(std::string portStr, std::string password);

    int getIndexofUser(std::vector<User> users, int index);
    std::string parseNickname(std::vector<User> users, std::string nickname);
    void checkPrivateMessage(std::vector<User> users, User user, std::string message);
    bool validateIncomingMessage (std::vector<User> &users, std::string &message, int index);
    std::string getPassword ();
    int getCurrentUsersToPoll();

    std::vector<Channel*> getUserChannels(User* user);
    void removeUsers(User* user);
    void removePollFd(int socket);
    void removePassword(User* user, Channel* channel, const std::string& mode);
    void removeUserLimit(User* user, Channel* channel, const std::string& mode);
    User* getUserByNickname(const std::string& nickname, std::vector<User>& users);
    User* getUserByNick(const std::string& nickname);

    std::string trim(const std::string& str);
    bool hasUserTest(User* user);
    void joinChannel(User* user, const std::string& channelName);

    void handleJoinCommand(User* user, const std::string& channelName);
    void handlePartCommand(User* user, const std::string& channelName);
    void handleQuitCommand(User* user, const std::string& reason, std::vector<User>& users);
    void handlePrivmsgCommand(User* user, const std::string& parameters, std::vector<User>& users);
    void handleKickCommand(User* user, const std::string& parameters, std::vector<User>& users);
    void handleModeCommand(User* user, const std::string& parameters, std::vector<User>& users);
    void processClientMessage(User& user, const std::string& message, std::vector<User>& users);
    void handleKeyMode(User* user, Channel* channel, const std::string& mode, const std::string& parameters);
    void handleLimitMode(User* user, Channel* channel, const std::string& mode, const std::string& parameters);
    void handleTopicProtectionMode(User* user, Channel* channel, const std::string& mode);
    void handleInviteCommand(User* user, const std::string& parameters, std::vector<User>& users);
    void handleInviteOnlyMode(User* user, Channel* channel, const std::string& mode);
    void handleOperatorMode(User* user, Channel* channel, const std::string& mode, const std::string& targetNickname, std::vector<User>& users);

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