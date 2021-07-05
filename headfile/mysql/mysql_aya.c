#include "mysql_aya.h"
#include "../buffer/buffer.h"
NUMBER_AND_BYTES length_coded_analysis(u_char *);
u_char *com_query(u_char *,int);
void out_header(buffer *,long,u_char *,u_char *,u_char *,u_char *,int,int,int);

void mysql_aya(buffer *buf,int dir,int db_type,int is_compress,u_char *payload,u_char *src_ip,u_char *src_port,
		u_char *dst_ip,u_char *dst_port,long op_time,u_char *seq_number,u_char *ack_number)
{
	u_char array[100];
	int n;
	/*根据报文方向进行不同的输出*/
	if(dir==1){
		if(*(payload+4)==0x03){//sql语句
			/*输出格式头*/
			out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);

			/*解析sql语句*/
			/*注意包长度不包含4字节包头，所以com_query的第二个参数是packet_length-1，而非减5*/
			int packet_length=(*(int *)payload)&0xffffff;
			u_char *text=com_query(payload+5,packet_length-1);//此函数执行后text指向的堆内存中存放的是已经规范化后的sql语句了

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
			int legal=is_legal(text,packet_length-1,0);	
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
		}else if((*(payload+4)>0x1c)||(*(payload+3)==1)){//登陆报文，不靠谱
			/*输出格式头*/
			out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);
			
			u_char *username;
			u_char *passwd;
			u_char *db_name;
			int passwd_len;
			authenticate(&username,&passwd,&db_name,&passwd_len,payload,buf);
			/*表名*/
			n=sprintf(array,"tb_name|||");
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

			n=sprintf(array,username);
                        print_buf(array,n,buf);

			n=sprintf(array," ");
                        print_buf(array,n,buf);
			int i;n=0;
			for(i=0;i<passwd_len;i++){
				n+=sprintf(array+n,"%02x",passwd[i]);
			}
                        print_buf(array,strlen(array),buf);

			if(db_name!=NULL){
				n=sprintf(array," ");
				print_buf(array,n,buf);

				n=sprintf(array,db_name);
				print_buf(array,n,buf);
			}

			n=sprintf(array,"\n");
			print_buf(array,n,buf);	
		}else if(*(payload+4)==0x01){//登出
			/*输出格式头*/
			out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);
			
			//用户名
			n=sprintf(array,"null|||");
			print_buf(array,n,buf);
			//数据库名
			n=sprintf(array,"null|||");
                        print_buf(array,n,buf);
			/*表名*/
			n=sprintf(array,"null|||");
			print_buf(array,n,buf);
			/*操作*/
			n=sprintf(array,"LOGOUT|||");
			print_buf(array,n,buf);
			/*响应*/
			n=sprintf(array,"ok|||");
			print_buf(array,n,buf);
			/*风险*/
			n=sprintf(array,"1|||");
			print_buf(array,n,buf);
			/*输出最后部分*/
			n=sprintf(array,"logout:");
                        print_buf(array,n,buf);

			n=sprintf(array,"\n");
			print_buf(array,n,buf);	
		}
	}else if((dir==2)&&(*(payload+4)!=0x00)&&(*(payload+4)!=0xff)){
		/*输出格式头*/
		out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);

		/*用户名*/
		n=sprintf(array,"sab|||");
		print_buf(array,n,buf);
		/*数据库名,表名*/
		//调用field_analysis函数，输出数据库名和表名
		number_and_bytes=length_coded_analysis(payload+4);
		u_char *field_packet=payload+4+number_and_bytes.bytes;
		field_analysis(field_packet+4,buf);
		/*操作*/
		n=sprintf(array,"|||RESPONSE|||");
		print_buf(array,n,buf);	
		/*响应*/
		n=sprintf(array,"ok|||");
		print_buf(array,n,buf);	
		/*风险*/
		n=sprintf(array,"1|||");
		print_buf(array,n,buf);	
		/*输出整个返回*/
		result_set(payload+4,buf);
		n=sprintf(array,"\n");
		print_buf(array,n,buf);	
	}
}
/*
if(payload_direction==request){
	switch(*(payload+4))
	{
		case 0x00:;break;
		case 0x01:;break;

		case 0x03:aya_and_print(buf,payload_direction,db_type,is_compress,payload,src_ip,src_port,
					  dst_ip,dst_port,op_time,seq_number,ack_number);

		case 0x1c:;break;
		default:printf("没有匹配到请求命令类型\n");
	}
}else if(payload_direction==response){
	switch(*(payload+4))
	{
		case 0x00:ok(payload+5);break;
		case 0xff:error(payload+5);break;
		default:aya_and_print(buf,payload_direction,db_type,is_compress,payload,src_ip,src_port,
					dst_ip,dst_port,op_time,seq_number,ack_number);
	}
}
*/
