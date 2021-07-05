#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <stdio.h>
#include <pthread.h>
#include "uthash/src/uthash.h"
#include "uthash/src/utlist.h"
#include <semaphore.h>
#include <pcap.h>

#include "headfile/buffer/buffer.h"
#include "headfile/buffer/buffer2.h"
#include "headfile/buffer/out_header.h"


#define sql_request 1
#define sql_response 2

#define TCP_WINDOW 20000
#define OVECCOUNT 30/* should be a multiple of 3 */

pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex_config = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex_config2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex_policy = PTHREAD_MUTEX_INITIALIZER;

u_char *buf_share;
u_char *buf_check_list;
u_char *buf_sys;


struct my_tcptable_struct {
	char hexname[36];          
	unsigned char *tcp_data;
	u_char TCP_BIG_DATA[TCP_WINDOW];
	unsigned char *tcp_head;
	int tcp_num;
	int tcp_len;
	int tcp_data_len;
	int tcp_data_all_len;
	int tcp_head_len;
	int tcp_state;
	int state;
	unsigned long ack_num;
	unsigned long seq_num;
	unsigned long syn_num;
	unsigned long psh_num;
	unsigned long first_seq;

	int time1;
	int docheck;
	UT_hash_handle hh;         /* makes this structure hashable */
};
struct my_tcptable_struct *my_tcptable=NULL;


typedef struct policy_log_struct {
	char bname[3001];
	struct policy_log_struct *next, *prev;
} policy_log_struct;

typedef struct dba_policy_struct {
	int id;
	char key_name[11];
	char key_ip[13];
	int log_type;
	int action;
	int str_len;
	pcre *re;
	char re_char[2001];
	char replace[2001];
	struct dba_policy_struct *next, *prev;
} dba_policy_struct;

typedef struct dba_config_struct {
	int dbip[4];
	int db_port[2];
	char key_ipp[13];
	char userkey[10];
	char security_level[2];
	u_char policy_open[101];
	int db_type;
	//int status; //0:reject;1:receive;
	char status[2];
	int count_num;
	int if_custom;
	int db_ipp[6];
	int vis_ip1[4];
	int vis_ip2[4];
	int vis_ip1_int;
	int vis_ip2_int;
	char custom_char[100];
	pcre *cus;
	struct dba_config_struct *next, *prev;
} dba_config_struct;

dba_config_struct *config_head = NULL; /* important- initialize to NULL! */
dba_config_struct *config_name, *config_elt, *config_tmp, config_etmp;
dba_policy_struct *policy_head = NULL; /* important- initialize to NULL! */
dba_policy_struct *policy_name, *policy_elt, *policy_tmp, policy_etmp, findstr_elt;
policy_log_struct *policy_log_head = NULL; /* important- initialize to NULL! */
policy_log_struct *policy_log_name, *policy_log_elt, *policy_log_tmp, policy_log_etmp;

int c1 = 0, c2 = 0;
int count_all = 0;
u_char *buf_share = NULL;
//u_char *sql_buf;
int layer1_len = 14;
int layer2_ip_len = 20;
int num_cons;
int layer3_tcp_len = 20;
int layer3_udp_len = 8;

int compressed = 0;
u_char my_src_ip[4];
int pkt_len_share;
char my_src_ip_char[9];
int db_type = 1;
time_t tim;
int count_sql = 0;
int dev_mode;
int security_level=3;
int serdata_check=1;
int bw_count = 0;
int sql_len = 0;
int chkq_post_start1=0;
int num_cons;
int last_packet_status = 0;
int last_dirction = 0;
char rebuild_buf1[10000] = "";
char rebuild_buf2[10000] = "";
int last_buf1 = 0;
int last_buf2 = 0;
int is_compress = 0;
FILE *fp = NULL;
char sys_content[1600] = "";
char key_ip_zero[12]="000000000000";
char status_on[2]="01";

buffer mysql_buffer, sqlserver_buffer, oracle_buffer, redis_buffer, syslog_buffer;
/*

3wnrfvkzdx 7 0 192.168.148.46 OSType16  000000000000|||c0a8942e0cea|||00|||00|||5.5.28 200121 14:20:46     9 Connect root@localhost on 
1578667281|||00000000|||0000|||c0a8930f|||05f1|||03|||01|||00|||sab|||database_name|||table_name|||SELECT|||ok|||1|||select max(value) from v$sysmetric_history where metric_name = Average Active Sessions
1578667281|||02|||000000000000|||000000000000|||c0a8930f05f1|||000000000000|||00|||00|||0|||true|||2020-01-17 07:37:28.2250757|||53|||0|||0|||WIN-9K6K2JE6MSA\Administrator|||123|||dbo.test|||select * from dbo.test

*/
static int waf_htoi(char *s)
{
	int value;
	int c;

	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
	//printf("value1 is %d\n", value);
	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
	//printf("value2 is %d\n", value);
	return (value);
}


