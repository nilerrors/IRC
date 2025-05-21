#include "../headers/Authenticate.hpp"

//---------------------------------------HELPERS---------------------------------------//

bool checkInvalidCharacters (std::string message)
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
    if (message[message.length() - 1] != '\n' || message[message.length() - 2] != '\r')
            return (false);
    return (true);
    // if (!isalnum(c) && !isValidSpecialChar(c) && c != ':' && c != '#' && c != '&' && c != '-' && c != '.') 
    //     return false;
}

std::string parseNickname(std::vector<User> users, std::string nickname)
{
    int i = 0;
    while (i < users.size())
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

std::string trimAuth(std::string str) 
{
    int b = 0;
    int e = str.length() - 3; 

    while (str[b] == '\t' || str[b] == '\f' || str[b] == '\v' || str[b] == ' ')
        b++;
    while (str[e] == '\t' || str[e] == '\f' || str[e] == '\v' || str[e] == ' ')
        e--;
    str = str.substr(b, e - b + 1) + "\r\n"; 
    return str;
}

//-------------------------------------HELPERS END-------------------------------------//

void authenticateCAPLS(User &user)
{
    std::string capls = trimAuth(user.auth[0]);
    if (user.auth.size() == 4)
    {
        if (capls.compare("CAP LS\r\n") == 0)
        {
            user.auth.erase(user.auth.begin());
            return ;
        }
        else
            user.isAuthFailed = true;
    }
}

void authenticatePassword(Server &server, User &user)
{
    std::string pass = user.auth[0];
    if (pass.substr(0,5).compare("PASS ") == 0)
    {
        pass = trim(pass.substr(5,pass.length() - 2));
        if (pass.compare(server.getPassword()) == 0)
                    return ;
    }
    user.isAuthFailed = true;
}

void authenticateNickname(std::vector<User> &users, User &user)
{
    std::string nickname = user.auth[1];
    if (nickname.substr(0,5).compare("NICK ") == 0)
    {
        nickname = trim(nickname.substr(5,nickname.length() - 2));
        user.nickname = parseNickname(users, nickname);
        if (user.nickname.compare("ThisNicknameIsNotValid") != 0)
            return;
    }
    user.isAuthFailed = true;
}

void authenticateUserInfo(std::vector<User> &users, User &user)
{
    std::vector<std::string> a;
    std::string userInfo = user.auth[2];
    int findIndex;
    int i = 0;

    if (userInfo.substr(0,5).compare("USER ") != 0)
    {
        user.isAuthFailed = true;
        return ;
    }
    userInfo = userInfo.substr(5);
    userInfo = userInfo.substr(0,userInfo.length() - 2); 
    while (i < 3)
    {
        findIndex = userInfo.find(" ");
        if (findIndex == std::string::npos)
        {
            a.push_back(userInfo);
            userInfo.clear();
        }
        a.push_back(userInfo.substr(0,findIndex + 1));
        userInfo = userInfo.substr(findIndex + 1);
        userInfo = trim (userInfo);
        i++;
    }
    if (a.size() != 3 || userInfo.empty() == true)
    {
        user.isAuthFailed = true;
        return ;
    }
    a.push_back(userInfo);
    if (a[3][0] != ':')
    {
        user.isAuthFailed = true;
        return ;
    }
    return ;
}

bool authenticate(Server &server, std::vector<User> &users, User &user, std::string message)
{
    std::string line;
    std::string parameter;
    std::string messageParsed;
    int i = 0;

    message = trimAuth(message);
    while (message.empty() == false)
    {
        
        user.auth.push_back(message.substr(0, message.find('\n') + 1));
        message = message.erase(0, message.find('\n') + 1);
    }
    if (user.auth.size() <= 2 || (user.auth.size() == 3 && user.auth[0].compare("CAP LS\r\n") == 0))
        return true;

    if (user.auth.size() > 4)
        return false;   
    while (i < user.auth.size())
    {
        if (checkInvalidCharacters(user.auth[i]) == false)
            return false;
        i++;
    }
    authenticateCAPLS(user);
    authenticatePassword(server,user);
    authenticateNickname(users, user);
    authenticateUserInfo(users, user);
    if (user.isAuthFailed == true)
            return false ;
    user.isUserAuthenticated = true;
    return true;
}

void authenticateMain(Server &server, std::vector<User> &users, User &user, std::string message, int index)
{
    std::string errMsg;
    if (authenticate(server, users, user, message) == false)
    {
        errMsg = "Authentication failed. Connect and try again.\n";
        send(server.pollfds[index].fd ,errMsg.c_str(),errMsg.length(),0);
        server.disconnectClient(users,index);
        return ;
    }

    if (user.isUserAuthenticated == true)
    {
        std::string nickMsg = ":server NICK " + user.nickname + "\r\n";
        std::string welcomeMsg = "001 " + user.nickname + " :Welcome to the Internet Relay Network " + user.nickname + "\r\n";
        send(server.pollfds[index].fd, nickMsg.c_str(), nickMsg.length(), 0);
        send(server.pollfds[index].fd,welcomeMsg.c_str(),welcomeMsg.length(),0);
        return ;
    }
}