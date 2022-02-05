#include <iostream>
#include <vector>

int64_t GetTimeFromKey(std::string key){
    int pos_first = key.find("&&");
    int pos_last = key.find_last_of("&&");
    std::string time = key.substr(pos_first + 2, pos_last - pos_first);
    return std::stol(time);
}

std::vector<std::string> GetKvPair(std::string data, int data_size){
    auto s = data.substr(14, data_size);
    int pos = s.find_last_of("&&");
    std::string key = s.substr(0, pos+1);
    std::string val = s.substr(pos + 2);
    val.erase(val.find_last_not_of(' ')+1); 
    val.erase(0, val.find_first_not_of(' '));
    std::vector<std::string> ret;
    ret.push_back(key);
    ret.push_back(val);
    return ret;
}