# hundun
Hundun is a persistent kv store based on rocksdb engine, which is compatible with redis protocol. The hundun kv store should support redis protocol, replication（including failover, leader selection and duplication)， distributed storage (supported by codis and zookeeper), slot migration and so on.

#PART I Redis Protocol

Now, get and set command in redis is included into HundunDB. More commands are to be added in the future.


#PART II Replication

Now, asynchronous replication is supported from master to slave.

#PART III Distributed Storage 

Supported by codis.

#PART IV Slot Migration

The hundun db is baseed on rocksdb engine. Therefore, different from codis, slot migration should be implemented by opearting sst file. For incremental data， it shoule be directly written into target instance. At last, delete sst file in the source machine.

#TODO 

Adopt raft or paxos protocol to replicae data between instances.

At last, this project is just for learning, not for production.
