#include "../src/db.h"
#include <iostream>   

int main(){

    HundunDB* hundun_db = new HundunDB();

    if(hundun_db->Put("hello&&1234999999995&&", "world") < 0){
        std::cout << "failed to pus kv into db." << std::endl; 
        return -1;
    }

    auto val = hundun_db->Get("hello");
    std::cout << "val is " << val << std::endl;

    return 0;
}