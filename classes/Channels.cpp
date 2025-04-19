#include "../headers/Channel.hpp"

Channel::Channel(const std::string& name) : name(name), InviteOnly(false), PasswordProtected(false), userLimit(-1) {}

Channel::~Channel() {}

void Channel::addUser(User* user) {
    users.push_back(user);
}

void Channel::removeUser(User* user) {
    // Remove the user from the users list
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it) {
        if (*it == user) {
            users.erase(it);
            break;
        }
    }

    // Remove the user from the operators list (if applicable)
    for (std::vector<User*>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (*it == user) {
            operators.erase(it);
            break;
        }
    }
}

bool Channel::hasUser(User* user) {
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it) {
        if (*it == user) {
            return true;
        }
    }
    return false;
}

void Channel::addOperator(User* user) {
    operators.push_back(user);
}

void Channel::removeOperator(User* user) {
    operators.erase(std::remove(operators.begin(), operators.end(), user), operators.end());
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

void Channel::setPassword(const std::string& password) {
    this->password = password;
    this->PasswordProtected = true;
}

void Channel::setInviteOnly(bool invite) {
    this->InviteOnly = invite;
}

bool Channel::isInviteOnly() const {
    return InviteOnly;
}

std::vector<User*> Channel::getInvitedUsers() const {
    return invitedUsers;
}

bool Channel::isPasswordProtected() const {
    return PasswordProtected;
}

void Channel::setUserLimit(int limit) {
    this->userLimit = limit;
}

bool Channel::isUserInChannel(User* user) const {
    return std::find(users.begin(), users.end(), user) != users.end();
}

bool Channel::isOperator(User* user) const {
    return std::find(operators.begin(), operators.end(), user) != operators.end();
}

bool Channel::isPasswordCorrect(const std::string& password) const {
    return this->password == password;
}

bool Channel::isFull() const {
    return userLimit != -1 && users.size() >= userLimit;
}

std::string Channel::getName() const {
    return name;
}

std::string Channel::getTopic() const {
    return topic;
}

std::vector<User*> Channel::getUsers() const {
    return users;
}

std::vector<User*> Channel::getOperators() const {
    return operators;
}
