#ifndef CHANNEL_H
# define CHANNEL_H

#include <string>
#include <vector>
#include <algorithm>
#include "User.hpp"

class User;

class Channel {
private:
    std::string name;
    std::string topic;
    std::vector<User*> users;
    std::vector<User*> operators;
    std::string password;
    bool InviteOnly;
    bool PasswordProtected;
    int userLimit;
    bool topicProtected;
    std::vector<User*> invitedUsers;

public:
    Channel(const std::string& name);
    ~Channel();

    void addUser(User* user);
    void removeUser(User* user);
    bool hasUser(User *user);
    void addOperator(User* user);
    void removeOperator(User* user);
    void inviteUser(User* user);
    void setTopic(const std::string& topic);
    void setPassword(const std::string& password);
    void setInviteOnly(bool inviteOnly);
    void setUserLimit(int limit);
    void setTopicProtected(bool enabled);
    void removeChannelUserLimit();
    void removeChannelPassword();
    void removeInvitedUser(User* user);

    bool isUserInvited(User* user) const;
    bool isTopicProtected() const;
    bool isUserInChannel(User* user) const;
    bool isOperator(User* user) const;
    bool isPasswordCorrect(const std::string& password) const;
    bool isPasswordProtected() const;
    bool isFull() const;
    bool isEmpty() const;
    bool isInviteOnly() const;
    std::string getName() const;
    std::string getTopic() const;
    std::vector<User*> getUsers() const;
    std::vector<User*> getOperators() const;
    std::vector<User*> getInvitedUsers() const;
};

#endif