#include<stdio.h>
#include<sys/types.h>
#include"../buffer/buffer.h"
void out_header(buffer *buf,long op_time,u_char *src_ip,u_char *src_port,u_char *dst_ip,u_char *dst_port,int db_type,int dir,int is_compress)
{
	u_char *visitor_ip=NULL;
        u_char *visitor_port=NULL;
        u_char *db_ip=NULL;
        u_char *db_port=NULL;
        if(dir==1){
                visitor_ip=src_ip;
                visitor_port=src_port;
                db_ip=dst_ip;
                db_port=dst_port;
        }else if(dir==2){
                visitor_ip=dst_ip;
                visitor_port=dst_port;
                db_ip=src_ip;
                db_port=src_port;
        }

	u_char array[100];
                         /*操作时间,访问者ip,       访问者端口，   数据库ip,      数据库端口, DB类型, 方向, 压缩*/
        int n=sprintf(array,"%ld|||%02x%02x%02x%02x|||%02x%02x|||%02x%02x%02x%02x|||%02x%02x|||%02d|||%02d|||%02d|||",
                        op_time,
                        *visitor_ip,*(visitor_ip+1),*(visitor_ip+2),*(visitor_ip+3),
                        *visitor_port,*(visitor_port+1),
                        *db_ip,*(db_ip+1),*(db_ip+2),*(db_ip+3),
                        *db_port,*(db_port+1),
                        db_type,
                        dir,
			//*seq_number,*(seq_number+1),*(seq_number+2),*(seq_number+3),
			//*ack_number,*(ack_number+1),*(ack_number+2),*(ack_number+3),
			is_compress);
	//将sprintf写入array数组的n个字符写入缓冲区
	print_buf(array,n,buf);
}
