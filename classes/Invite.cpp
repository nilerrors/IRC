#include "../headers/Server.hpp"

void Channel::inviteUser(User* user) {
    if (std::find(invitedUsers.begin(), invitedUsers.end(), user) == invitedUsers.end()) {
        invitedUsers.push_back(user);
        std::cout << "[DEBUG] User " << user->nickname << " invited to " << name << "\n";
    }
}

bool Channel::isUserInvited(User* user) const {
    // Debug message to check the user being invited
    std::string debugMsg = "[DEBUG] Checking if user " + user->nickname + " is invited to channel " + this->getName() + "\r\n";
    send(user->socket, debugMsg.c_str(), debugMsg.length(), 0);
    
    // Print all invited users for debugging
    std::string invitedListMsg = "[DEBUG] Invited users in channel " + this->getName() + ": ";
    for (size_t i = 0; i < invitedUsers.size(); ++i) {
        invitedListMsg += invitedUsers[i]->nickname + " ";
    }
    invitedListMsg += "\r\n";
    send(user->socket, invitedListMsg.c_str(), invitedListMsg.length(), 0);
    
    // Check if the user is in the invited list
    for (size_t i = 0; i < invitedUsers.size(); ++i) {
        if (invitedUsers[i] == user) {
            return true;
        }
    }
    return false;
}

void Channel::removeInvitedUser(User* user) {
    invitedUsers.erase(std::remove(invitedUsers.begin(), invitedUsers.end(), user), invitedUsers.end());
}

void Server::handleInviteCommand(User* user, const std::string& parameters, std::vector<User>& users) {
    if (!user) {
        std::cerr << "Error: Invalid user pointer in handleInviteCommand." << std::endl;
        return;
    }

    std::istringstream iss(parameters);
    std::string nickname, channelName;
    iss >> channelName >> nickname;

    // Check if both parameters are provided
    if (channelName.empty() || nickname.empty()) {
        std::string errorMsg = "461 " + user->nickname + " INVITE :Not enough parameters\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Check if channel exists
    if (channels.find(channelName) == channels.end()) {
        std::string errorMsg = "403 " + user->nickname + " " + channelName + " :No such channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    Channel* channel = channels[channelName];

    // Check if the user is a channel operator
    if (!channel->isOperator(user)) {
        std::string errorMsg = "482 " + user->nickname + " " + channelName + " :You're not a channel operator\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Check if the user to be invited exists
    User* targetUser = getUserByNickname(nickname, users);
    if (!targetUser) {
        std::string errorMsg = "401 " + user->nickname + " " + nickname + " :No such nick/channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    channel->inviteUser(targetUser);

    // If the channel is invite-only and the user is not invited yet
    if (channel->isInviteOnly() && std::find(channel->getInvitedUsers().begin(), channel->getInvitedUsers().end(), targetUser) == channel->getInvitedUsers().end()) {
        std::string errorMsg = "473 " + user->nickname + " " + channelName + " :Invite-only channel\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Add the user to the invited list
    channel->inviteUser(targetUser);

    // Send confirmation message
    std::string response = ":" + user->nickname + " INVITE " + nickname + " " + channelName + "\r\n";
    send(user->socket, response.c_str(), response.length(), 0);
}
