#ifndef UTILITY_H
# define UTILITY_H
#include <string>
#include <sstream>

std::string trim(std::string str); 
bool checkAlNum(std::string str);
bool detectInt(const char* stringLiteral);
bool toInt(std::string str, int *val);
bool parseArgs(char *argv[]);
bool checkLastIrssi (std::string input);
bool checkValidCommand (std::string input);



// class Utility
// {
//     private:

//     public:
//     std::string trim(std::string str); 
//     bool checkAlNum(std::string str);
//     bool detectInt(const char* stringLiteral);
//     bool toInt(std::string str, int *val);
//     bool parseArgs(char *argv[]);



// };

#endif