#include <iostream>
#include <string>

int main(){
    std::string addr = "127.0.0.1:8080";
    std::string ip = addr.substr(0, addr.find(":"));
    int port = std::stoi(addr.substr(addr.find(":") + 1)); 
    std::cout << "ip is " << ip << std::endl;
    std::cout << "port is " << port << std::endl;
    return 0;
}