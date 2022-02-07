// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include<cstdio>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"


std::string kDBPath = "data/";

using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;

using namespace ROCKSDB_NAMESPACE;

int main() {
  // open DB
  Options options;
  options.create_if_missing = true;
  DB* db;
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // create column family
  ColumnFamilyHandle* cf;
  s = db->CreateColumnFamily(ColumnFamilyOptions(), "new_cf", &cf);
  assert(s.ok());

  // close DB
  s = db->DestroyColumnFamilyHandle(cf);
  assert(s.ok());
  delete db;

  // open DB with two column families
  std::vector<ColumnFamilyDescriptor> column_families;
  // have to open default column family
  column_families.push_back(ColumnFamilyDescriptor(
      ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, ColumnFamilyOptions()));
  // open the new one, too
  column_families.push_back(ColumnFamilyDescriptor(
      "new_cf", ColumnFamilyOptions()));
  std::vector<ColumnFamilyHandle*> handles;
  s = DB::Open(DBOptions(), kDBPath, column_families, &handles, &db);
  assert(s.ok());

  // put and get from non-default column family
  s = db->Put(WriteOptions(), handles[1], Slice("key"), Slice("value"));
  assert(s.ok());
  std::string value;
  s = db->Get(ReadOptions(), handles[1], Slice("key"), &value);
  assert(s.ok());

  // atomic write
  WriteBatch batch;
  batch.Put(handles[0], Slice("key2"), Slice("value2"));
  batch.Put(handles[1], Slice("key3"), Slice("value3"));
  batch.Delete(handles[0], Slice("key"));
  s = db->Write(WriteOptions(), &batch);
  assert(s.ok());

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
    std::string data = result.writeBatchPtr->Data();
    printf(data.c_str());
    std::cout << std::endl;
    iter->Next();
    count++;
  }

  // drop column family
  s = db->DropColumnFamily(handles[1]);
  assert(s.ok());

  // close db
  for (auto handle : handles) {
    s = db->DestroyColumnFamilyHandle(handle);
    assert(s.ok());
  }
  delete db;

  return 0;
}