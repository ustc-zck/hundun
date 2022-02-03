#include "db.h"
#include "parse.h"
#include <vector>

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
}

HundunDB::~HundunDB(){
    delete db;
}

std::string HundunDB::Get(std::string key){
    std::string value;
    // get value
    s = db->Get(rocksdb::ReadOptions(), key, &value);
    if(s.ok()){
        return value;
    }
    return "\0";
}

int HundunDB::Put(std::string key, std::string value){
    s = db->Put(rocksdb::WriteOptions(), key, value);
    if(s.ok()){
        return 1;
    }
    return -1;
}

std::string HundunDB::Handler(std::string buf){
    Parser* parser = new Parser(buf);
    parser->Parse();
    std::vector<std::string> cmds = parser->Cmds();
    std::vector<std::string> keys = parser->Keys();
    std::vector<std::vector<std::string>> args = parser->Args();

    // for(int i = 0; i < cmds.size(); i++){
    //     std::cout << "cmd is " << cmds[i] << std::endl;
    // }
    // for(int i = 0; i < keys.size(); i++){
    //     std::cout << "key is " << keys[i] << std::endl;
    // }
    
    // for(int i = 0; i < args.size(); i++){
    //     for(int j = 0; j < args[i].size(); j++){
    //         std::cout << "arg is " << args[i][j] << std::endl;
    //     }
    // }

    std::string result = "";
    int j = 0, k = 0;
    for(int i = 0; i < cmds.size(); i++){
        std::string cmd = cmds[i];
        //transform string into lower case...
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return std::tolower(c);});
        std::cout << "lower case cmd is " << cmd << std::endl;
        if(cmd == "get"){
            std::string value = this->Get(keys[k]);
            std::cout << "value is" << std::endl;
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
            if(keys[i] == "" || args[i].size() != 1){
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
        // else if(){
        // TODO, add more redis command...
        // }
        else{
            std::string ret = "-ERR unknown command\r\n";
            result += ret;
        }
    }
    return result;
}


