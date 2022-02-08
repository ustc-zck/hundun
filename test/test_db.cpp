#include "../src/db.h"
#include <iostream>   

int main(){
    HundunDB* hundun_db = new HundunDB();
    // if(hundun_db->Put("hello", "world") < 0){
    //     std::cout << "failed to put key and value" << std::endl;
    // }
    // std::string value = hundun_db->Get("hello");
    // std::cout << "value is " << value << std::endl;
    //*3\r\n$3\r\nset\r\n$1\r\nb\r\n$1\r\n2\r\n
    std::string s1 = hundun_db->Handler("*3\r\n$3\r\nset\r\n$1\r\nb\r\n$1\r\n2\r\n");
    std::cout << s1<< std::endl;

    return 0;
}

 