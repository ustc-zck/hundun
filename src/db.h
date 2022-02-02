#include <iostream>
#include <string>
#include "../lib/lei/src/server.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

class HundunDB{
    public:
        HundunDB(int Port = 8080);
        HundunDB(rocksdb::DB* db_, int Port = 8080);
        ~HundunDB();
        std::string Get(std::string key);
        int Put(std::string key, std::string value);
        char* Handler(char* buf);
        void Start();
    private:
        rocksdb::Status s;
        rocksdb::DB* db;
};
