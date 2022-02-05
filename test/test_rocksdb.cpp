// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdio>
#include <string>
#include <iostream>
#include <unistd.h>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "../utils/utils.h"

using namespace ROCKSDB_NAMESPACE;

std::string kDBPath = "data/";

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
    assert(s.ok());

    std::string value;
    int i = 0;
    while(i < 1000){
      // Put key-value
      std::string key = "key";
      key += std::to_string(i);
      std::string value1 = "value";
      s = db->Put(WriteOptions(), key, value1);
      s = db->Delete(rocksdb::WriteOptions(), key);
      // get value
      i++;
    }
    
    std::unique_ptr<TransactionLogIterator> iter;

    //s = db->GetUpdatesSince(db->GetLatestSequenceNumber() - 1000000, &iter);
    s = db->GetUpdatesSince(db->GetLatestSequenceNumber() - 2000, &iter);
    if(!s.ok()){
      std::cout << "failed to get update since seq number" << std::endl;
      return -1;
    }
    
    int count = 0;
    while(iter->Valid()){
      auto result = iter->GetBatch();
      std::cout << "seq num is " << result.sequence << std::endl;
      auto data = result.writeBatchPtr->Data();
      std::cout << (data[12]) << std::endl;
      int data_size = result.writeBatchPtr->GetDataSize();
      std::cout << "data size is " << data_size << std::endl;
      std::cout << "data is " << data << std::endl;
      iter->Next();
      count++;
    }

    std::cout << "count is " << count << std::endl;

    delete db;

    return 0;
  }