#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>

// gcc -o test_hiredis test_hiredis.c -I /usr/local/include/hiredis  -lhiredis
int main() {
     redisContext *conn;
     redisReply *reply;

     struct timeval timeout = { 1, 500000 }; // 1.5 seconds
     conn = redisConnectWithTimeout("127.0.0.1", 9001, timeout);
     if (conn == NULL || conn->err) {
	if (conn) {
             printf("Connection error: %s\n", conn->errstr);
             redisFree(conn);
        }
	else {
             printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
     }
     /* Set */
     char* key = "hello";
     char* val = "slave";
     reply = redisCommand(conn, "SET %s %s", key, val);
     printf("reply: %s\n", reply->str);
     freeReplyObject(reply);
     redisFree(conn);
     return 0;
}

