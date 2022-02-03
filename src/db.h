#include <iostream>
#include <string>
#include "../lib/lei/src/server.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

class HundunDB{
    public:
        HundunDB(std::string data_path = "data/");
        HundunDB(rocksdb::DB* db_);
        ~HundunDB();
        std::string Get(std::string key);
        int Put(std::string key, std::string value);
        char* Handler(char* buf);
        void Start();
    private:
        rocksdb::Status s;
        rocksdb::DB* db;
};
