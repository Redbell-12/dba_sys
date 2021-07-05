#include "mysql_aya.h"
#include "../buffer/buffer.h"

//长度指示器就是指示长度的token，长度编码的长度指示器就是此token本身的长度就是不固定的，这样其可以指示更大范围的长度
//此函数接受一个长度编码指示器，返回其所指示的长度和它本身所占的字节数
NUMBER_AND_BYTES length_coded_analysis(u_char *p)
{
        if((*p)<251){
                number_and_bytes.number=*p;
                number_and_bytes.bytes=1;
        }else if((*p)==0xfb){
                number_and_bytes.number=0;
                number_and_bytes.bytes=1;
        }else if((*p)==0xfc){
                number_and_bytes.number=*((short *)(p+1));//the number in next 2 bytes
                number_and_bytes.bytes=1+2;
        }else if((*p)==0xfd){
                number_and_bytes.number=(*((int *)(p+1)))&0xffffff;//the number in next 3 bytes
                number_and_bytes.bytes=1+3;
        }else{
                number_and_bytes.number=*((long *)(p+1));//the number in next 8 bytes
                number_and_bytes.bytes=1+8;
        }
        return number_and_bytes;
}
void field_analysis(u_char * field_body,buffer *buf)
{
        int i;
        number_and_bytes=length_coded_analysis(field_body);
        field_body=field_body+number_and_bytes.bytes+number_and_bytes.number;

        number_and_bytes=length_coded_analysis(field_body);
        field_body=field_body+number_and_bytes.bytes;
	if(number_and_bytes.number==0){
		print_buf("null",4,buf);
	}else{
		print_buf(field_body,number_and_bytes.number,buf);
	}
	print_buf("|||",3,buf);
	field_body=field_body+number_and_bytes.number;
        number_and_bytes=length_coded_analysis(field_body);
        field_body=field_body+number_and_bytes.bytes;
	if(number_and_bytes.number==0){
		print_buf("null",4,buf);
	}else{
		print_buf(field_body,number_and_bytes.number,buf);
	}
}

void rowdata_analysis(u_char *rowdata_body,long text_number,buffer *buf)
{
	int i;
	for(i=0;i<text_number;i++){
		number_and_bytes=length_coded_analysis(rowdata_body);	
		rowdata_body=rowdata_body+number_and_bytes.bytes;

		print_buf(rowdata_body,number_and_bytes.number,buf);

		rowdata_body=rowdata_body+number_and_bytes.number;//rowdata_body指针移到下一个text
		if(i!=text_number-1){
			print_buf("&&&",3,buf);
			//fprintf(fp,"&&&");//字段分隔符
		}else{
			print_buf("$$$",3,buf);
			//fprintf(fp,"$$$");//记录分隔符
		}
	}
}
int if_eof(u_char * p)
{
	if((*(p+4)==0xfe)&&(((*(int *)p)&0xffffff)<=5))
		return 0;//是eof报文
	return 1;//不是eof报文
}
/*result_set报文*/     
int result_set(u_char *result,buffer *buf)//result指向resultset报文的numberoffield字段（token）,此token指示有多少个field报文,其本身是长度编码的
{
	u_char *field_packet;
	u_char *eof_packet;
	u_char *rowdata_packet;
        unsigned long field_number;
        unsigned long text_number;
	/*计算result指向的长度指示器token所指示的长度以及其本身所占的字节数*/
	number_and_bytes=length_coded_analysis(result);
	text_number=field_number=number_and_bytes.number;//text_number用于下面分析row data报文
	field_packet=result+number_and_bytes.bytes;
printf("111111111111111\n");
	/*跳过所有field*/	
	while(field_number>0){
		field_packet=field_packet+4+((*((int*)field_packet))&0xffffff);
		field_number--;
	}

printf("2222222222222222222\n");
	/*此时field_packet指向eof报文*/
	/*这里分析eof报文*/
	rowdata_packet=field_packet+9;//暂不分析直接跳过
	/*这里分析row data报文*/
	/*row data报文的个数并无明确指示，只是用eof报文作为结束，这就要求我们每次分析输出row data报文的时候先判断有没有到eof报文，如果到了则
 	row data报文分析结束。*/
	/*一个row data报文内部的text个数等于字段field的个数，所以我们根据field_number循环打印field_number个text*/
printf("55555555555555\n");
	while(if_eof(rowdata_packet)){
		rowdata_analysis(rowdata_packet+4,text_number,buf);
		rowdata_packet=rowdata_packet+4+((*((int*)rowdata_packet))&0xffffff);
	}

}
/*ok报文*/
int ok(u_char *ok)
{
	//ok指向受影响行数字段
	unsigned long affected_row=0;
	if((*ok)<251){
		affected_row=*ok;
	}else if((*ok)==0xfc){
		affected_row=*((short *)(ok+1));//the number in next 2 bytes
	}else if((*ok)==0xfd){
		affected_row=(*((int *)(ok+1)))&0xffffff;//the number in next 3 bytes
	}else{
		affected_row=*((long *)(ok+1));//the number in next 8 bytes
	}
	//fprintf(fp,"受影响行数：%ld\n",affected_row);
	
}

/*err报文*/
int error(u_char *err)
{
	/*err指向错误编号*/
	//fprintf(fp,"错误编号：%d",*((short *)err));
}
