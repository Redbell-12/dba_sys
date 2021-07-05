#include "mysql_aya.h"
void * sql_std(u_char *,int);

void com_quit(u_char * com_body,int com_length,FILE *fp)
{
}
void com_init_db(u_char * com_body,int com_length,FILE *fp)
{
	fprintf(fp,"切换数据库为：");
	while(com_length>0){
		fprintf(fp,"%c",*com_body);
		com_body++;
		com_length--;
	}		
	fprintf(fp,"\n");
}

/*接受命令正文和命令正文长度*/
u_char *com_query(u_char * com_body,int com_length)
{
        return sql_std(com_body,com_length);
}	

void com_field_list(u_char *com_body,int com_length,FILE *fp)
{
	int table_name_length=0;
	int field_name_length=0;
	/*输出表格名称*/
	fprintf(fp,"table name:");
	while(*com_body!='\0'){
		fprintf(fp,"%c",*com_body);
		com_body++;
		table_name_length++;
	}
	fprintf(fp,"\n");
	/*输出字段名称*/
	table_name_length++;
	com_body++;
	field_name_length=com_length-table_name_length;	
	fprintf(fp,"field name:");
	while(field_name_length>0){
		fprintf(fp,"%c",*com_body);
		com_body++;
		field_name_length--;
	}
	fprintf(fp,"\n");
}
/*该消息已过时*/
void com_create_db(u_char *com_body,int com_length,FILE *fp)
{

}
/*该消息已过时*/
void com_drop_db(u_char *com_body,int com_length,FILE *fp)
{

}
void com_refresh(u_char *com_body,int com_length,FILE *fp)
{
	#define REFRESH_GRANT 0x01
	#define REFRESH_LOG 0x02
	#define REFRESH_TABLES 0x04
	#define REFRESH_HOSTS 0x08
	#define REFRESH_STATUS 0x10
	#define REFRESH_THREADS 0x20
	#define REFRESH_SLAVE 0x40
	#define REFRESH_MASTER 0x80

	if(((*com_body)&REFRESH_GRANT)==REFRESH_GRANT){
		fprintf(fp,"REFRESH_GRANT set\n");
	}
	if(((*com_body)&REFRESH_LOG)==REFRESH_LOG){
		fprintf(fp,"REFRESH_LOG set\n");
	}
	if(((*com_body)&REFRESH_TABLES)==REFRESH_TABLES){
		fprintf(fp,"REFRESH_TABLES set\n");
	}
	if(((*com_body)&REFRESH_HOSTS)==REFRESH_HOSTS){
		fprintf(fp,"REFRESH_HOSTS set\n");
	}
	if(((*com_body)&REFRESH_STATUS)==REFRESH_STATUS){
		fprintf(fp,"REFRESH_STATUS set\n");
	}
	if(((*com_body)&REFRESH_THREADS)==REFRESH_THREADS){
		fprintf(fp,"REFRESH_THREADS set\n");
	}
	if(((*com_body)&REFRESH_SLAVE)==REFRESH_SLAVE){
		fprintf(fp,"REFRESH_SLAVE set\n");
	}
	if(((*com_body)&REFRESH_MASTER)==REFRESH_MASTER){
		fprintf(fp,"REFRESH_MASTER set\n");
	}
}
void com_shutdown(u_char *com_body,int com_length,FILE *fp)
{
	#define SHUTDOWN_DEFAULT 0x00
	#define SHUTDOWN_WAIT_CONNECTIONS 0x01
	#define SHUTDOWN_WAIT_TRANSACTIONS 0x02
	#define SHUTDOWN_WAIT_UPDATES 0X08
	#define SHUTDOWN_WAIT_ALL_BUFFERS 0x10
	#define SHUTDOWN_WAIT_CRITICAL_BUFFERS 0x11
	#define KILL_QUERY 0Xfe
	#define KILL_CONNECTION 0Xff

	if(((*com_body)&SHUTDOWN_DEFAULT)==SHUTDOWN_DEFAULT){
		fprintf(fp,"SHUTDOWN_DEFAULT set\n");
	}
	if(((*com_body)&SHUTDOWN_WAIT_CONNECTIONS)==SHUTDOWN_WAIT_CONNECTIONS){
		fprintf(fp,"SHUTDOWN_WAIT_CONNECTIONS set\n");
	}
	if(((*com_body)&SHUTDOWN_WAIT_TRANSACTIONS)==SHUTDOWN_WAIT_TRANSACTIONS){
		fprintf(fp,"SHUTDOWN_WAIT_TRANSACTIONS set\n");
	}
	if(((*com_body)&SHUTDOWN_WAIT_UPDATES)==SHUTDOWN_WAIT_UPDATES){
		fprintf(fp,"SHUTDOWN_WAIT_UPDATES set\n");
	}
	if(((*com_body)&SHUTDOWN_WAIT_ALL_BUFFERS)==SHUTDOWN_WAIT_ALL_BUFFERS){
		fprintf(fp,"SHUTDOWN_WAIT_ALL_BUFFERS set\n");
	}
	if(((*com_body)&SHUTDOWN_WAIT_CRITICAL_BUFFERS)==SHUTDOWN_WAIT_CRITICAL_BUFFERS){
		fprintf(fp,"SHUTDOWN_WAIT_CRITICAL_BUFFERS set\n");
	}
	if(((*com_body)&KILL_QUERY)==KILL_QUERY){
		fprintf(fp,"KILL_QUERY set\n");
	}
	if(((*com_body)&KILL_CONNECTION)==KILL_CONNECTION){
		fprintf(fp,"KILL_CONNECTION set\n");
	}
}
void com_statistics(u_char *com_body,int com_length,FILE *fp)
{
	fprintf(fp,"com_statistics\n");
}
void com_process_info(u_char *com_body,int com_length,FILE *fp)
{
	fprintf(fp,"com_process_info\n");
}	
void com_process_kill(u_char *com_body,int com_length,FILE *fp)
{
	fprintf(fp,"%u\n",*((int *)com_body));
}
void com_debug(u_char *com_body,int com_length,FILE *fp)
{
	fprintf(fp,"com_debug\n");
}
void com_ping(u_char *com_body,int com_length,FILE *fp)
{
	fprintf(fp,"com_ping\n");
}
void com_change_user(u_char *com_body,int com_length,FILE *fp)
{
	//密码字段有疑问
}
void com_binlog_dump(u_char *com_body,int com_length,FILE *fp)
{
	//该消息是备份时,由从服务器向主服务器发送的请求
	//不是客户端发往服务器的，所以暂时不实现
}
void com_table_dump(u_char *com_body,int com_length,FILE *fp)
{
	//该消息已过时，不再使用
}
void com_register_slave(u_char *com_body,int com_length,FILE *fp)
{
	//此消息用于从服务器向主服务器发送注册消息
}
void com_prepare(u_char *com_body,int com_length,FILE *fp)
{
	//就是com_query
	fprintf(fp,"预处理sql语句：");
	while(com_length>0){
		fprintf(fp,"%c",*com_body);
		com_body++;
		com_length--;
	}
	fprintf(fp,"\n");
}
void com_execute(u_char *com_body,int com_length,FILE *fp)
{
	//经试验这还是com_query
}
void com_long_data(u_char *com_body,int com_length,FILE *fp)
{
	//不知道什么命令用到这个消息
}
void com_close_stmt(u_char *com_body,int com_length,FILE *fp)
{
	//也是com_query
}
