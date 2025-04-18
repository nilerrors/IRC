#ifndef USER_H
# define USER_H
#include "Channel.hpp"

#include <string>
#include <vector>

class User
{
    private:

    public:
    User (void);
    std::string nickname;
    std::vector<std::string> auth;
    bool isUserAuthenticated;
    bool isAuthFailed;
    int clienNumber;
};

#endif