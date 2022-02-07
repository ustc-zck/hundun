#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>

// gcc -o test_hiredis test_hiredis.c -I /usr/local/include/hiredis  -lhiredis
int main(int argc, char **argv) {
     redisContext *conn;
     redisReply *reply;

     const char *password = argv[3];
     struct timeval timeout = { 1, 500000 }; // 1.5 seconds
     conn = redisConnectWithTimeout("127.0.0.1", 9000, timeout);
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
     reply = redisCommand(conn,"SET %s %s", "welcome", "Hello, DCS for Redis!");
     printf("SET: %s\n", reply->str);
     reply = redisCommand(conn, "GET welcome");
     printf("Value is: %s\n", reply->str);
     freeReplyObject(reply);
     redisFree(conn);
     return 0;
}