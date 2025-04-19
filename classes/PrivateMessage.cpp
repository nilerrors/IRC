#include "../headers/Server.hpp"

User* Server::getUserByNickname(const std::string& nickname, std::vector<User>& users) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].nickname == nickname) {
            return &users[i]; // Return a pointer to the User object
        }
    }
    return NULL; // User not found
}

void Server::handlePrivmsgCommand(User* user, const std::string& parameters, std::vector<User>& users) {
    if (!user) {
        std::cerr << "Error: Invalid user pointer in handlePrivmsgCommand." << std::endl;
        return;
    }

    // Parse the target and message
    size_t spacePos = parameters.find(' ');
    if (spacePos == std::string::npos) {
        std::string errorMsg = "412 :No text to send\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string target = parameters.substr(0, spacePos);
    std::string message = parameters.substr(spacePos + 1);

    // Remove leading ':' from the message (if present)
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }

    // Check if the target is a channel
    if (target[0] == '#') {
        // Target is a channel
        if (channels.find(target) == channels.end()) {
            std::string errorMsg = "403 " + user->nickname + " " + target + " :No such channel\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        Channel* channel = channels[target];
        if (!channel->hasUser(user)) {
            std::string errorMsg = "442 " + user->nickname + " " + target + " :You're not on that channel\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        // Forward the message to all users in the channel (except the sender)
        std::string privmsg = ":" + user->nickname + " PRIVMSG " + target + " :" + message + "\r\n";
        std::vector<User*> usersInChannel = channel->getUsers();
        std::cout << "Userchannel size: " << usersInChannel.size() << std::endl;
        for (size_t i = 0; i < usersInChannel.size(); ++i) {
            if (usersInChannel[i] != user) {
                send(usersInChannel[i]->socket, privmsg.c_str(), privmsg.length(), 0);
            }
        }
    } else {
        // Target is a user
        User* targetUser = getUserByNickname(target, users);
        if (!targetUser) {
            std::string errorMsg = "401 " + user->nickname + " " + target + " :No such nick/channel\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        }

        // Forward the message to the target user
        std::string privmsg = ":" + user->nickname + "!" + " PRIVMSG " + target + " :" + message + "\r\n";
        send(targetUser->socket, privmsg.c_str(), privmsg.length(), 0);
    }
}
