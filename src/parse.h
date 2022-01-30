#include <string>
#include <map>
#include <vector>

class Parser{
    public:
        Parser(std::string s){
            buf = s;
        }
        ~Parser(){
        }
        void Parse();
        std::vector<std::string> Cmds(){
            return cmds;
        }
        std::vector<std::string> Keys(){
            return keys;
        }
        std::vector<std::vector<std::string>> Args(){
            return args;
        }
    private:
        std::string buf;
        std::vector<std::string> cmds;
        std::vector<std::string> keys;
        std::vector<std::vector<std::string>> args;
};
