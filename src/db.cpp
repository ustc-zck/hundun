#include "db.h"

HundunDB::HundunDB(std::string data_path){
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
}

HundunDB::HundunDB(rocksdb::DB* db_){
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
}

HundunDB::~HundunDB(){
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
                db->Delete(rocksdb::WriteOptions(), iter->key());
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
    if(iter->Valid()){
        auto key = iter->key().ToString();
        s = db->Delete(rocksdb::WriteOptions(), key);
        if(!s.ok()){
            std::cout << "failed to delete prefix" << std::endl;
            return -1;
        }
        std::cout << "delete key: " << key << std::endl;
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
            } else if (keys[k] != "no" && args[j][0] != "one"){
                std::string master_addr = keys[k] + ":" + args[j][0];
                redisContext *conn;
                redisReply *reply;
                //0.5 seconds.
                struct timeval timeout = {0, 500000}; 
                conn = redisConnectWithTimeout(keys[k].c_str(),  std::stoi(args[j][0]), timeout);
                if (conn == NULL || conn->err) {
                    if (conn) {
                        std::cout << "Connection error: " << conn->errstr << std::endl;
                        redisFree(conn);
                    }
                    else {
                        std::cout << "Connection error: can't allocate redis context" << std::endl;
                    }
                    continue;
                }
                void* ret = redisCommand(conn, "addslave %s", master_addr);
                reply = static_cast<redisReply*>(ret);
                if(reply->str == "OK"){
                    result += "+OK\r\n";
                } else {
                    result += "-ERR slave of error\r\n";
                }
                redisFree(conn);
                freeReplyObject(reply);
            } else{
                result += "-ERR server put error\r\n";
            }
            k++;
            j++;
        }
        else if(cmd == "addslave"){
            this->AddSlave(keys[k]);
            k++;
        }
        else{
            std::string ret = "-ERR unknown command\r\n";
            result += ret;
        }
    }
    return result;
}

void HundunDB::Sync(){
    //TODO，sync seq num...
    //master request seq num from slaves...

    if(role == "master"){
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
                redisContext *conn;
                redisReply *reply;
                //0.5 seconds.
                struct timeval timeout = {0, 500000}; 
                conn = redisConnectWithTimeout(ip.c_str(), port, timeout);
                if (conn == NULL || conn->err) {
                    if (conn) {
                        std::cout << "Connection error: " << conn->errstr << std::endl;
                        redisFree(conn);
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
                redisFree(conn);
                freeReplyObject(reply);
            } else if(seq_num < this_seq_num){
                    //TODO, consider the condition that seq num of slave is larger than master's seq num...
            } else {
                    //synced...
            }
           
        }
    }
}