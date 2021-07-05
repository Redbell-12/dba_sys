#include "tds.h"

/*此函数根据某个column的数据类型给出此column的总长度*/
int datatype_length(const u_char *datatype)
{
	if((*datatype)==165||(*datatype)==173)
		return 7+2+1+(*(datatype+3))*2;
	else if((*datatype)==167||(*datatype)==175||(*datatype)==231||(*datatype)==239)
		return 7+7+1+(*(datatype+8))*2;
	else if((*datatype)==106||(*datatype)==108)
		return 7+3+1+(*(datatype+4))*2;
	else if((*datatype)==36||(*datatype)==38||(*datatype)==41||(*datatype)==42
		||(*datatype)==43||(*datatype)==104||(*datatype)==109||(*datatype)==110||(*datatype)==111)
		return 7+1+1+(*(datatype+2))*2;
	else if((*datatype)==40||(*datatype)==56||(*datatype)==127||(*datatype)==48||(*datatype)==52
		||(*datatype)==50||(*datatype)==62||(*datatype)==59||(*datatype)==60||(*datatype)==122
		||(*datatype)==61||(*datatype)==58)
		return 7+0+1+(*(datatype+1))*2;
	else if((*datatype)==98)
		return 7+4+1+(*(datatype+5))*2;
	else if((*datatype)==241)//这一支为什么不归入第4分支呢？因为特殊
		return 7+1+1+(*(datatype+2))*2;
	else if((*datatype)==240){
		u_char *p=(u_char *)datatype+3;
		int dbname_len=(*p)*2+1;	
		p=p+dbname_len;
		int schemaname_len=(*p)*2+1;
		p=p+schemaname_len;
		int typename_len=(*p)*2+1;
		p=p+typename_len;
		int ass_qua_name_len=(*(short *)p)*2+2;
		p=p+ass_qua_name_len;
		int column_name_len=(*p)*2+1;
		return 7+2+dbname_len+schemaname_len+typename_len+ass_qua_name_len+column_name_len;
	}else if((*datatype)==34){
		u_char *p=(u_char *)datatype+6;
		int tb_name_len=(*(short *)p)*2+2;
		p=p+tb_name_len;
		int tb_name2_len=(*(short *)p)*2+2;
		p=p+tb_name2_len;
		int column_name_len=(*p)*2+1;
		return 7+4+1+tb_name_len+tb_name2_len+column_name_len;
	}else if((*datatype)==35||(*datatype)==99){
		u_char *p=(u_char *)datatype+11;
                int tb_name_len=(*(short *)p)*2+2;
                p=p+tb_name_len;
                int tb_name2_len=(*(short *)p)*2+2;
                p=p+tb_name2_len;
                int column_name_len=(*p)*2+1; 	
		return 7+4+2+2+1+1+tb_name_len+tb_name2_len+column_name_len;
	}
}
/*int main(){
	char arr[]={0x00,0x00,0x00,0x00,0x09,0x00,0x63,0xfe,0xff,0xff,0x7f,0x04,0x08,0xd0,0x00,0x00,0x02,0x03,0x00,0x64,0x00,0x62,0x00,0x6f,0x00,0x0c,0x00,0x64,0x00,0x61,0x00,0x74,0x00,0x61,0x00,0x74,0x00,0x79,0x00,0x70,0x00,0x65,0x00,0x5f,0x00,0x61,0x00,0x6c,0x00,0x6c,0x00,0x05,0x6e,0x00,0x74,0x00,0x65,0x00,0x78,0x00,0x74,0x00};
	char arr1[]={0x00,0x00,0x00,0x00,0x09,0x00,0x6f,0x08,0x08,0x64,0x00,0x61,0x00,0x74,0x00,0x65,0x00,0x74,0x00,0x69,0x00,0x6d,0x00,0x65,0x00};
	printf("%d\n",datatype_length(arr1+6));
}*/
