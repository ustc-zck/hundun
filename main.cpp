#include "src/db.h"

HundunDB* hundun_db;

std::string handler(char* buf){
    //set handler...
    std::string ret = hundun_db->Handler(buf);
    
    std::unique_ptr<rocksdb::TransactionLogIterator> iter;
    rocksdb::Status s = hundun_db->GetRocksDb()->GetUpdatesSince(0, &iter);

    while(iter->Valid()){
        auto batch = iter->GetBatch();
        std::cout << "seq num is " << batch.sequence << std::endl;
        std::cout << "seq data is " << batch.writeBatchPtr->Data() << std::endl;
        iter->Next();
    }

    std::cout << "role is " << hundun_db->Role() << std::endl;
    auto seq_num_of_slaves = hundun_db->SeqNumOfSlaves();
    for(auto iter = seq_num_of_slaves.begin(); iter != seq_num_of_slaves.end(); iter++){
        std::cout << "slave addr is " << iter->first << std::endl;
    }
    return ret;
}

int Sync(){
    hundun_db->Sync();
    return 0;
}

std::string echo(char* buf){
    //set handler...
    //std::string ret = hundun_db->Handler(buf);
    std::cout << "ret is " << buf << std::endl;
    return buf;
}

//g++ -o main main.cpp src/* lib/lei/src/* -lrocksdb -lpthread -ldl
int main(){ 
    std::string path = "data1/";
    hundun_db = new HundunDB(path, 9001);
    std::cout << "listening on port: " << hundun_db->Port() << std::endl;
    Server* s = new Server(hundun_db->Port());
    s->Handler = &handler;
    s->TimeHandler = &Sync;
    s->AddTimeEvent(1000);
    s->Run();
    return 0;
}

