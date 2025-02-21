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
#include <iostream>
#include <ostream>
#include <vector>



// std::string trim(std::string str) 
// {
//     str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
//     str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
//     return str;
// }


std::string trim(std::string str) 
{
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
    return str;
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
int main (void)
{
    std::string input = "USER x x x :sqdqdq\r\n";
    std::vector<std::string> a;
    if (input.empty() == true)
            return (false);
    
    int i = 0;
    input = trim (input);
    input = input.substr(5);
    std::cout << input << std::endl;
    while (input.empty() == false)
    {
        a.push_back(input.substr(0,input.find(" ") + 1));
        input = input.substr(input.find(" ") + 1);
        input = trim (input);
        std::cout << input << std::endl;
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
    std::cout << "char is : '"<< a[3][0] << "'" << std::endl;

    return (true);
}


// class User
// {
//     private:

//     public:
//     User (void);
//     bool isAuthenticated;
//     bool isPassAuthenticated;
//     bool isNickAuthenticated;
//     bool isUserAuthenticated;
//     std::vector<std::string> authIrssi;
//     int clienNumber;
//     std::string username;
//     std::string nickname;
    

// };

// User::User(void)
// {
//     this->isAuthenticated = false;
//     this->isPassAuthenticated = false;
//     this->isNickAuthenticated = false;
//     this->isUserAuthenticated = false;
// }




// bool checkLastIrssi (std::string input)
// {
//     int i = 0;
//     int newlineCount = 0;
//     while (i < input.length())
//     {
//         if (input[i] == '\n')
//                 newlineCount++;
//         i++;
//     }
//     if (newlineCount == 1)
//         return true;
//     else 
//         return false;
// }


// int main (void)
// {
//     User user;
//     std::string input = "CAP LS\r\nPASS abc123\r\nNICK bc\r\nUSER zengi zengi 127.0.0.1 :zengi\r\n";
//     std::string str;

//     while (input.empty() == false)
//     {
//         str = input.substr(0,input.find("\n") + 1);
//         str.erase(str.length() - 2);
//         user.authIrssi.push_back(str);
//         input = input.substr(input.find("\n") + 1);
//         if (checkLastIrssi(input) == true)
//         {
//             input.erase(input.length() - 2);
//             user.authIrssi.push_back(input);
//             break;
//         }
//     }
//     if (user.authIrssi.size() != 4)
//             return (false);
//     return (true);
// }