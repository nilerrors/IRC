#include "../headers/Utility.hpp"


bool parseArgs(char *argv[])
{
    std::string portStr = argv[1];
    std::string password = argv[2];
    int port;


    portStr = trim(portStr);
    password = trim(password);
    if (password.empty() == true || password.length() > 10)
            return (false);
    if (checkAlNum(password) == false)
            return (false);
    if (detectInt(portStr.c_str()) == false)
            return (false);
    return (true);
}

std::string trim(std::string str) 
{
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
    return str;
}

bool checkAlNum(std::string str)
{
    int i = 0;

    while (i < str.length())
    {
        if (isalnum(str[i]) == false)
                return (false);
        i++;
    }
    return (true);
}

bool detectInt(const char* stringLiteral)
{
    std::string n;
    size_t i = 0;

    n = stringLiteral;
    if (n[i] == '+' || n[i]  == '-')
            i++;
    if (i == n.length())
            return (false);
    while (i < n.length()  && ((n[i] >= 48 && n[i] <= 57)))
        i++;
    if ( n.length() != i)
        return (false);
    return (true);
}

bool toInt(std::string str, int *val) 
{
    std::istringstream iss(str);
    double number;
    bool status = true;
    
    if (!(iss >> number)) 
    {
        return (false);
    }
    if (number < 1030 || number > 7000)
            return (false);
    *val =  static_cast<int>(number);
    return (true);
}

bool checkLastIrssi (std::string input)
{
    int i = 0;
    int newlineCount = 0;
    while (i < input.length())
    {
        if (input[i] == '\n')
                newlineCount++;
        i++;
    }
    if (newlineCount == 1)
        return true;
    else 
        return false;
}


bool checkValidCommand(std::string input)
{
    int i = 0;

    while (i < input.length())
    {
        if (input[i] == '\0' || input[i] == '\r' || input[i] == '\n') 
                return false;
        i++;
    }
    return true;
}

