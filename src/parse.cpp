#include "parse.h"
#include <algorithm>
#include <iostream>

//*2\r\n$3\r\nget\r\n$1\r\na\r\n

//TODO, support pipeline...
void Parser::Parse(){
    std::vector<std::string> cmds_;
    std::vector<std::string> keys_;
    std::vector<std::vector<std::string>> args_;
    std::vector<int> pos;
    for(int i = 0; i < buf.length(); i++){
        if (buf[i] == '*'){
            pos.push_back(i);
        }
    }

    for(int i = 0; i < pos.size(); i ++){
        std::string temp;
        if(i + 1 == pos.size()){
            temp = buf.substr(pos[i]);
            std::cout << temp << std::endl;
        }else{
            temp = buf.substr(pos[i], pos[i+1]-pos[i]);
        }
        int j = 0; 
        int count = 0;
        std::vector<std::string> args;
        while(j < temp.size()){
            if(temp[j] == '$'){
                count++;
                for(int k = j; k < temp.size() - 1; k++){
                    //*2\r\n$3\r\nget\r\n$1\r\na\r\n
                    if(temp.substr(k, 2) == "\r\n" && count == 1){ 
                        int str_len = stoi(temp.substr(j + 1, k - j -1));
                        //std::cout << "j is " << j << std::endl;
                        //std::cout << "str len is " << str_len << std::endl;
                        std::string cmd = temp.substr(k + 2, str_len);
                        //std::cout << j + str_len + 3 << std::endl;
                        std::cout << "cmd is " << cmd << std::endl;
                        cmds_.push_back(cmd);
                        break;
                    }else if(temp.substr(k, 2) == "\r\n" && count == 2){ 
                        int str_len = stoi(temp.substr(j + 1, k - j -1));
                        std::string key = temp.substr(k + 2, str_len);
                        keys_.push_back(key);
                        break;
                    }else if (temp.substr(k, 2) == "\r\n" && count > 2){
                        int str_len = stoi(temp.substr(j + 1, k - j -1));
                        std::string arg = temp.substr(k + 2, str_len);
                        args.push_back(arg);
                        break;
                    }else{
                        //TODO
                    }
                }
            }
            j++;
        }
        args_.push_back(args);
    }

    this->cmds = cmds_;
    this->keys = keys_;
    this->args = args_;
    return;
}