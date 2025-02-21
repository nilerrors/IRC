#include <string>
#include <cctype>

class IRCValidator {
private:
    static const size_t MAX_LENGTH = 512;  // Including CR-LF
    
    bool isValidSpecialChar(char c) const {
        return c == '[' || c == ']' || c == '\\' || 
               c == '`' || c == '^' || c == '_' || 
               c == '|' || c == '{' || c == '}';
    }

    bool isValidNickChar(char c) const 
    {
        return isalnum(c) || c == '-' || isValidSpecialChar(c);
    }

public:
    bool isValidCommand(const std::string& cmd) const 
    {
        //Implemented
        if (cmd.empty() || cmd.length() > MAX_LENGTH) 
            return false;
        //Implemented End

        //Implemented
        // Check CR-LF ending
        if (cmd.length() < 2 || cmd[cmd.length()-2] != '\r' || cmd[cmd.length()-1] != '\n') 
            return false;
        //Implemented End

        //NOT SURE IF NEEDED
        // First character must be letter or special command char
        if (!isalpha(cmd[0]) && cmd[0] != '/' && cmd[0] != ':') 
            return false;

        // Check for invalid characters
        for (size_t i = 0; i < cmd.length()-2; ++i) 
        {  
            // -2 to skip CR-LF
            char c = cmd[i];
            
            // NULL, CR, LF not allowed except at end
            if (c == '\0' || c == '\r' || c == '\n') 
                return false;
            
            //Implemented
            // Space is allowed
            if (c == ' ') 
                continue;
            //Implemented End

            // Only allow valid characters
            if (!isalnum(c) && !isValidSpecialChar(c) && c != ':' && c != '#' && c != '&' && c != '-' && c != '.') 
                return false;
        }

        return true;
    }

    bool isValidNickname(const std::string& nick) const 
    {
        if (nick.empty() || nick.length() > 9) 
            return false;

        // Must start with letter
        if (!isalpha(nick[0])) {
            return false;
        }

        // Check remaining characters
        for (size_t i = 1; i < nick.length(); ++i) {
            if (!isValidNickChar(nick[i])) {
                return false;
            }
        }

        return true;
    }

    bool isValidChannel(const std::string& channel) const {
        if (channel.empty() || channel.length() > MAX_LENGTH) {
            return false;
        }

        // Must start with # or &
        if (channel[0] != '#' && channel[0] != '&') {
            return false;
        }

        // Check for invalid characters
        for (size_t i = 1; i < channel.length(); ++i) {
            char c = channel[i];
            if (c == ' ' || c == '\a' || c == ',' || 
                c == '\0' || c == '\r' || c == '\n') {
                return false;
            }
        }

        return true;
    }
};


int main() {
    IRCValidator validator;
    
    // Test command validation
    std::string cmd = "PRIVMSG #channel :Hello\r\n";
    if (validator.isValidCommand(cmd)) {
        // Command is valid
    }
    
    // Test nickname validation
    std::string nick = "User123";
    if (validator.isValidNickname(nick)) {
        // Nickname is valid
    }
    
    // Test channel validation
    std::string channel = "#test";
    if (validator.isValidChannel(channel)) {
        // Channel is valid
    }
    
    return 0;
}