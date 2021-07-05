/*
 * 这是一个缓冲区，由循环双链表实现，数据从头部往next方向入链，从头部往pre方向出链写入文件
 * 有两个操作接口print_buf和print_file前者用来往缓冲区中写入len个字符，后者用来将缓冲区
 * 中的内容写入文件。后者调用的时机为固定时间间隔
 */
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<signal.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include"buffer.h"

//接受一个buffer的地址，初始化这个buffer
int init_buffer(buffer *buf)
{
	//生成头节点
	buf->head=(buf_node *)malloc(sizeof(buf_node));
	if(buf->head==NULL){
		printf("malloc fail\n");
		return -1;
	}	
	buf->head->data=NULL;
	buf->head->next=buf->head;
	buf->head->pre=buf->head;
	//初始化锁
	buf->lock=0;
}
/*将p指向的len个字符写入buf所指向的缓冲区*/
int print_buf(char *p,int len,buffer *buf)
{
	//申请len+1个字节，将len个字符写入
	char *data=(char *)malloc(len+1);
	if(data==NULL){
		printf("malloc fail\n");
		return -1;
	}
	int i;
	for(i=0;i<len;i++)
		data[i]=p[i];
	data[i]='\0';
	//申请一个节点，绑定数据和节点
	buf_node *node=(buf_node *)malloc(sizeof(buf_node));

	if(node==NULL){
		printf("malloc fail\n");
		return -1;
	}
	node->data=data;
	node->next=NULL;
	node->pre=NULL;
	//将节点入链
	buf->lock=1;
	node->next=buf->head->next;
	buf->head->next=node;

	//node->pre=buf->head->pre;	
	node->pre=buf->head;	
	node->next->pre=node;	
	buf->lock=0;
}
/*将缓冲区中的内容写入文件并释放缓冲区*/
int print_file(buffer *buf,FILE * fp)
{
	if(buf->lock==1){
		printf("lock\n");
		return;
	}
	//清空链表，只留下头节点
	while(buf->head->pre!=buf->head){
		buf_node *p=buf->head->pre;
		buf->head->pre=p->pre;
		p->pre->next=buf->head;
		//fprintf(fp,"%s",p->data);
		fwrite(p->data,strlen(p->data),1,fp);
		free(p->data);
		free(p);
	}
}
/*定时处理程序*/
//extern buffer mysql_buffer;
//extern buffer sqlserver_buffer;
//extern buffer oracle_buffer;
//extern buffer redis_buffer;
extern buffer syslog_buffer;
int timer_handler()
{
//	int fd=0;
//	fd=open("./outfile",O_RDWR|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
	//FILE *fp;
	FILE *fp=fopen("/home/dba_sys.txt","a+");
	//print_file(&mysql_buffer,fp);
	//print_file(&sqlserver_buffer,fp);
	//print_file(&oracle_buffer,fp);
	//print_file(&redis_buffer,fp);
	print_file(&syslog_buffer,fp);
	fclose(fp);
/*
	fd=open("./sqlserver_outfile",O_RDWR|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
	perror("open file2");
	print_file(&sqlserver_buffer,fd);
	close(fd);
	printf("close file2\n");

	fd=open("./oracle_outfile",O_RDWR|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
	perror("open file3");
	print_file(&oracle_buffer,fd);
	close(fd);
	printf("close file3\n");
*/	
}
/*设置定时*/
int set_timer(int sec)
{
	//signal(SIGALRM,(__sighandler_t)timer_handler);
	struct sigaction act,oldact;
	act.sa_handler=timer_handler;
	//sigaddset(&act.sa_mask,SIGALRM);
	act.sa_flags=0;
	sigaction(SIGALRM,&act,&oldact);
	
	struct itimerval tick;
	memset(&tick,0,sizeof(tick));
	tick.it_value.tv_sec=sec;
	tick.it_value.tv_usec=0;
	tick.it_interval.tv_sec=sec;
	tick.it_interval.tv_usec=0;

	setitimer(ITIMER_REAL,&tick,NULL);
}
