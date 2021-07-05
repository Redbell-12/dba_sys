#include "tds.h"
#include "../buffer/buffer.h"
void *sql_batch_request(u_char *);
void out_header(buffer *,long,u_char *,u_char *,u_char *,u_char *,int,int,int);

//int tds_analysis(int payload_direction,u_char *payload,u_char *src_ip,u_char *dst_ip,long op_time,u_char *seq_number,u_char *ack_number)
int tds_analysis(buffer *buf,int dir,int db_type,int is_compress,u_char *payload,u_char *src_ip,u_char *src_port,
                u_char *dst_ip,u_char *dst_port,long op_time,u_char *seq_number,u_char *ack_number)
{
	int n;
	char array[100];
	/*根据报文方向进行不同的输出*/
	if(*payload==0x01){
		/*输出格式头*/
		out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);

		/*解析sql语句*/
		u_char *text=sql_batch_request(payload);
		/*用户名*/
		n=sprintf(array,"sab|||");
		print_buf(array,n,buf);
		/*数据库名,表名*/	
		n=sprintf(array,"database_name|||");
                print_buf(array,n,buf);
                n=sprintf(array,"table_name|||");
                print_buf(array,n,buf);
		/*操作类型（sql语句首单词）*/
                //将命令类型与所有合法命令类型比较，若没有匹配到，说明用户输入了非法命令
                //若匹配到了，则输出sql语句首单词
		int legal=is_legal(text,strlen(text),0);
                if(legal==0){
                        u_char *p=text;
                        n=0;
                        while((*p)!=' '){
                                p++;
                                n++;
                        }
                        //print_buf(text,n,buf);//sql语句首单词写入缓冲区
			/*把首单词转成大写后写入缓冲区。转大写本不该是这里的功能*/
			char upper[n];
			int i;
			for(i=0;i<n;i++){
				upper[i]=toupper(text[i]);
			}
			print_buf(upper,n,buf);//sql语句首单词写入缓冲区
                }else if(legal==1){
                        n=sprintf(array,"ILLEGAL");
                        print_buf(array,n,buf);
                }
                /*响应*/
                n=sprintf(array,"|||ok|||");
                print_buf(array,n,buf);
                /*风险*/
		n=sprintf(array,"1|||");
                print_buf(array,n,buf);
		/*输出整个sql语句到缓冲区*/
                //fprintf(fp,"%s",text);
                print_buf(text,strlen(text),buf);
		//输出换行到缓冲区
		n=sprintf(array,"\n");
                print_buf(array,n,buf);
                free(text);
	}else if(*payload==0x04){
		/*输出格式头*/
		out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);

		/*用户名*/
		n=sprintf(array,"sab|||");
		print_buf(array,n,buf);	
                /*数据库名,表名*/
		/*这里mysql数据库是可以输出数据库名和表名的，但sqlserver不可以，因为它的返回报文中没有数据库名和表名*/
		n=sprintf(array,"database_name|||");
                print_buf(array,n,buf);
                n=sprintf(array,"table_name|||");
                print_buf(array,n,buf);
                /*操作*/
                n=sprintf(array,"RESPONSE|||");
                print_buf(array,n,buf);
                /*响应*/
                n=sprintf(array,"ok|||");
                print_buf(array,n,buf);
                /*风险*/
                n=sprintf(array,"1|||");
                print_buf(array,n,buf);
                /*输出整个返回*/
		response(payload,buf);
                n=sprintf(array,"\n");
                print_buf(array,n,buf);
	}else if(*payload==0x12){//预登陆
		static int status;
		if(status==0){
			status=1;//碰到第一个预登陆报文，只置状态，不解
		}else if(status==1){
			status=2;
		}else if(status==2){
			status=3;
		}else if(status==3){
			/*输出格式头*/
			out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);

			/*用户名*/
			n=sprintf(array,"sab|||");
			print_buf(array,n,buf);
			/*数据库名,表名*/
			n=sprintf(array,"database_name|||");
			print_buf(array,n,buf);
			n=sprintf(array,"table_name|||");
			print_buf(array,n,buf);
			/*操作*/
			n=sprintf(array,"LOGIN|||");
			print_buf(array,n,buf);
			/*响应*/
			n=sprintf(array,"ok|||");
			print_buf(array,n,buf);
			/*风险*/
			n=sprintf(array,"1|||");
			print_buf(array,n,buf);
			/*输出最后部分*/
			n=sprintf(array,"login:");
			print_buf(array,n,buf);

			n=sprintf(array,"\n");
			print_buf(array,n,buf);
			
			status=0;//解完pre-login报文后将状态复原
		}
	}
}
/*
        u_char *text=NULL;
	switch(*payload)
	{
		
		case 0x00:;break;
		case 0x01:text=sql_batch_request(payload);
			  print(text,src_ip,dst_ip,op_time,seq_number,ack_number);
			  break;
		case 0x02:;break;//登陆
		case 0x03:rpc_request(payload);break;
		//...
		case 0x04:response(payload,src_ip,dst_ip,op_time,seq_number,ack_number);break;
		//...
		case 0x12:;break;//预登陆
		case 0x1c:;break;
		default:printf("没有匹配到请求命令类型\n");
	}
*/

