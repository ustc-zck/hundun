#include "db.h"

HundunDB::HundunDB(std::string data_path, int Port){
    rocksdb::Options options;
    // Optimize RocksDB...
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present...
    options.create_if_missing = true;
    // open DB...
    s = rocksdb::DB::Open(options, data_path, &db);
    assert(s.ok());
    role = "slave";
    seq_num = db->GetLatestSequenceNumber();
    port = Port;
}

HundunDB::HundunDB(rocksdb::DB* db_, int Port){
    rocksdb::Options options;
    // Optimize RocksDB...
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present...
    options.create_if_missing = true;
    // assgin db...
    db = db_;
    role = "slave";
    seq_num = db->GetLatestSequenceNumber();
    port = Port;
}

HundunDB::~HundunDB(){
    redisFree(conn);
    freeReplyObject(reply);
    delete db;
}

std::string HundunDB::Get(std::string key){
    rocksdb::ReadOptions read_options;
    read_options.total_order_seek = false;
    read_options.prefix_same_as_start = true;
    rocksdb::Iterator* iter = db->NewIterator(read_options);
    key += "&&";
    iter->Seek(key);  
    std::string value;
    if(iter->Valid()){
        auto val = iter->value().ToString();
        return val;
    }
    // get value
    //s = db->Get(rocksdb::ReadOptions(), key, &value);
    // if(s.ok()){
    //     int pos = value.find("&&");
    //     return value.substr(0, pos);
    // }
    return "\0";
}

int HundunDB::Put(std::string key, std::string value){
    if(key.find("&&") == std::string::npos){
        if(this->DelPrefix(key) < 0){
            return -1;
        }
        key += "&&";
        key += std::to_string(GetTimeMillSeconds());
        key += "&&";
        s = db->Put(rocksdb::WriteOptions(), key, value);
        if(s.ok()){
            return 1;
        }
    } else{
        auto prefix = GetPrefixFromKey(key);
        auto time = GetTimeFromKey(key);
        rocksdb::ReadOptions read_options;
        read_options.total_order_seek = false;
        read_options.prefix_same_as_start = true;
        rocksdb::Iterator* iter = db->NewIterator(read_options);
        prefix += "&&";
        iter->Seek(prefix);  
        std::string value;
        if(iter->Valid()){
            auto key_ = iter->key().ToString();
            auto time_ = GetTimeFromKey(key_);
            if(time_ < time){
                rocksdb::WriteOptions write_options;
                write_options.disableWAL = true;
                db->Delete(write_options, iter->key());
            }else{
                return 0;
            }
            iter->Next();
        }
        s = db->Put(rocksdb::WriteOptions(), key, value);
        if(s.ok()){
            return 1;
        }
    }
    
    return -1;
}

int HundunDB::DelPrefix(std::string prefix){
    rocksdb::ReadOptions read_options;
    read_options.total_order_seek = false;
    read_options.prefix_same_as_start = true;
    rocksdb::Iterator* iter = db->NewIterator(read_options);
    prefix += "&&";
    iter->Seek(prefix);  
    std::string value;
    rocksdb::WriteOptions write_options;
    write_options.disableWAL = true;
    if(iter->Valid()){
        auto key = iter->key().ToString();
        s = db->Delete(write_options, key);
        if(!s.ok()){
            //std::cout << "failed to delete prefix" << std::endl;
            return -1;
        }
        //std::cout << "delete key: " << key << std::endl;
    }
    return 0;
}

