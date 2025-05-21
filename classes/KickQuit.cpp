#include "../headers/Server.hpp"

std::vector<Channel*> Server::getUserChannels(User* user) {
    std::vector<Channel*> userChannels;
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->second->hasUser(user)) {
            userChannels.push_back(it->second);
        }
    }
    return userChannels;
}

void Server::removeUsers(User* user) {
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it) {
        if (*it == user) {
            users.erase(it);
            break;
        }
    }
}

std::string Server::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

void Server::handleKickCommand(User* user, const std::string& parameters, std::vector<User>& users) {
    if (!user) {
        std::cerr << "Error: Invalid user pointer in handleKickCommand." << std::endl;
        return;
    }

    // Parse the channel, target user, and optional reason
    size_t spacePos1 = parameters.find(' ');
    size_t spacePos2 = parameters.find(' ', spacePos1 + 1);

    if (spacePos1 == std::string::npos || spacePos2 == std::string::npos) {
        std::string errorMsg = "461 " + user->nickname + " KICK :Not enough parameters\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string channelName = parameters.substr(0, spacePos1);
    std::string targetNickname = parameters.substr(spacePos1 + 1, spacePos2 - (spacePos1 + 1));
    std::string reason = parameters.substr(spacePos2 + 1);

    // Remove leading ':' from the reason (if present)
    if (!reason.empty() && reason[0] == ':') {
        reason = reason.substr(1);
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

    // Check if the user has permission to kick (e.g., is a channel operator)
    if (!channel->isOperator(user)) {
        std::string errorMsg = "482 " + user->nickname + " " + channelName + " :You're not a channel operator\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Find the target user
    User* targetUser = getUserByNickname(targetNickname, users);
    if (!targetUser) {
        std::string errorMsg = "401 " + user->nickname + " " + targetNickname + " :No such nick/channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Check if the target user is in the channel
    if (!channel->hasUser(targetUser)) {
        std::string errorMsg = "441 " + user->nickname + " " + targetNickname + " " + channelName + " :They aren't on that channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Remove the target user from the channel
    channel->removeUser(targetUser);

    // Notify the channel about the kick
    std::string kickMsg = ":" + user->nickname + " KICK " + channelName + " " + targetNickname + " :" + reason + "\r\n";
    std::vector<User*> usersInChannel = channel->getUsers();
    for (size_t i = 0; i < usersInChannel.size(); ++i) {
        send(usersInChannel[i]->socket, kickMsg.c_str(), kickMsg.length(), 0);
    }

    std::cout << user->nickname << " kicked " << targetNickname << " from " << channelName << std::endl;
}
