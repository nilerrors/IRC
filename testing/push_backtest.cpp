#include <string>
#include <iostream>
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
#include <vector>


class User
{
    private:

    public:
    User (void);
    std::vector<std::string> authIrssi;
};

User::User(void)
{
}


void irssiAuthenticate(User &user)
{
    int i = 0;
    std::string str;
    user.authIrssi.push_back("hello?");
    return ;

}

void m(std::vector<User> &users)
{
    User a = User();
    users.push_back(a);
}

int main (void)
{
    std::vector<User> users;
    m(users);
    irssiAuthenticate(users[0]);
    std::cout << "OK?" << std::endl;
}