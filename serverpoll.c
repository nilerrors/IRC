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
#include <string.h>


#define PORT 6667
#define SIZE 1024
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


int creat_socket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int optval = 1;
    int optLen = sizeof(optval);
    int sockoptions = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, optLen);
    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1)
    {
        perror("bindResult");
    }

    int listenResult = listen(server_socket, 5);
    if (listenResult == -1)
    {
        perror("listenResult");
    }
    printf("server start\n");
    return server_socket;
}

int wait_client(int server_socket)
{
    struct pollfd pollfds[MAX_CLIENTS + 1];
    int i = 1;
    while (i < MAX_CLIENTS)
    {
        pollfds[i].fd = 0;
        pollfds[i].revents = 0;
        i++;
    }
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN | POLLPRI;
    int useClient = 0;

    while (1)
    {

        printf("useClient => %d\n", useClient);
        int pollResult = poll(pollfds, useClient + 1, 5000);

        if (pollResult > 0)
        {
            if (pollfds[0].revents & POLLIN)
            {
                struct sockaddr_in cliaddr;
                int addrlen = sizeof(cliaddr);
                int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
                printf("accept success %s\n", inet_ntoa(cliaddr.sin_addr));
                for (int i = 1; i < MAX_CLIENTS; i++)
                {
                    if (pollfds[i].fd == 0)
                    {

                        pollfds[i].fd = client_socket;
                        pollfds[i].events = POLLIN | POLLPRI;
                        useClient++;
                        break;
                    }
                }
            }
            for (int i = 1; i < MAX_CLIENTS; i++)
            {
                if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
                {
                    char buf[SIZE];
                    int bufSize = recv(pollfds[i].fd, buf, SIZE - 1,0);
                    if (bufSize == -1 || bufSize == 0)
                    {
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;
                    }
                    else
                    {
                        buf[bufSize] = '\0';
                        printf("From client: %s\n", buf);
                        if (strncmp(buf, "CAP LS",4 ) == 0)
                        {
                            send(pollfds[i].fd,"001 zengi :Welcome to the Internet Relay Network zengi\n",56,0);
                            // gettimeofday(&start_time,NULL);
                            memset(buf,0,1024);
                        }
                        if (strncmp(buf, "PING",4 ) == 0)
                        {
                            printf("send pong command\n");
                            send(pollfds[i].fd,"PONG 127.0.0.1\r\n",17,0);
                            memset(buf,0,1024);
                        }

                        if (strncmp(buf, "JOIN", 4) == 0)
                        {

                        }
                    }
                }
            }
        }
    }
}

int main()
{
    int server_socket = creat_socket();

    int client_socket = wait_client(server_socket);

    printf("server end\n");

    close(client_socket);
    close(server_socket);

    return 0;
}



void runServer(Server &server)
{
    std::vector<User> x;
    struct pollfd pollfds[MAX_CLIENTS + 1];
    int useClient = 0;
    int i = 1;
    initPollfds(pollfds);
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN | POLLPRI;

    while (1)
    {
        printf("useClient => %d\n", useClient);
        int pollResult = poll(pollfds, useClient + 1, 5000);

        if (pollResult > 0)
        {
            if (pollfds[0].revents & POLLIN) // If not connected it will jump in here
            {
                struct sockaddr_in cliaddr;
                int addrlen = sizeof(cliaddr);
                int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
                printf("accept success %s\n", inet_ntoa(cliaddr.sin_addr));
                for (int i = 1; i < MAX_CLIENTS; i++)
                {
                    if (pollfds[i].fd == 0)
                    {
                        pollfds[i].fd = client_socket;
                        pollfds[i].events = POLLIN | POLLPRI;
                        useClient++;
                        User a = User();
                        a.clienNumber = i;
                        x.push_back(a);
                        break;
                    }
                }

            }
            
            
            
            else 
            {
                for (int i = 1; i < MAX_CLIENTS; i++)
                {
                    if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
                    {
                        char buf[510 + 2];
                        int bufSize = recv(pollfds[i].fd, buf, SIZE - 1,0);
                        buf[bufSize] = '\0';
                        std::cout << buf << std::endl;
                        if (bufSize == -1 || bufSize == 0)
                        {
                            pollfds[i].fd = 0;
                            pollfds[i].events = 0;
                            pollfds[i].revents = 0;
                            useClient--;
                            x.erase(x.begin() + getIndexofUser(x,i));
                            continue;
                        }
                        int index = getIndexofUser(x,i);
                        // if (checkValidCommand(bufSize) == false)
                        //         continue;
                        // // if (checkEndCommand(buf,bufSize,x[index]) == false)
                        // //         continue;

                        if (x[index].isAuthenticated == false)
                        {
                            std::cout << "this client is still not authenticated." <<  std::endl;
                            ft_authenticate(server_socket,x[index],buf,bufSize,x);
                            if (x[index].isAuthenticated == true)
                            {
                                send(pollfds[i].fd,"001 zengi :Welcome to the Internet Relay Network zengi\n",56,0);
                            }
                        }
                    }
                }
            }

        }
    }
}
