#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <poll.h>
# include <time.h>
# include <sys/time.h>
#include <vector>
#include "headers/User.hpp"
#include "headers/Server.hpp"
#include "headers/Utility.hpp"
#define MAX_CLIENTS 10

long int	ft_time(struct timeval st, struct timeval ct)
{
	long int	a;
	long int	b;

	gettimeofday(&ct, NULL);
	a = (ct.tv_sec * 1000) + (ct.tv_usec / 1000);
	b = (st.tv_sec * 1000) + (st.tv_usec / 1000);
	return (a - b);
}

long int	actual_time(void)
{
	long int			time;
	struct timeval		current_time;

	time = 0;
	gettimeofday(&current_time, NULL);
	time = (current_time.tv_sec * 1000) + (current_time.tv_usec / 1000);
	return (time);
}

void	ft_usleep(long int time_in_ms)
{
	long int	start_time;

	start_time = 0;
	start_time = actual_time();
	while ((actual_time() - start_time) < time_in_ms)
		usleep(time_in_ms / 10);
}

bool checkLast (std::string input)
{
    int i = 0;
    while (i < input.length())
    {
        if (input[i] == ' ')
        {
            return false;
        }
        i++;
    }
    return true;
}

bool parseUser (std::string input)
{
    std::vector<std::string> a;

    if (input.empty() == true)
            return (false);
    
    int i = 0;
    input = trim (input);
    input = input.substr(5);
    while (input.empty() == false)
    {
        a.push_back(input.substr(0,input.find(" ") + 1));
        input = input.substr(input.find(" ") + 1);
        input = trim (input);
        if (checkLast(input) == true)
        {
            a.push_back(input);
            break;
        }
        i++;
    }
    if (a.size() != 4)
        return (false);
    i = 0;
    if (a[3][0] != ':')
            return (false);
    return (true);
}


// bool ft_authenticate(int server_socket, User &user,char *buf,int bufSize, std::vector<User> u)
// {
//     std::string a = buf;

//         if (user.isPassAuthenticated == false)
//         {
//             if (a.substr(0,5).compare("PASS ") == 0)
//             {
//                 if (a.substr(5,bufSize - 5 - 2).compare("abc123") == 0)
//                 {
//                     std::cout << "CORRECT PASSWORD\n" << std::endl;
//                     user.isPassAuthenticated = true;
//                     return (true);
//                 }
//                 else
//                     return false;
//             }
//             return (false);
//         }   
//         if (user.isNickAuthenticated == false)
//         {
//             if (a.substr(0,5).compare("NICK ") == 0)
//             {
//                 user.nickname = parseNickname(u,a.substr(5,bufSize - 5 - 2));
//                 user.isNickAuthenticated = true;
//                 return (true);
//             }
//             else 
//             {
//                     user.isPassAuthenticated = false;
//                     return false;
//             }
//         }
//         if (user.isUserAuthenticated == false)
//         {
//             if (a.substr(0,5).compare("USER ") == 0)
//             {
//                 if (parseUser(a) == false)
//                 {
//                     user.isPassAuthenticated = false;
//                     user.isNickAuthenticated = false;
//                     return (false);
//                 }
//             }
//         }   
//     }
    
//     user.isAuthenticated = true;
//     return (true);
// }

int getIndexofUser(std::vector<User> users, int index)
{
    int i = 0;
    while (i < MAX_CLIENTS)
    {
        if(index == users[i].clienNumber)
        {
            break;
        }
        i++;
    }
    return (i);
}







void runServer(Server &server)
{
    std::vector<User> users;
    std::string message;
    int msgSize;
    int i;
    while (1)
    {
        // std::cout << "useClient => " << server.clientAmount << std::endl; 
        // int pollResult = poll(server.pollfds, server.clientAmount + 1, 5000);
        int z = 1;
        int k = 1;
        while (k < server.maxSize)
        {
            if (server.pollfds[k].fd != 0)
            {
                    z = k;
            }
            k++;
        }
        std::cout << z << std::endl;
        int pollResult = poll(server.pollfds,z+1, 5000);

        if (pollResult > 0)
        {
            // std::cout << "polling....\n";
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
                        if (users[server.getIndexofUser(users,i)].isAuthenticated == false)
                                server.authenticate(users, users[server.getIndexofUser(users,i)], message, i);
                        // else 
                        //     server.checkPrivateMessage(users,users[server.getIndexofUser(users,i)],message);
                    }
                    // if (server.pollfds[i].fd > 0)
                    // {
                    //         send(server.pollfds[i].fd,"PONG 127.0.0.1\r\n",17,0);
                    // }
                    i++;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
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
    Server server;
    // int server_socket = server.initializeSocket("6667");
    server.initializeSocket("6667", "   abc123   ");

    // int clien_socket = runServer(server_socket);
    runServer(server);

    printf("server end\n");
    // close(client_socket);
    close(server.serverSocket);

    return 0;
}



// if (strncmp(buf, "PING",4 ) == 0)
// {
//     printf("send pong command\n");
//     send(pollfds[i].fd,"PONG 127.0.0.1\r\n",17,0);
//     memset(buf,0,1024);
// }


                        // message.clear();
                        // message.resize(512);
                        // msgSize = recv(server.pollfds[i].fd, &message[0], message.size(),0);
                        // if (msgSize == -1 || msgSize == 0)
                        // {
                        //     server.disconnectClient(users, i);
                        //     continue;
                        // }
                        // message.resize(msgSize);
                        // if (msgSize > 512 || msgSize < 2 || message[message.length()-2] != '\r' || message[message.length()-1] != '\n')
                        //         continue;