static int waf_htoi2(char *s)
{
	int value = 0;
	int c1, c2, c3, c4;

	c1 = ((unsigned char *)s)[0];
	if (isupper(c1))
		c1 = tolower(c1);
	value = 0;
	printf("value11 is %d\n", value);
	c2 = ((unsigned char *)s)[1];
	if (isupper(c2))
		c2 = tolower(c2);
	value += (c2 >= '0' && c2 <= '9' ? c2 - '0' : c2 - 'a' + 10) * 256;
	printf("value22 is %d\n", value);
	c3 = ((unsigned char *)s)[2];
	if (isupper(c3))
		c3 = tolower(c3);
	value += (c3 >= '0' && c3 <= '9' ? c3 - '0' : c3 - 'a' + 10) * 16;
	printf("value33 is %d\n", value);
	c4 = ((unsigned char *)s)[3];
	if (isupper(c4))
		c4 = tolower(c4);
	value += c4 >= '0' && c4 <= '9' ? c4 - '0' : c4 - 'a' + 10;
	printf("value44 is %d\n", value);
	return (value);
}

void byte2hex(u_char ch, char *out)
{
	static const char *tab = "0123456789abcdef";
	out[0] = tab[ch >> 4];
	out[1] = tab[ch & 0x0f];
}

int seq_num_sort(struct my_tcptable_struct *a, struct my_tcptable_struct *b) {
	return strcmp(a->seq_num, b->seq_num);
}

void sort_by_seq_num(struct my_tcptable_struct *a) {
	HASH_SORT(a, seq_num_sort);
}

int dba_policy_insert(FILE * pFile){
	printf("insert policy!\n");
	pthread_mutex_lock(&mutex_policy);
	DL_FOREACH_SAFE(policy_head, policy_elt, policy_tmp){
		DL_DELETE(policy_head, policy_elt);
		free(policy_elt);
	}
	char mystring[2000];
	while ( fgets (mystring , 2000 , pFile) != NULL ){
		if(!memcmp(mystring,"[dba_rule_zcp]",13)){
			if ( (policy_name = (dba_policy_struct*)malloc(sizeof(dba_policy_struct))) != NULL){
				if(fgets (mystring , 2000 , pFile) != NULL)
				{	char policy_id[4];
					memcpy(policy_id,mystring+24,3);
					policy_name->id=atoi(policy_id);
					printf("policy_name->id:%d ",policy_name->id);
					memcpy(policy_name->key_name,mystring,10);
					memcpy(policy_name->key_ip,mystring+11,12);
					printf("policy_name->key_name:%s ",policy_name->key_name);
					printf("policy_name->key_ip:%s ",policy_name->key_ip);
					policy_name->log_type=atoi(mystring+29);
					printf("policy_name->log_type:%d ",policy_name->log_type);
					policy_name->action=atoi(mystring+32);
					policy_name->str_len=atoi(mystring+35);
					printf("policy_name->log_type:%d\n",policy_name->str_len);
				}
				const char *error;
    				int  erroffset;
				if(fgets (mystring , 2000 , pFile) != NULL)
				{	mystring[strlen(mystring)-1]=0x00;
					memcpy(policy_name->re_char,mystring,2000);
					policy_name->re= pcre_compile(policy_name->re_char, 0, &error, &erroffset, NULL);
				}
				if(fgets (mystring , 2000 , pFile) != NULL)
				{	mystring[strlen(mystring)-1]=0x00;
					memcpy(policy_name->replace,mystring,2000);
				}
				DL_APPEND(policy_head, policy_name);		
			}
		}
	}
	pthread_mutex_unlock(&mutex_policy);	
	return 1;
}

