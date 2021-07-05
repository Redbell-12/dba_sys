#include "tds.h"
void * sql_std(u_char *,int);
void *sql_batch_request(u_char *payload)
{
        int i,j=0;
        unsigned short sql_length=ntohs(*(short *)(payload+2))-(*(int *)(payload+8))-8;
        u_char *sql_text=payload+8+(*(int *)(payload+8));

	u_char temp[sql_length];
        for(i=0;i<sql_length;i+=2){
		temp[j++]=sql_text[i];
        }
	//规范化
	return sql_std(temp,sql_length/2);
}
void rpc_request(u_char *payload)
{

}
