#include"redis.h"
//解析一个$报文，返回这个报文的数据字符串和长度
data bulk(char *bulk)
{
	//计算报文的数据长度
	long len=0;
	bulk++;
	while(!((*bulk==0x0d)&&(*(bulk+1)==0x0a))){
		len=(len*10)+(*bulk-'0');
		bulk++;
	}
	bulk+=2;
	//将报文数据放入字符串
	char *p=malloc(len+1);
	int i;
	for(i=0;i<len;i++){
		*(p+i)=*bulk;
		bulk++;
	}
	*(p+i)='\0';
	
	data da;
	da.data=p;
	da.len=len;
	return da;
}
//+-:三合一
data status_error_integer(char *status)
{
	char *status_cpy=status;
	status++;
	status_cpy++;
	int len=0;
	while(!((*status_cpy==0x0d)&&(*(status_cpy+1)==0x0a))){
		len++;
		status_cpy++;
	}	
	//将报文数据放入字符串
	char *p=malloc(len+1);
        int i;
        for(i=0;i<len;i++){
                *(p+i)=*status;
                status++;
        }
        *(p+i)='\0';
	
	data da;
	da.data=p;
	da.len=len;
	return da;
}