void* dba_policy_process()
{	char policy_file[50]="/dev/shm/policy.json";
	int policy_time=0;
	while(1)
	{	FILE * pFile;	
		pFile = fopen (policy_file, "r");
		if (pFile == NULL)	{	perror ("Error opening file");	sleep(1);	continue; }
		struct stat buf;
		int result;
		result =stat(policy_file, &buf );
		if( result != 0 )	{	perror( "error" );	sleep(1);	continue;	}		
		if(policy_time==0)
		{	policy_time=buf.st_mtime;
			dba_policy_insert(pFile);
		}
		if(policy_time==buf.st_mtime)
		{	//printf("nochange!!!\n");
		}
		else
		{	printf("policy change!!!!!\n");
			policy_time=buf.st_mtime;
			dba_policy_insert(pFile);
		}
		fclose (pFile);
		sleep(1);
	}
	return 0;
}

int add_rebuild_buf(int dirction, u_char * buf, int buf_length)
{
	if (dirction == 1)
	{
		memcpy(rebuild_buf1 + last_buf1, buf, buf_length);
		printf("rebuild_buf1 is %s\n\n", rebuild_buf1);
	}
	else if (dirction == 2)
	{
		memcpy(rebuild_buf2 + last_buf2, buf, buf_length);
		last_buf2 = last_buf2 + buf_length;
		printf("rebuild_buf2 is %s\n\n", rebuild_buf2);
	}

	return 0;
}


int dba_config_insert(char *mystring,FILE * pFile){
	//pthread_mutex_lock(&mutex_config);
	DL_FOREACH_SAFE(config_head,config_elt,config_tmp) {
		DL_DELETE(config_head,config_elt);
		free(config_elt);
	}
	//pthread_mutex_unlock(&mutex_config);	

	while ( fgets (mystring , 500 , pFile) != NULL )
	{
		if(!memcmp(mystring,"[db_content]",10))
		{
			while ( fgets (mystring , 300 , pFile) != NULL )
			{
				if(memcmp(mystring,"[/db_content]",10))
				{	if ( (config_name = (dba_config_struct*)malloc(sizeof(dba_config_struct))) != NULL)
					{	const char *error;
    						int  erroffset;					
						//pthread_mutex_lock(&mutex_config);
						config_name->dbip[0]=(u_char)waf_htoi(mystring);
						config_name->dbip[1]=(u_char)waf_htoi(mystring+2);
						config_name->dbip[2]=(u_char)waf_htoi(mystring+4);
						config_name->dbip[3]=(u_char)waf_htoi(mystring+6);

						config_name->db_port[0]=(u_char)waf_htoi(mystring+8);
						config_name->db_port[1]=(u_char)waf_htoi(mystring+10);
						memcpy(config_name->key_ipp,mystring,12);

						//config_name->status=atoi(mystring+13);
						memcpy(config_name->status, mystring+13, 2);

						config_name->db_type=atoi(mystring+16);
						config_name->if_custom=atoi(mystring+19);
						

						mystring[strlen(mystring)-1]=0x00;
						memcpy(config_name->custom_char, mystring + 22, 100);
						config_name->cus= pcre_compile(config_name->custom_char, 0, &error, &erroffset, NULL);

						memcpy(config_name->policy_open, mystring+19, 8);

						DL_APPEND(config_head, config_name);
						//pthread_mutex_unlock(&mutex_config);
					}
				}
				else{
					break;
				}
			}

		}
		if(!memcmp(mystring,"[db_userkey]",10))
		{
			while ( fgets (mystring , 300 , pFile) != NULL )
			{
				if(memcmp(mystring,"[/db_userkey]",10))
				{
					if ( (config_name = (dba_config_struct*)malloc(sizeof(dba_config_struct))) != NULL)
					{						
						//pthread_mutex_lock(&mutex_config);
						memcpy(config_name->userkey, mystring, 10);
						memcpy(config_name->status, mystring+11, 2);
						//config_name->status=atoi(mystring+11);

						DL_APPEND(config_head, config_name);
						//pthread_mutex_unlock(&mutex_config);
					}
				}
				else{
					break;
				}
			}

		}
	}
	return 1;
}

void* dba_config_process(){
	char config_file[50]="/dev/shm/dba_config.txt";
	int config_time=0;
	while(1){
		FILE * pFile;
		char mystring [501];
		pFile = fopen (config_file, "r");
		if (pFile == NULL){ perror ("Error opening file"); fclose (pFile); sleep(1); continue; }
		struct stat buf;
		int result;
		result =stat(config_file, &buf );
		if( result != 0 ){ perror( "error" ); fclose (pFile); sleep(1); continue; }

		if(config_time==0){
			config_time=buf.st_mtime;
			dba_config_insert(mystring,pFile);
		}
		if(config_time==buf.st_mtime){	}//printf("nochange!!!\n");
		else{
			printf("config change!!!!!\n");
			config_time=buf.st_mtime;
			dba_config_insert(mystring,pFile);	
		}
		fclose (pFile);
		sleep(1);
	}
	return 0;
}


