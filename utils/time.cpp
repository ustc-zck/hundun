#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

using std::cout; using std::endl;
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
