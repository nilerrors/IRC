#include "../headers/Server.hpp"

void Server::handleJoinCommand(User* user, const std::string& channelName) {
    if (!user) {
        std::cerr << "Error: Invalid user pointer in handleJoinCommand." << std::endl;
        return;
    }

    if (channelName.empty() || channelName[0] != '#') {
        std::string errorMsg = "ERROR :Invalid channel name: " + channelName + "\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    std::string trimChannelName = trim(channelName);
    Channel* channel = NULL;

    // Check if the channel exists
    if (channels.find(trimChannelName) == channels.end()) {
        channel = new Channel(trimChannelName);
        channels[trimChannelName] = channel;
        channel->addOperator(user); // First user is the operator
    } else {
        channel = channels[trimChannelName];
    }

    if (!channel) {
        std::cerr << "Error: Channel pointer is null!" << std::endl;
        return;
    }

    // 🔒 Invite-only check
    if (channel->isInviteOnly()) {
        if (!channel->isUserInvited(user)) {
            std::string errorMsg = "473 " + user->nickname + " " + channelName + " :Cannot join channel (+i)\r\n";
            send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
            return;
        } else {
            channel->removeInvitedUser(user); // Optional: Remove user from invited list
        }
    }

    // ✅ Add the user to the channel
    channel->addUser(user);

    // 📢 Notify the user and others
    std::string joinMsg = ":" + user->nickname + " JOIN " + channelName + "\r\n";
    send(user->socket, joinMsg.c_str(), joinMsg.length(), 0); // Assuming you have a function like this

    // 👥 Send user list
    std::string namesMsg = "353 " + user->nickname + " = " + channelName + " :";
    std::vector<User*> users = channel->getUsers();
    for (size_t i = 0; i < users.size(); ++i) {
        User* u = users[i];
        namesMsg += u->nickname + " ";
    }
    namesMsg += "\r\n";
    send(user->socket, namesMsg.c_str(), namesMsg.length(), 0);

    std::string endOfNamesMsg = "366 " + user->nickname + " " + channelName + " :End of /NAMES list.\r\n";
    send(user->socket, endOfNamesMsg.c_str(), endOfNamesMsg.length(), 0);

    std::cout << user->nickname << " joined " << channelName << std::endl;
}

void Server::handlePartCommand(User* user, const std::string& channelName) {
    if (!user) {
        std::cerr << "Error: Invalid user pointer in handlePartCommand." << std::endl;
        return;
    }

    if (channelName.empty() || channelName[0] != '#') {
        std::string errorMsg = "ERROR :Invalid channel name: " + channelName + "\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    if (channels.find(channelName) == channels.end()) {
         std::string errorMsg = "ERROR :Channel " + channelName + " does not exist\r\n";
        send(user->socket, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }
    Channel* channel = channels[channelName];
    channel->removeUser(user);
    std::string partMsg = ":" + user->nickname + "!" + " PART :" + channelName + "\r\n";
    send(user->socket, partMsg.c_str(), partMsg.length(), 0);
    std::vector<User*> usersInChannel = channel->getUsers();
    for (size_t i = 0; i < usersInChannel.size(); ++i) {
        if (usersInChannel[i] != user) {
            send(usersInChannel[i]->socket, partMsg.c_str(), partMsg.length(), 0);
        }
    }
    std::cout << user->nickname << " has left " << channelName << std::endl;
    if (channel->getUsers().empty() && channel->getInvitedUsers().empty()) {
        delete channel;
        channels.erase(channel->getName());
    }
}
