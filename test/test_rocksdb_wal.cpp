#include <cstdio>
#include <string>
#include <iostream>
#include <unistd.h>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "../src/utils.h"
using namespace ROCKSDB_NAMESPACE;

std::string kDBPath = "../data/";

int main() {
    DB* db;
    Options options;
    // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present
    options.create_if_missing = true;

    // open DB
    Status s = DB::Open(options, kDBPath, &db);
    std::unique_ptr<rocksdb::TransactionLogIterator> log_iter;
    s = db->GetUpdatesSince(0, &log_iter);
    int count = 0;
    hundun_db->Handler("*3\r\n$3\r\nset\r\n$1\r\nb\r\n$1\r\n2\r\n");
    while(log_iter->Valid()){
        count++;
        log_iter->Next();
    }
    std::cout << "count is " << count << std::endl;
    assert(s.ok());
}