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
    std::string quitMsg = "ERROR :Closing Link: localhost (Server disconnected)\r\n";
    send(this->pollfds[index].fd,quitMsg.c_str(),quitMsg.length(),0);
    close(this->pollfds[index].fd);
    this->pollfds[index].fd = 0;
    this->pollfds[index].events = 0;
    this->pollfds[index].revents = 0;
    users.erase(users.begin() + this->getIndexofUser(users,index));
}

void Server::checkPrivateMessage(std::vector<User> users, User user, std::string message)
{
    std::cout << message;
}

std::string Server::getPassword()
{
    return this->password;
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

int Server::getCurrentUsersToPoll() 
{
    int i = 1;
    int j = 1;
        while (j < this->maxSize)
        {
            if (this->pollfds[j].fd != 0)
            {
                    i = j;
            }
            j++;
        }
        return i;
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

Server::~Server()
{
}