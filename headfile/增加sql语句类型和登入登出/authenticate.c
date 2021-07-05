#include "mysql_aya.h"
#include "../buffer/buffer.h"
void *authenticate(u_char **user_name,u_char **passwd,u_char **dbname,int *passwd_len,u_char *payload,buffer *buf)
{
        u_char array[100];
        int n;

        u_char *auth=payload+4;

        int username_size=1;
        u_char * username=auth+32;
        unsigned long password_size=0;
        u_char * password;

        u_char * db_name;

	//将用户名输入缓冲区
	n=sprintf(array,username);
	print_buf(array,n,buf);
	
        n=sprintf(array,"|||");
	print_buf(array,n,buf);

        /*计算用户名长度*/
	*user_name=username;
        while((*username)!='\0'){
                username++;
                username_size++;
        }
	/*计算挑战认证数据长度*/
        password=++username;
        /*计算password_size*/
        if((*password)<251){
                password_size=*password;
                password+=1;
        }else if((*password)==0xfc){
                password_size=*((short *)(password+1));
                password+=3;
        }else if((*password)==0xfd){
                password_size=(*((int *)(password+1)))&0xffffff;//the number in next 3 bytes;
                password+=4;
        }else{
                password_size=*((long *)(password+1));//the number in next 8 bytes;
                password+=9;
        }
	*passwd=password;
	*passwd_len=password_size;
        db_name=password+password_size;
        #define CONNECT_WITH_DB 0x08 //此标志位置位的话，那么报文中就会有数据库名称的信息
        if(((*((unsigned int *)auth)) & CONNECT_WITH_DB) == CONNECT_WITH_DB){
		//将数据库名输入缓冲区
		n=sprintf(array,db_name);
		print_buf(array,n,buf);
		*dbname=db_name;	
        }else{
		//没有数据库名就输出null
		n=sprintf(array,"null");
                print_buf(array,n,buf);
		*dbname=NULL;	
	}
	n=sprintf(array,"|||");
	print_buf(array,n,buf);
}
