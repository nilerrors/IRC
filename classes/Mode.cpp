#include "../headers/Server.hpp"

void Server::handleModeCommand(User* user, const std::string& parameters, std::vector<User>& users) {
    if (!user) {
        std::cerr << "Error: Invalid user pointer in handleModeCommand." << std::endl;
        return;
    }

    std::istringstream iss(parameters);
    std::string channelName, mode_and_argument;
    iss >> channelName;
    std::getline(iss, mode_and_argument); // get rest of line including mode and optional arg

    // Trim leading/trailing whitespace
    mode_and_argument = trim(mode_and_argument);

    std::string mode, argument;
    size_t space_pos = mode_and_argument.find(' ');
    if (space_pos == std::string::npos) {
        mode = trim(mode_and_argument);
    } else {
        mode = trim(mode_and_argument.substr(0, space_pos));
        argument = trim(mode_and_argument.substr(space_pos + 1));
    }

    // Debug print
    std::cout << "[DEBUG] Parsed MODE command - Channel: '" << channelName
              << "', Mode: '" << mode << "', Argument: '" << argument << "'\n";

    if (channelName.empty() || mode.empty()) {
        std::string errorMsg = "461 " + user->nickname + " MODE :Not enough parameters\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Check if the channel exists
    if (channels.find(channelName) == channels.end()) {
        std::string errorMsg = "403 " + user->nickname + " " + channelName + " :No such channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    Channel* channel = channels[channelName];

    // Check if the user is in the channel
    if (!channel->hasUser(user)) {
        std::string errorMsg = "442 " + user->nickname + " " + channelName + " :You're not on that channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Check if the user is a channel operator
    if (!channel->isOperator(user)) {
        std::string errorMsg = "482 " + user->nickname + " " + channelName + " :You're not a channel operator\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Handle mode logic
    if (mode == "+o" || mode == "-o") {
        if (argument.empty()) {
            std::string errorMsg = "461 " + user->nickname + " MODE :Not enough parameters for +o/-o\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }
        handleOperatorMode(user, channel, mode, argument, users);
    } else if (mode == "+i" || mode == "-i") {
        handleInviteOnlyMode(user, channel, mode);
    } else if (mode == "+t" || mode == "-t") {
        handleTopicProtectionMode(user, channel, mode);
    } else if (mode == "+l") {
        if (argument.empty()) {
            std::string errorMsg = "461 " + user->nickname + " MODE :Need user limit\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }
        handleLimitMode(user, channel, mode, argument);
    } else if (mode == "-l") {
        removeUserLimit(user, channel, mode);
    } else if (mode == "+k") {
        if (argument.empty()) {
            std::string errorMsg = "461 " + user->nickname + " MODE :Need channel key\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }
        handleKeyMode(user, channel, mode, argument);
    } else if (mode == "-k") {
        removePassword(user, channel, mode);
    } else {
        std::string errorMsg = "472 " + user->nickname + " " + mode + " :Unknown mode\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
    }
}

void Server::handleOperatorMode(User* user, Channel* channel, const std::string& mode, const std::string& targetNickname, std::vector<User>& users) {
    User* targetUser = getUserByNickname(targetNickname, users);
    if (!targetUser) {
        std::string errorMsg = "401 " + user->nickname + " " + targetNickname + " :No such nick/channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    if (!channel->hasUser(targetUser)) {
        std::string errorMsg = "441 " + user->nickname + " " + targetNickname + " " + channel->getName() + " :They aren't on that channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string modeMsg = ":" + user->nickname + " MODE " + channel->getName() + " " + mode + " " + targetNickname + "\r\n";
    if (mode == "+o") {
        channel->addOperator(targetUser);
    } else {
        channel->removeOperator(targetUser);
    }

/*     std::vector<User*> usersInChannel = channel->getUsers();
    for (std::vector<User*>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); ++it) {
        User* u = *it;
        send(u->socket, modeMsg.c_str(), modeMsg.length(), 0);
    } */ 
}

void Server::handleInviteOnlyMode(User* user, Channel* channel, const std::string& mode) {
    if (mode == "+i") {
        channel->setInviteOnly(true);
    } else if (mode == "-i") {
        channel->setInviteOnly(false);
    }
    std::string response = ":" + user->nickname + " MODE " + channel->getName() + " " + mode + "\r\n";
    send(user->socket, response.c_str(), response.length(), 0);
}

void Channel::setTopicProtected(bool enabled) {
    this->topicProtected = enabled;
}

bool Channel::isTopicProtected() const {
    return this->topicProtected;
}

void Server::handleTopicProtectionMode(User* user, Channel* channel, const std::string& mode) {
    bool enable = (mode == "+t");
    channel->setTopicProtected(enable);

    std::string modeMsg = ":" + user->nickname + " MODE " + channel->getName() + " " + mode + "\r\n";
    std::vector<User*> usersInChannel = channel->getUsers();
/*     for (std::vector<User*>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); ++it) {
        User* u = *it;
        send(u->socket, modeMsg.c_str(), modeMsg.length(), 0);
    } */
}

void Server::removeUserLimit(User* user, Channel* channel, const std::string& mode) {
    channel->setUserLimit(0); // setting to 0 disables the limit

    std::string modeMsg = ":" + user->nickname + " MODE " + channel->getName() + " -l\r\n";
    std::vector<User*> usersInChannel = channel->getUsers();

    for (size_t i = 0; i < usersInChannel.size(); ++i) {
        send(usersInChannel[i]->socket, modeMsg.c_str(), modeMsg.length(), 0);
    }

    std::cout << user->nickname << " removed user limit from " << channel->getName() << std::endl;
}

void Channel::removeChannelUserLimit() {
    this->userLimit = -1; // or 0, depending on how you represent no limit
}

void Server::handleLimitMode(User* user, Channel* channel, const std::string& mode, const std::string& parameters) {
    if (mode == "+l") {
        size_t pos = parameters.find_last_of(' ');
        if (pos == std::string::npos) return;

        std::string limitStr = parameters.substr(pos + 1);
        int limit = std::atoi(limitStr.c_str());
        channel->setUserLimit(limit);
    } else {
        channel->removeChannelUserLimit();
    }

    std::string modeMsg = ":" + user->nickname + " MODE " + channel->getName() + " " + mode + "\r\n";
    std::vector<User*> usersInChannel = channel->getUsers();
/*     for (std::vector<User*>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); ++it) {
        User* u = *it;
        send(u->socket, modeMsg.c_str(), modeMsg.length(), 0);
    } */
}

void Server::removePassword(User* user, Channel* channel, const std::string& mode) {
    channel->removeChannelPassword();

    std::string modeMsg = ":" + user->nickname + " MODE " + channel->getName() + " -k\r\n";
    std::vector<User*> usersInChannel = channel->getUsers();

    for (size_t i = 0; i < usersInChannel.size(); ++i) {
        send(usersInChannel[i]->socket, modeMsg.c_str(), modeMsg.length(), 0);
    }

    std::cout << user->nickname << " removed password from " << channel->getName() << std::endl;
}

void Channel::removeChannelPassword() {
    this->password.clear();
}

void Server::handleKeyMode(User* user, Channel* channel, const std::string& mode, const std::string& parameters) {
    if (mode == "+k") {
        size_t pos = parameters.find_last_of(' ');
        if (pos == std::string::npos) return;

        std::string key = parameters.substr(pos + 1);
        channel->setPassword(key);
    } else {
        channel->removeChannelPassword();
    }

    std::string modeMsg = ":" + user->nickname + " MODE " + channel->getName() + " " + mode + "\r\n";
    std::vector<User*> usersInChannel = channel->getUsers();
    for (std::vector<User*>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); ++it) {
        User* u = *it;
        send(u->socket, modeMsg.c_str(), modeMsg.length(), 0);
    }
}
