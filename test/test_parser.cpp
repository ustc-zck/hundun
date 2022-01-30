#include <algorithm>
#include <string>
#include <iostream>

int main(){
    std::string s = "hello world";
    auto ret = std::find(s.begin(), s.end(), 'o');
    std::cout << ret - s.begin() << std::endl;
    return 0;
}