int print_data( buffer *buf )
{
	int i_tmp;
	for ( i_tmp = 0; i_tmp < TCP_WINDOW; i_tmp++ )
	{
		printf( "0x%02x, ", *(buf+i_tmp) );
	}
	return 0;
}



void change(int num, char *str)
{
	int p = 0;
	int tmp = num;
	while(tmp)
	{
		p++;
		tmp /= 10;
	}
	if(num > 0)
		p--;
	tmp = num > 0 ? num : -num;
	//printf("tmp is %d, p is %d\n", tmp, p);
	while(tmp)
	{
		str[p] = (tmp%10) + '0';
		p--;
		tmp /= 10;
	}
	if( num < 0 )
		str[0] = '-';
}



int check_if_log( char *src, char* the_ip_port)
{	int  ovector[OVECCOUNT];
	int  custom_rc, i;
	int log_len=strlen(src);
	printf("11the_ip_port is %s\n", the_ip_port);
	//pthread_mutex_lock(&mutex_config2);
	DL_FOREACH(config_head,config_elt)
	{	//printf("config_elt->key_ipp is %s\n", config_elt->key_ipp);
		//printf("config_elt->if_custom is  %d\n", config_elt->if_custom);
		//if ( config_elt->if_custom == 0 )
		//{	
		//	pthread_mutex_unlock(&mutex_config2);
		//	return 1;//continue;
		//}
//printf("before cmp ipport\n");
printf("config_elt->key_ipp is %d\n", config_elt->key_ipp);
		if(!memcmp(config_elt->key_ipp, the_ip_port, 12))
		{	//printf("p3\n");
			if ( config_elt->if_custom == 0 )
			{	//no need
				//pthread_mutex_unlock(&mutex_config2);
				return 1;//continue;
			}
			if ( ( custom_rc = pcre_exec(config_elt->cus, NULL, src, strlen(src), 0, 0, ovector, OVECCOUNT)) != PCRE_ERROR_NOMATCH )
			{	printf("config access system_find_OK!!!!,key_ipp:%s,action:%s\n", config_elt->key_ipp, config_elt->custom_char);
				//pthread_mutex_unlock(&mutex_config2);
				return 1;	//record
			}
			else 
			{	//printf("no match\n");
				//pthread_mutex_unlock(&mutex_config2);
				return 0;
			}
		}
	}
	//pthread_mutex_unlock(&mutex_config2);
	return 0;
}

