#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <vector>


int64_t GetTimeSeconds();

int64_t GetTimeMillSeconds();

std::string GetPrefixFromKey(std::string key);

int64_t GetTimeFromKey(std::string key);

std::vector<std::string> GetKvPair(std::string data, int data_size);
