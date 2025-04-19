#ifndef AUTHENTICATE_H
# define AUTHENTICATE_H

#include "../headers/User.hpp"
#include "../headers/Server.hpp"
#include "../headers/Utility.hpp"
#include <vector>
#include <iostream>

void authenticateMain(Server &server, std::vector<User> &users, User &user, std::string message, int index);

#endif