int check_list( const u_char * buf )
{
	int before_data_head = layer1_len + layer2_ip_len + layer3_udp_len;	//14+20+8=42
	buf_check_list = buf;
	//pthread_mutex_lock(&mutex_config);
	int count_bad = 0, dba_userkey = 0, sys_length = 0;
	int position_to_check = 0, sys_check_result = 0;
	char check_policy_src[2000] = "";
	char syslog_ip_port[12] = "\0";
	char sys_check_result_p[3] = "";
	char timestamp[10] = "";
	u_char sys_array[1600] = "\0";
	int print_char_n;


	DL_FOREACH(config_head,config_elt)
	{
		if ( config_elt->userkey[0] == buf_check_list[42] && config_elt->userkey[1] == buf_check_list[43] )
		{	//v2 check syslog head, the first two key serial.
			for (dba_userkey = 0; dba_userkey < 10; dba_userkey++)
			{	//after this, userkey is right, we can save the data
				if (  config_elt->userkey[dba_userkey] !=  buf_share[42+dba_userkey])
				{	printf("this is not a good syslog\n");
					return 0;
				}
			}
			if (memcmp(config_elt->status, status_on, 2) == 0)	//该条生效启用
			{	//v1 is in config line, v2 is in v2 config after userkey
				sys_length = buf_check_list[38]*256 + buf_check_list[39];	// sys_length is calculate here.

//3wnrfvkzdx 8 0 192.168.145.226 OSType16 000000000000|||c0a8930f05f1|||00|||00|||0|||true|||2020-01-17 07:37:28.2250757|||53|||0|||0|||WIN-9K6K2JE6MSA\Administrator|||123|||dbo.test|||select * from dbo.test
//1578667281|||02|||000000000000|||c0a8930f05f1|||00|||00|||0|||true|||2020-01-17 07:37:28.2250757|||53|||0|||0|||WIN-9K6K2JE6MSA\Administrator|||123|||dbo.test|||select * from dbo.test
//1578667281|||02|||000000000000|||000000000000|||c0a8930f05f1|||000000000000|||00|||00|||0|||true|||2020-01-17 07:37:28.2250757|||53|||0|||0|||WIN-9K6K2JE6MSA\Administrator|||123|||dbo.test|||select * from dbo.test

				change(time(&tim), timestamp);
				memcpy(syslog_ip_port, buf_check_list + before_data_head + 55, 12);

				memset(sys_content,'\0',sizeof(sys_content));
				memcpy(sys_content, timestamp, 10);		
				memcpy(sys_content+10, "||||", 3);
				//1578667281|||

				if(buf_share[42+11] == 0x37)			//mysql key 7 0 xxxxxx
				{	memcpy(sys_content+13, "01", 2);
					position_to_check = 110;
				}
				else if(buf_share[42+11] == 0x38)		//sqlserver key 8 0 xxxxxx
				{	memcpy(sys_content+13, "02", 2);
					position_to_check = 150;
				}
				else if(buf_share[42+11] == 0x39)		//oracle key 9 0 xxxxxx
				{	memcpy(sys_content+13, "03", 2);
					position_to_check = 160;
				}
				else							//unknow
				{	memset(sys_content,'\0',sizeof(sys_content));
					continue;					//don't need 00
				}
				memcpy(sys_content+15, "||||", 3);
//1578667281|||02|||000000000000|||000000000000|||c0a8930f05f1|||000000000000|||00|||00|||000|||true|||2020-01-17 07:37:28.2250757|||53|||0|||0|||WIN-9K6K2JE6MSA\Administrator|||123|||dbo.test|||select * from dbo.test
//0	    10   15 18		30 33	       45 48	      60 63	     75 78   83   88 91 94
				memcpy(sys_content+18, buf_check_list + before_data_head + 40, 15);	//visit_ipport|||
				memcpy(sys_content+33, key_ip_zero, 12);	//visit_mac
				memcpy(sys_content+45, "||||", 3);
				memcpy(sys_content+48, buf_check_list + before_data_head + 55, 15);	//db_ipport|||
				memcpy(sys_content+63, key_ip_zero, 12);	//db_mac
				memcpy(sys_content+75, "||||", 3);
				memcpy(sys_content+78, buf_check_list + before_data_head + 70, 10);	//two set position:00|||00|||

				//if sm version, 70 and 71 will be 01 or 02, normal version, that will be set 00, that is 0x30 in hex.
				if( *(buf_check_list + before_data_head+70) == 0x30 && *(buf_check_list + before_data_head+71) == 0x30 )	//00
				{	//printf("aaaa\n");
					memset(check_policy_src,'\0',sizeof(check_policy_src));
					memcpy(check_policy_src, buf_check_list + before_data_head + position_to_check, sys_length - position_to_check);
					sys_check_result = check_policy_dba( check_policy_src, syslog_ip_port, sys_length - position_to_check );
					//printf("sys_check_result is %03d\n", sys_check_result);
					sprintf(sys_check_result_p, "%03d", sys_check_result);
					memcpy(sys_content+88, sys_check_result_p, 3);
				}
				else	//01 or 02 or 03
				{	//in else means sm
					memcpy(sys_content+88, buf_check_list + before_data_head + 80, 3);
				}
//1578667281|||02|||000000000000|||000000000000|||c0a8930f05f1|||000000000000|||00|||00|||000|||true|||2020-01-17 07:37:28.2250757|||53|||0|||0|||WIN-9K6K2JE6MSA\Administrator|||123|||dbo.test|||select * from dbo.test
//0	    10   15 18		30 33	       45 48	      60 63	     75 78   83   88 91 94
				
				memcpy(sys_content+91, buf_check_list + before_data_head + 83, sys_length - 83 - layer3_udp_len);
				//just done check policy, no.125 byte
				//memcpy(sys_content+91+(sys_length - 83 - layer3_udp_len), "\n", 1);
		//printf("a7\n");
				memset(sys_array,'\0',sizeof(sys_array));
				print_char_n = sprintf( sys_array, "%s", sys_content );	//print_char_n is the length of sys_array. with \n
		printf("print_char_n is %d\n", print_char_n);
				//print_buf( sys_array, print_char_n, &syslog_buffer );
				//print_char_n = sprintf( sys_array, "\n" );
				print_buf( sys_array, print_char_n, &syslog_buffer );
				print_buf("\n",strlen("\n"),&syslog_buffer);
				printf("syslog_buffer is %s\n", sys_array);
			}
			else
			{	count_bad++;
			}
			return 0;
		}
	}
	return 0;
}


