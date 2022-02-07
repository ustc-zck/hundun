#include <iostream>
#include <string>
#include <vector>
#include "../lib/lei/src/server.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "utils.h"
#include "parse.h"
#include <hiredis.h>


class HundunDB{
    public:
        HundunDB(std::string data_path = "data/");
        HundunDB(rocksdb::DB* db_);
        ~HundunDB();
        std::string Get(std::string key);
        int Put(std::string key, std::string value);
        int DelPrefix(std::string prefix);
        std::string Handler(std::string buf);

        void SetMaster(){
            if(role == "master"){
                return;
            }
            role = "master";
            master_addr = "self";
            seq_num_of_slaves.clear();
        }

        void AddSlave(std::string slaveAddr){
            if(role == "master"){
                seq_num_of_slaves[slaveAddr] = 0;
                return;
            }
            return;
        }

        void SetSlave(std::string new_master_addr){
            role = "slave";
            master_addr = new_master_addr;
            seq_num_of_slaves.clear();
        }
        void Sync();

    private:
        rocksdb::Status s;
        rocksdb::DB* db;
        //"master" or "slave"...
        std::string role;
        //if master, master addr is self...
        std::string master_addr;
        //if slave, ignore this...
        //seq num...
        uint64_t seq_num;
        //seq num of each slave...
        std::unordered_map<std::string, uint64_t> seq_num_of_slaves; 
};
