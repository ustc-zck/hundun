#include "src/db.h"

HundunDB* hundun_db;

std::string handler(char* buf){
    //set handler...
    std::string ret = hundun_db->Handler(buf);
    //std::cout << "ret is " << ret << std::endl;
    return ret;
}

std::string echo(char* buf){
    //set handler...
    //std::string ret = hundun_db->Handler(buf);
    std::cout << "ret is " << buf << std::endl;
    return buf;
}


//g++ -o main main.cpp src/* lib/lei/src/* -lrocksdb -lpthread -ldl
int main(){ 
    hundun_db = new HundunDB();
    Server* s = new Server(9000);
    s->Handler = &echo;
    s->Run();
    return 0;
}