void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * buf)	
{
	int i, dir = 0;
	int check_rebuild_result = 0;
	int dbtype = 0;
	int direction = 0;
	sql_len = 0;
	//u_char sql_buf[10000];
	buf_share = buf;
	pkt_len_share = pkthdr->caplen; 

	if ( buf_share[12]==0x08 && buf_share[13]==0x00 )	//ip packet
	{
		if ( buf_share[23]==0x11 && buf_share[36]==0x02 && buf_share[37]==0x02 )	//udp and 514
		{	//printf("\nbegin:514udp syslog\n\n");
			count_all++;
			printf("count_all is %d\n", count_all);
			check_list(buf);
			/*if ( check_list(buf) == 0 )
			{
				//printf("packet is 0\n");
			}
			else
			{
				//printf("not 0\n");
			}*/
		}
	}
}


int check_policy_dba( char *src, char* the_ip_port, int src_len)
{	int  ovector[OVECCOUNT];
	int  rc, i;

	DL_FOREACH(policy_head, policy_elt)
	{	if(!memcmp(policy_elt->key_ip, the_ip_port, 12) || !memcmp(policy_elt->key_ip,key_ip_zero,12))
		{	//printf("p3\n");
			if ( ( rc = pcre_exec(policy_elt->re, NULL, src, src_len, 0, 0, ovector, OVECCOUNT)) != PCRE_ERROR_NOMATCH )
			{	printf("policy access system_find_OK!!!!,id:%d,action:%d\n",policy_elt->id,policy_elt->action);
				if (policy_elt->action == 1)
				{
					return policy_elt->id;
				}
				else if (policy_elt->action == 0)
				{
					return policy_elt->action;
				}
			}
			else 
			{
				//printf("no match\n");
			}
		}
	}
	return 0;
}




int main(int argc, char *argv[])
{
	char errBuf[PCAP_ERRBUF_SIZE] = {0}, *devStr;
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int id = 0;
	char errbuf[PCAP_ERRBUF_SIZE] = {0};
	char *net_work = argv[1];

	init_buffer( &syslog_buffer );
	set_timer(10);
	
	pthread_t dba_config_process1;
	pthread_create(&dba_config_process1,NULL,dba_config_process,NULL);
	pthread_detach(dba_config_process1);

	pthread_t dba_policy_process1;
	pthread_create(&dba_policy_process1, NULL, dba_policy_process,NULL);
	pthread_detach(dba_policy_process1);


	sleep(1);
	//printf("why not printf\n");
	//pthread_mutex_lock(&mutex_config);
	DL_FOREACH(config_head,config_elt){		
		printf("config_elt->dbip:%02x%02x%02x%02x, port:%02x%02x, status:%d, db_type:%d\n", config_elt->dbip[0], config_elt->dbip[1], config_elt->dbip[2], config_elt->dbip[3], config_elt->db_port[0], config_elt->db_port[1], config_elt->status, config_elt->db_type);
		printf("config_name->userkey is %s, config_name->staus is %d\n", config_elt->userkey, config_elt->status);
	}
	//pthread_mutex_unlock(&mutex_config);

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
		exit(1);
	}
	devStr = net_work;
	if(devStr)
	{
		printf("success: device: %s\n", devStr);
	}
	else
	{
		printf("error: %s\n", errbuf); 
		exit(1);
	}
	/* open a device, wait until a packet arrives */
	pcap_t * device = pcap_open_live(devStr, 65535, "port 514", 0, errbuf);

	if(!device)
	{
		printf("error: pcap_open_live(): %s\n", errbuf);
		exit(1);
	}

	/* construct a filter */
	struct bpf_program filter;
	//pcap_compile(device, &filter, "dst port 80", 1, 0);
	pcap_compile(device, &filter, "", 1, 0);
	pcap_setfilter(device, &filter);
	/* wait loop forever */
	
	pcap_loop(device, -1, getPacket, (u_char*)&id);

	pcap_close(device);

	return 0;
}
