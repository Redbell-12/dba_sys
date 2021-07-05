#ifndef _ANALYSIS_MYSQL
#define _ANALYSIS_MYSQL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct{
        long number;//长度编码的指示器所指示的长度数值
        short bytes;//长度编码的指示器本身所占的字节数
}NUMBER_AND_BYTES;
NUMBER_AND_BYTES number_and_bytes;

#endif
