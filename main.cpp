#include "headers/User.hpp"
#include "headers/Server.hpp"
#include "headers/Utility.hpp"
#include "headers/Authenticate.hpp"
#include <csignal>

static int gotSignal = 0;

void handler(int sig)
{
    gotSignal = 1;
} 


void runServer(Server &server)
{
    std::vector<User> users;
    std::string message;
    int msgSize;
    int i;
    int usersToPoll;
    
    while (gotSignal == 0)
    {
        usersToPoll = server.getCurrentUsersToPoll() + 1;
        int pollResult = poll(server.pollfds,usersToPoll, 5000);
        if (pollResult > 0)
        {
            i = 1;
            if (server.pollfds[0].revents & POLLIN) // If not connected it will jump in here
                    server.connectToServer(users);
            else 
            {
                while (i < server.maxSize)
                {
                    if (server.pollfds[i].fd > 0 && server.pollfds[i].revents & POLLIN)
                    {
                        if (server.validateIncomingMessage(users,message,i) == false)
                                continue;
                        
                        if (message.compare("PING 127.0.0.1\r\n") == 0)
                                send(server.pollfds[i].fd,"PONG 127.0.0.1\r\n",16,0);

                        else if (users[server.getIndexofUser(users,i)].isUserAuthenticated == false)
                                authenticateMain(server, users, users[server.getIndexofUser(users,i)], message, i);
                        // else 
                        //     server.checkPrivateMessage(users,users[server.getIndexofUser(users,i)],message);
                    }
                    i++;
                }
            }
        }
    }
    users.clear();
}

int main(int argc, char *argv[])
{
    struct sigaction sa;  
    sa.sa_handler = handler;  
    sigemptyset(&sa.sa_mask);  
    sa.sa_flags = 0;  
    sigaction(SIGINT, &sa, NULL);  

    // if (argc !=  3)
    // {
    //     std::cout << "2 arguments needed" << std::endl;
    //     return (0);
    // }
    // if (parseUtil(argv) == false)
    // {
    //     std::cout << "Port and/or password are not formatted properly" << std::endl;
    //     return (0);
    // }
    Server server = Server();
    try 
    {
        server.initializeSocket("6667", "   abc123   ");
    }
    catch (Server::ServerSocketFail &ex) 
    {
        std::cerr << "Server error: " << ex.what() << std::endl;
        return 1;
    }
    catch (Server::PortOutofRange &ex)
    {
        std::cout << ex.what();
        return 1;
    }
    runServer(server);
    std::cout << "server end\n";
    close(server.serverSocket);
    return 0;
}