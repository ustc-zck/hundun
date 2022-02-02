#include "src/db.h"

HundunDB* hundun_db;

char* handler(char* buf){
    //set handler...
    return hundun_db->Handler(buf);
}

int main(){ 
    Server* s = new Server(9000);
    s->Handler = &handler;
    s->Run();
    return 0;
}