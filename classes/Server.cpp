#include "../headers/Server.hpp"

void Server::initializeSocket(std::string portStr, std::string password)
{
    int bindResult,listenResult;
    int socketOptions;
    struct sockaddr_in addr;
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int reUse = 1;

    if (toInt(portStr, &this->port) == false)
            throw Server::PortOutofRange();

    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketOptions = setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &reUse, sizeof(reUse));

    bindResult = bind(this->serverSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1)
        throw Server::ServerSocketFail();

    listenResult = listen(this->serverSocket, 5);
    if (listenResult == -1)
        throw Server::ServerSocketFail();
    initializePollFds();
    this->clientAmount = 0;
    this->password = trim(password);
    std::cout << "Server initialization successful\n"; 
}

void Server::connectToServer(std::vector<User> &users)
{
    struct sockaddr_in cliaddr;
    int client_socket;
    int addrlen = sizeof(cliaddr);
    int i = 1;

    client_socket = accept(this->serverSocket, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
    while (i < this->maxSize)
    {
        if (this->pollfds[i].fd == 0)
        {
            this->pollfds[i].fd = client_socket;
            this->pollfds[i].events = POLLIN | POLLPRI;
            this->clientAmount++;
            User a = User();
            a.clienNumber = i;
            users.push_back(a);
            std::cout << "User joined the server" << std::endl;
            return ;
        }
        i++;
    }
    std::cout << "Server is full" << std::endl;
    std::string quitMsg = "ERROR :Closing Link: localhost (Server is full)\r\n";
    send(client_socket,quitMsg.c_str(),quitMsg.length(),0);
    close(client_socket);
    return ;
}

void Server::disconnectClient(std::vector<User> &users, int index)
{
    std::cout << "i number to disconnect to server: " << index << std::endl;
    std::string quitMsg = "ERROR :Closing Link: localhost (Server disconnected)\r\n";
    send(this->pollfds[index].fd,quitMsg.c_str(),quitMsg.length(),0);
    // close(this->pollfds[index].fd);
    this->pollfds[index].fd = 0;
    this->pollfds[index].events = 0;
    this->pollfds[index].revents = 0;
    this->clientAmount--;
    std::cout << "index of disconnection vector: " << this->getIndexofUser(users,index)<< std::endl;
    users.erase(users.begin() + this->getIndexofUser(users,index));
}

void Server::authenticate(std::vector<User> &users, User &user, std::string message, int index)
{
    if (message.substr(0,8).compare("CAP LS\r\n") == 0 || user.authIrssi.size() != 0)
    {
        if (this->irssiAuthenticate(users, user, message) == true)
        {
            user.isAuthenticated = true;
            std::string nickMsg = ":server NICK " + user.nickname + "\r\n";
            std::string welcomeMsg = "001 " + user.nickname + " :Welcome to the Internet Relay Network " + user.nickname + "\r\n";
            send(this->pollfds[index].fd, nickMsg.c_str(), nickMsg.length(), 0);
            send(this->pollfds[index].fd,welcomeMsg.c_str(),welcomeMsg.length(),0);
            user.authIrssi.clear();
        }
        else if (user.authIrssi.size() == 4 && user.isAuthenticated == false)
        {

            std::string quitMsg = "ERROR :Closing Link: localhost (Server disconnected)\r\n";
            send(this->pollfds[index].fd,quitMsg.c_str(),quitMsg.length(),0);
            this->disconnectClient(users,index);
            user.authIrssi.clear();
        }
        return ;
    }
    else 
    {
        if (netcatAuthenticate(users, user, message) == false)
        {
            // std::string failMsg = "Something went during authentication (wrong password, invalid nickname, etc...)\r\n";
            // send(this->pollfds[index].fd, failMsg.c_str(), failMsg.length(), 0);
            return ;
        }
    }
}

bool Server::netcatAuthenticate(std::vector<User> &users, User &user, std::string message)
{
    if (message.length() <= 7 || checkInvalidCharacters(message) == false)
    {
        user.isPassAuthenticated = false;
        user.isNickAuthenticated = false;
        user.isUserAuthenticated = false;
        return false;
    }
    message = trim (message);
    std::string parameter = message.substr(0,5);
    std::string messageParsed = message.substr(5);
    // std::cout << parameter << "|" <<std::endl;
    // std::cout << messageParsed << "|" << std::endl;
    if (user.isPassAuthenticated == false)
    {
            // if (message.substr(0,5).compare("PASS ") == 0)
            // {
            //     if (a.substr(5,message.length() - 5 - 2).compare(this->password) == 0)
            //     {
            //         std::cout << "CORRECT PASSWORD\n" << std::endl;
            //         user.isPassAuthenticated = true;
            //         return (true);
            //     }
            //     else
            //         return false;
            // }
            // return (false);
        } 
    return (false);
}


bool Server::irssiAuthenticate(std::vector<User> &users, User &user, std::string message)
{
    int i = 0;
    std::string str;

    while (message.empty() == false)
    {
        str = message.substr(0,message.find("\n") + 1);
        str.erase(str.length() - 2);
        user.authIrssi.push_back(str);
        message = message.substr(message.find("\n") + 1);
        if (checkLastIrssi(message) == true)
        {
            message.erase(message.length() - 2);
            user.authIrssi.push_back(message);
            break;
        }
    }
    if (user.authIrssi.size() != 4)
    {
            std::cout << "not enough\n";
            return (false);
    }
    if (user.authIrssi[1].substr(5).compare(this->password) != 0)
    {
            std::cout << "zrong password\n";
            return (false);

    }
    user.nickname = parseNickname(users,user.authIrssi[2].substr(5));
    if (user.nickname.compare("ThisNicknameIsNotValid") == 0)
            return (false);
    return (true);
}

void Server::checkPrivateMessage(std::vector<User> users, User user, std::string message)
{
    std::cout << message;
}

//---------------------------------------HELPERS---------------------------------------//

void Server::initializePollFds()
{
    int i = 0;
    while (i < this->maxSize)
    {
        this->pollfds[i].fd = 0;
        this->pollfds[i].revents = 0;
        i++;
    }
    this->pollfds[0].fd = this->serverSocket;
    this->pollfds[0].events = POLLIN | POLLPRI;
}

int Server::getIndexofUser(std::vector<User> users, int index)
{
    int i = 0;
    while (i < this->maxSize)
    {
        if(index == users[i].clienNumber)
        {
            break;
        }
        i++;
    }
    return (i);
}

std::string Server::parseNickname(std::vector<User> users, std::string nickname)
{
    int i = 0;
    
    while (i < this->clientAmount)
    {
        if (users[i].nickname.compare(nickname) == 0)
        {
            nickname.push_back('_');
            i = -1;
        }
        i++;
    }
    if (nickname.length() > 20 || isalpha(nickname[0]) == false)
        return ("ThisNicknameIsNotValid");
    i = 0;
    while (i < nickname.length())
    {
        if (isalnum(nickname[i]) == false && nickname[i] != '-' && nickname[i] != '_')
            return ("ThisNicknameIsNotValid");
        i++;
    }
    return (nickname);
}

//-------------------------------------HELPERS END-------------------------------------//



//-------------------------------------EXCEPTIONS--------------------------------------//

const char* Server::ServerSocketFail::what() const throw()
{
    return ("Error: Something went wrong during the initialization of the server socket.\n");
}

const char* Server::PortOutofRange::what() const throw()
{
    return ("Error: Port has to be between 1030 and 6000.\n");
}

//-----------------------------------EXCEPTIONS END------------------------------------//




bool Server::validateIncomingMessage (std::vector<User> &users, std::string &message, int index)
{
    int msgSize;
    message.clear();
    message.resize(512);
    msgSize = recv(this->pollfds[index].fd, &message[0], message.size(),0);
    if (msgSize == -1 || msgSize == 0)
    {
        this->disconnectClient(users, index);
        return (false);
    }
    message.resize(msgSize);
    std::cout << message;
    if (msgSize > 512 || msgSize < 2 || message[message.length()-2] != '\r' || message[message.length()-1] != '\n')
            return (false);
    return (true);
}


bool Server::checkInvalidCharacters (std::string message)
{
    int i = 0;
    char c;
    while (i < message.length() - 2)
    {
        c = message[i];
        if (c == '\0' || c == '\r' || c == '\n') 
            return false;
        i++;
    }
    return (true);
    // if (!isalnum(c) && !isValidSpecialChar(c) && c != ':' && c != '#' && c != '&' && c != '-' && c != '.') 
    //     return false;
}