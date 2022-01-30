#include "../src/parse.h"
#include <iostream>
#include <string> 

int main(){
    //test get...
    //std::string s = "*2\r\n$3\r\nget\r\n$1\r\na\r\n";
    //test set...
    //std::string s = "*3\r\n$3\r\nset\r\n$1\r\na\r\n$1\r\n1\r\n";
    //test mget, keys are stored in vector keys and args...
    //std::string s = "*5\r\n$4\r\nmget\r\n$1\r\na\r\n$1\r\nb\r\n\r\n$1\r\nc\r\n$1\r\nd\r\n";

    //test mset, append vector args into vector keys...
    //std::string s = "*7\r\n$4\r\nmset\r\n$1\r\na\r\n$1\r\n1\r\n$1\r\nb\r\n$1\r\n2\r\n$1\r\nc\r\n$1\r\n3\r\n";

    //test hmget...
    //std::string s = "*5\r\n$5\r\nHMGET\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n$6\r\nfield2\r\n$7\r\nnofield\r\n";

    //test hmset...
    //std::string s = "*6\r\n$5\r\nHMSET\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n$5\r\nHello\r\n$6\r\nfield2\r\n$5\r\nWorld\r\n";

    //test multi keys...
    std::string s = "*6\r\n$5\r\nHMSET\r\n$6\r\nmyhash\r\n$6\r\nfield1\r\n$5\r\nHello\r\n$6\r\nfield2\r\n$5\r\nWorld\r\n *3\r\n$3\r\nset\r\n$1\r\na\r\n$1\r\n1\r\n";

    Parser* p = new Parser(s);
    
    p->Parse();

    std::cout << "print cmd:" << std:: endl;
    for(int i = 0; i < p->Cmds().size(); i++){
        std::cout << p->Cmds()[i] << std::endl;
    }

    std::cout << "print keys" << std::endl;
    for(int i = 0; i < p->Keys().size(); i++){
        std::cout << p->Keys()[i] << std::endl;
    }

    std::cout << "print args" << std::endl;
    for(int i = 0; i < p->Args().size(); i++){
        for(int j = 0; j < p->Args()[i].size(); j++){
            std::cout << p->Args()[i][j] << std::endl;
        }
    }
    return 0;
}