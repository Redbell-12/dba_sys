#include "redis.h"
#include "../buffer/buffer.h"
void out_header(buffer *,long,u_char *,u_char *,u_char *,u_char *,int,int,int);
data bulk(char *bulk);
data status_error_integer(char *status);
/*将报文中的各有效值解析出存放到data arr[]数组中。num指明子报文个数，也即arr数组的有效元素*/
void redis_aya(buffer *buf,int dir,int db_type,int is_compress,u_char *payload,u_char *src_ip,u_char *src_port,
		u_char *dst_ip,u_char *dst_port,long op_time,u_char *seq_number,u_char *ack_number)
{
	data arr[1024];//这里假定一个复合报文的子报文数量不超过1024个
	long num=0;
	if(*payload=='*'){//复合报文
		//计算*后面到换行之间的数值，ascii码转二进制
		payload++;
		while(!((*payload==0x0d)&&(*(payload+1)==0x0a))){
			num=(num*10)+(*payload-'0');
			payload++;
		}
		payload+=2;//使payload指向复合报文的第一个$
		//循环解析num个子报文
		int i;
		for(i=0;i<num;i++){
			//判断子报文的类型并解析之
			if(*payload=='$'){
				arr[i]=bulk(payload);
				//将指针移至下一个子报文。注意下面4句要执行两次才能移至下一个子报文，因为每个$报文有两对0x0d 0x0a需要跳过
				while(!((*payload==0x0d)&&(*(payload+1)==0x0a))){
					payload++;
				}
				payload+=2;
				while(!((*payload==0x0d)&&(*(payload+1)==0x0a))){
					payload++;
				}
				payload+=2;
			}else if((*payload=='+')||(*payload=='-')||(*payload==':')){
				arr[i]=status_error_integer(payload);
				//将指针移至下一个子报文
				while(!((*payload==0x0d)&&(*(payload+1)==0x0a))){
                                        payload++;
                                }
                                payload+=2;
			}
		}
	}else if(*payload=='$'){
		num=1;//复合报文时*后面的值赋值num，但这里是单一报文，我们手动设置num，否则147行的代码执行不了
		arr[0]=bulk(payload);
	}else if((*payload=='+')||(*payload=='-')||(*payload==':')){
		num=1;
		arr[0]=status_error_integer(payload);
	}

	u_char array[100];
	int n;
	if(!(strcmp(arr[0].data,"AUTH"))){//这是个登陆报文
		out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);
		/*用户名*/
		n=sprintf(array,"sab|||");
		print_buf(array,n,buf);
		/*数据库名*/
		n=sprintf(array,"database_name|||");
		print_buf(array,n,buf);
		/*表名*/
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
		/*输出最后部分:用户名 密码*/
		n=sprintf(array,"login:");
		print_buf(array,n,buf);
		/*用户名*/
		n=sprintf(array,"null");
		print_buf(array,n,buf);

		n=sprintf(array," ");
		print_buf(array,n,buf);
		/*密码*/
		n=sprintf(array,arr[1].data);
		print_buf(array,n,buf);

		n=sprintf(array,"\n");
		print_buf(array,n,buf);	
	}else if(dir==1){
		/*输出格式头*/
		out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);
		/*用户名*/
		n=sprintf(array,"sab|||");
		print_buf(array,n,buf);
		/*数据库名*/
		n=sprintf(array,"database_name|||");
		print_buf(array,n,buf);
		/*表名*/	
		n=sprintf(array,"table_name|||");
		print_buf(array,n,buf);	
		/*操作*/
		int legal=is_legal(arr[0].data,arr[0].len,1);//1表示使用redis的命令集	
		if(legal==0){
			//print_buf(arr[0].data,arr[0].len,buf);//命令写入缓冲区
			/*把首单词转成大写后写入缓冲区。转大写本不该是这里的功能*/
                        char upper[arr[0].len];
                        int i;
                        for(i=0;i<arr[0].len;i++){
                                upper[i]=toupper(arr[0].data[i]);
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
		/*输出所有内容*/
		int i;
		for(i=0;i<num;i++){
			print_buf(arr[i].data,arr[i].len,buf);
			print_buf(" ",1,buf);
		}
			
		n=sprintf(array,"\n");
		print_buf(array,n,buf);	
	}else if(dir==2){
		/*输出格式头*/
		out_header(buf,op_time,src_ip,src_port,dst_ip,dst_port,db_type,dir,is_compress);
		/*用户名*/
		n=sprintf(array,"sab|||");
		print_buf(array,n,buf);
		/*数据库名*/
		n=sprintf(array,"database_name|||");
		print_buf(array,n,buf);
		/*表名*/	
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
		/*输出所有内容*/
		int i;
		for(i=0;i<num;i++){
			print_buf(arr[i].data,arr[i].len,buf);
		}
			
		n=sprintf(array,"\n");
		print_buf(array,n,buf);	
	}
	int i;
	for(i=0;i<num;i++){
		free(arr[i].data);
	}	
}