std::string HundunDB::Handler(std::string buf){
    Parser* parser = new Parser(buf);
    parser->Parse();
    std::vector<std::string> cmds = parser->Cmds();
    std::vector<std::string> keys = parser->Keys();
    std::vector<std::vector<std::string>> args = parser->Args();

    std::string result = "";
    int j = 0, k = 0;
    for(int i = 0; i < cmds.size(); i++){
        std::string cmd = cmds[i];
        //transform string into lower case...
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return std::tolower(c);});
        std::cout << "lower case cmd is " << cmd << std::endl;
        if(cmd == "get"){
            std::string value = this->Get(keys[k]);
            if(value == "\0"){
                result += "$-1\r\n";
                continue;
            }
            std::string tmp;
            tmp += "$";
            //std::cout << "value length is " << value.length() << std::endl;
            tmp += std::to_string(value.length());
            tmp += "\r\n" + value + "\r\n";
            result += tmp;
            k++;
        } else if(cmd == "set"){
            if(keys[k] == "" || args[i].size() != 1){
                result += "-ERR syntax error\r\n";
                continue;
            }
            int ret = this->Put(keys[i], args[j][0]);
            if(ret < 0){
                result += "-ERR server put error\r\n";
                continue;
            }
            result += "+OK\r\n";
            k++;
            j++;
        }
        else if(cmd == "slaveof"){
            //set as master...
            if(keys[k] == "no" && args[j][0] == "one"){
                this->SetMaster();
                result += "+OK\r\n";
            } else if (keys[k] != "no" && args[j][0] != "one"){
                std::string new_master_addr = keys[k] + ":" + args[j][0];
                std::cout << "new master addr is " << new_master_addr << std::endl;
                //0.5 seconds...
                struct timeval timeout = {0, 500000}; 
                conn = redisConnectWithTimeout(keys[k].c_str(),  std::stoi(args[j][0]), timeout);
                if (conn == NULL || conn->err) {
                    if (conn) {
                        std::cout << "Connection error: " << conn->errstr << std::endl;
                    }
                    else {
                        std::cout << "Connection error: can't allocate redis context" << std::endl;
                    }
                    continue;
                }
                std::string local_ip = "127.0.0.1";
                std::string this_slave_addr = local_ip + ":" + std::to_string(port);
                std::cout << "this slave addr is " << this_slave_addr << std::endl;
                std::string cmd = "addslave " + this_slave_addr;
                void* ret = redisCommand(conn, &cmd[0]);
                reply = static_cast<redisReply*>(ret);
                std::string result_reply(reply->str);
                std::cout << "reply is " << result_reply << std::endl;
                std::cout << "reply is ok: " << (result_reply == "OK") << std::endl;
                if(result_reply == "OK"){
                    result += "+OK\r\n";
                    master_addr = new_master_addr;
                    std::cout << "master addr is " << master_addr << std::endl;
                } else {
                    result += "-ERR slave of error\r\n";
                }
            } else{
                result += "-ERR server put error\r\n";
            }
            k++;
            j++;
        }
        else if(cmd == "addslave"){
            this->AddSlave(keys[k]);
            k++;
            result += "+OK\r\n";
        }
        else if(cmd == "getseqnum"){
            seq_num = db->GetLatestSequenceNumber();
            std::string seq_num_str = std::to_string(seq_num);
            std::string tmp;
            tmp += "$";
            //std::cout << "value length is " << value.length() << std::endl;
            tmp += std::to_string(seq_num_str.length());
            tmp += "\r\n" + seq_num_str + "\r\n";
            result += tmp;
        }
        else{
            std::string ret = "-ERR unknown command\r\n";
            result += ret;
        }
    }
    return result;
}

void HundunDB::Sync(){
    if(role == "master"){
        //sync seq num...
        for(auto iter = seq_num_of_slaves.begin(); iter != seq_num_of_slaves.end(); iter++){
            std::string addr = iter->first;
            std::string ip = addr.substr(0, addr.find(":"));
            int port = std::stoi(addr.substr(addr.find(":") + 1)); 
            struct timeval timeout = {0, 500000}; 
            conn = redisConnectWithTimeout(ip.c_str(), port, timeout);
            void* ret = redisCommand(conn, "getseqnum");
            reply = static_cast<redisReply*>(ret);
            if(reply->str != ""){
                uint64_t seq_num_slave = std::stol(reply->str);
                seq_num_of_slaves[addr] = seq_num_slave;
            }
        }

        //replicat data to remote slave...
        seq_num = db->GetLatestSequenceNumber();
        for(auto iter = seq_num_of_slaves.begin(); iter != seq_num_of_slaves.end(); iter++){
            std::string addr = iter->first;
            std::string ip = addr.substr(0, addr.find(":"));
            int port = std::stoi(addr.substr(addr.find(":") + 1));        
            uint64_t this_seq_num = iter->second;

            if(seq_num > this_seq_num){
                std::unique_ptr<rocksdb::TransactionLogIterator> log_iter;
                s = db->GetUpdatesSince(this_seq_num, &log_iter);
                if(!s.ok()){
                    continue;
                }
                //0.5 seconds.
                struct timeval timeout = {0, 500000}; 
                conn = redisConnectWithTimeout(ip.c_str(), port, timeout);
                if (conn == NULL || conn->err) {
                    if (conn) {
                        std::cout << "Connection error: " << conn->errstr << std::endl;
                    }
                    else {
                        std::cout << "Connection error: can't allocate redis context" << std::endl;
                    }
                    continue;
                }
                while(log_iter->Valid()){
                    auto batch = log_iter->GetBatch();
                    auto data = batch.writeBatchPtr->Data();
                    auto data_size = batch.writeBatchPtr->GetDataSize();
                    //TODO, support more commands...
                    std::vector<std::string> kv = GetKvPair(data, data_size);  
                    void* ret = redisCommand(conn, "SET %s %s", kv[0], kv[1]);  
                    reply = static_cast<redisReply*>(ret);
                    if(reply->str == "OK"){
                        log_iter->Next();
                    } else{
                        break;
                    }
                }
            } else if(seq_num < this_seq_num){
                    //TODO, consider the condition that seq num of slave is larger than master's seq num...
            } else {
                    //synced...
            }
           
        }
    }
}