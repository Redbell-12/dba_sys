#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void * sql_std(u_char *arr,int len)
{
	//将注释用空格替换*/
	int i;
	u_char *p,*q,*work;
	for(i=0;i<len;i++){
		if(arr[i]==0x2f&&arr[i+1]==0x2a){
			p=arr+i;
			work=arr+i;
			while(!(*(work+2)==0x2a&&*(work+3)==0x2f))
				work++;	
			q=work+3;
			while(p!=q){
				*p=' ';
				p++;
			}*p=' ';
		}
	}
	//将换行符用空格替换
	for(i=0;i<len;i++){
		if(arr[i]==0x0d&&arr[i+1]==0x0a)
			arr[i]=arr[i+1]=' ';
		if(arr[i]==0x0a)
			arr[i]=' ';
	}
	//去掉多余的空格，得到最后的规范格式
	u_char * arr_new=(u_char *)malloc(len+1);
	i=0;
	int j=0;
	int flag=0;
	while(arr[i]==' ')
                i++;	
	for(;i<len;i++){
		if(arr[i]!=' '){
			arr_new[j]=arr[i];
			j++;
			flag=1;
		}else if(arr[i]==' '){
			if(flag==1){
				arr_new[j]=' ';
				j++;
				flag=0;
			}
		}
	}
	arr_new[j]='\0';
	return arr_new;
}
/*判断用户输入的sql语句中的命令类型是否合法*/
int is_legal(const u_char *com,int len)
{
	/*所有命令类型*/
	u_char *all_com_type[]={
	"select",
	"update",
	"delete",
	"insert",
	"create",
	"drop",
	"use",
	"alter",
	NULL
	};

	u_char com_type[len];
	int i=0;
	while((*com)!=' '&&(*com)!='\0'){
		com_type[i]=*com;	
		com++;
		i++;
	}com_type[i]='\0'; 
	/*将命令类型字符串转换成小写*/	
	for(i=0;i<strlen(com_type);i++){
                com_type[i]=tolower(com_type[i]);
        }
	/*将用户输入的sql中的命令类型与所有合法的命令类型比较*/
	for(i=0;all_com_type[i]!=NULL&&strcmp(com_type,all_com_type[i]);i++){
		;
	}
	if(all_com_type[i]==NULL)
		return 1;//没有匹配到
	else
		return 0;//匹配到了
}
