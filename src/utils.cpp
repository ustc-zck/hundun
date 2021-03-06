#include <iostream>
#include <vector>
#include "utils.h"


// std::string GetPrefixFromKey(std::string key){
//     int pos_first = key.find("&&");
//     return key.substr(0, pos_first);
// }

// int64_t GetTimeFromKey(std::string key){
//     int pos_first = key.find("&&");
//     int pos_last = key.find_last_of("&&");
//     std::string time = key.substr(pos_first + 2, pos_last - pos_first);
//     return std::stol(time);
// }

//TODO, parse wal log...now only support set operation...
std::vector<std::string> GetKvPair(std::string data, int data_size){
    auto s = data.substr(14, data_size);
    int pos = s.find_first_of("&&");
    std::string key = s.substr(0, pos);
    std::string val = s.substr(pos + 2);
    val.erase(val.find_last_not_of(' ')+1); 
    val.erase(0, val.find_first_not_of(' '));
    std::vector<std::string> ret;
    ret.push_back(key);
    ret.push_back(val);
    return ret;
}

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

int64_t GetTimeSeconds(){
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

int64_t GetTimeMillSeconds(){
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

//return local ipv4 address...
std::string GetLocalIp() {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    if (ifAddrStruct!=NULL)
        freeifaddrs(ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        std::string ifa_name(ifa->ifa_name);
        // check it is IP4
        if (ifa->ifa_addr->sa_family == AF_INET && ifa_name != "lo") { 
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            std::string ret(addressBuffer); 
            return ret;
        }
    }
    return NULL;
}