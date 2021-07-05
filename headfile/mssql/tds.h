#ifndef _TDS_H
#define _TDS_H

#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

/*用于转换date，datetime等与时间有关的数据类型*/
typedef struct{
	unsigned short year;
	unsigned short mon;
	unsigned short day;
	unsigned short hour;
	unsigned short min;
	unsigned short sec;
	unsigned short last;
}TIME;

/*
 * 原则上应该定义完整的column格式，但目前只用到这两个，所以只定义这两个
 * 后续如果需要可以再补充
 */
typedef struct{
	int datatype;
	int scale;//当类型是165，167，231时，此变量中存的就不是scale了，所以此变量的用途并不专一，随类型而定
}COLUMN;

#endif
