#include "tds.h"
#include "../buffer/buffer.h"

TIME * convert_smalldatetime(const void *);
TIME * convert_datetime(const void *);
TIME * convert_date(const void *);
TIME * convert_time(const u_char *,int);
TIME * convert_datetime2(const u_char *,int);
double convert_decimal_numeric(const u_char *,int);

/*
 * 接受一个指向field的指针和数据类型，输出出field内容，返回field长度（如果有指示器连同指示器）以便上层函数利用长度来跳到下一个field
 */
int ms_field_analysis(u_char *field,int datatype,int scale,buffer *buf)
{
	int n;
	char array[100];
	if(datatype==127){
		//fprintf(fp,"%ld",(*(long *)field));
		n=sprintf(array,"%ld",(*(long *)field));
		print_buf(array,n,buf);
		return 8;
	}else if(datatype==56){
		//fprintf(fp,"%d",(*(int *)field));
		n=sprintf(array,"%d",(*(int *)field));
		print_buf(array,n,buf);
		return 4;
	}else if(datatype==52){
		//fprintf(fp,"%hd",(*(short *)field));
		n=sprintf(array,"%hd",(*(short *)field));
		print_buf(array,n,buf);
		return 2;
	}else if(datatype==48){
		n=sprintf(array,"%d",(*(char *)field));
		print_buf(array,n,buf);
		return 1;
	}else if(datatype==60){
		n=sprintf(array,"%ld",(*(long *)field));//money需要转换成金钱的数字格式
		print_buf(array,n,buf);
		return 8;
	}else if(datatype==122){//smallmoney
		float smallmoney=((float)(*(int *)(field)))/10000;
		n=sprintf(array,"%f",smallmoney);
		print_buf(array,n,buf);
		return 4;
	}else if(datatype==61){//datetime
		TIME *t=convert_datetime(field);
		n=sprintf(array,"%u/%u/%u %u:%u:%u\n",t->year,t->mon,t->day,t->hour,t->min,t->sec);
		print_buf(array,n,buf);
		return 8;
	}else if(datatype==50){
		n=sprintf(array,"%d",(*(u_char *)field));
		print_buf(array,n,buf);
		return 1;
	}else if(datatype==62){
		n=sprintf(array,"%f",(*(double *)field));//这里的float就是c中的double，所以解析此类型数据时直接用一个double类型的变量接就可以了
		print_buf(array,n,buf);
		return 8;
	}else if(datatype==59){
		n=sprintf(array,"%f",(*(float *)field));//这里的real就是c中的float，所以编程解析此类型数据时直接用一个float变量接就可以了
		print_buf(array,n,buf);
		return 4;
	}else if(datatype==58){
		TIME *t=convert_smalldatetime(field);
		n=sprintf(array,"%u/%u/%u %u:%u",t->year,t->mon,t->day,t->hour,t->min);
		print_buf(array,n,buf);
		return 4;
//以上是没有长度指示器的部分
	}else if(datatype==38){
		if(*field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else if(*field==8){
			n=sprintf(array,"%ld",(*(long *)(field+1)));
			print_buf(array,n,buf);
			return 1+8;
		}else if(*field==4){
			n=sprintf(array,"%d",(*(int *)(field+1)));
			print_buf(array,n,buf);
			return 1+4;
		}else if(*field==2){
			n=sprintf(array,"%hd",(*(short *)(field+1)));
			print_buf(array,n,buf);
			return 1+2;
		}else{//(*field==1)
			n=sprintf(array,"%d",(*(u_char *)(field+1)));
			print_buf(array,n,buf);
			return 1+1;
		}
	}else if(datatype==110){//需要转换成金钱格式
		if(*field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else if(*field==8){//money
                        n=sprintf(array,"%ld",(*(long *)(field+1)));
			print_buf(array,n,buf);
			return 1+8;
		}else{//(*field==4)//smallmoney
			float smallmoney=((float)(*(int *)(field+1)))/10000;
                        n=sprintf(array,"%f",smallmoney);
			print_buf(array,n,buf);
			return 1+4;
		}
	}else if(datatype==111){
		if(*field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else if(*field==8){//datetime
			TIME *t=convert_datetime(field+1);
			n=sprintf(array,"%u/%u/%u %u:%u:%u",t->year,t->mon,t->day,t->hour,t->min,t->sec);
			print_buf(array,n,buf);
			return 1+8;
		}else{//*field==4//smalldatetime
			TIME *t=convert_smalldatetime(field+1);
			n=sprintf(array,"%u/%u/%u %u:%u:%u",t->year,t->mon,t->day,t->hour,t->min,t->sec);
			print_buf(array,n,buf);
                        return 1+4;
		}
	}else if(datatype==104){
		if(*field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else{//(*field==1)
                        n=sprintf(array,"%d",(*(u_char *)(field+1)));
			print_buf(array,n,buf);
			return 1+1;
		}
	}else if(datatype==109){//有长度指示器的情况，float和real都是109
                if(*field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else if(*field==8){
                        n=sprintf(array,"%f",(*(double *)(field+1)));
			print_buf(array,n,buf);
			return 1+8;
		}else{//(*field==4)
                        n=sprintf(array,"%f",(*(float *)(field+1)));
			print_buf(array,n,buf);
			return 1+4;
		}
////
	}else if(datatype==40){//date
		if(*field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else{
			TIME *t=convert_date(field+1);
			n=sprintf(array,"%u/%u/%u",t->year,t->mon,t->day);
			print_buf(array,n,buf);
			return *field+1;
		}
	}else if(datatype==41){//time
		if(*field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else{
			TIME *t=convert_time(field,scale);//此处传入的的是field而非field+1，因为convert_time要用到*field
			n=sprintf(array,"%u:%u:%u.%u",t->hour,t->min,t->sec,t->last);
			print_buf(array,n,buf);
			return *field+1;
		}
	}else if(datatype==42){//datetime2
		if(*field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else{
			TIME *t=convert_datetime2(field,scale);
			n=sprintf(array,"%u/%u/%u %u:%u:%u.%u",t->year,t->mon,t->day,t->hour,t->min,t->sec,t->last);
			print_buf(array,n,buf);
			return *field+1;
		}
	}else if(datatype==43){//datetimeoffset
		if(*field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else{
			n=sprintf(array,"datetimeoffset");//此处解析datetimeoffset类型数据
			print_buf(array,n,buf);
			return *field+1;
		}
	}else if(datatype==106||datatype==108){//decimal和numeric
		if(*field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 1;
		}else{
			double f=convert_decimal_numeric(field,scale);
			if(*(field+1)==0){
				n=sprintf(array,"-%f",f);
				print_buf(array,n,buf);
			}else{
				n=sprintf(array,"%f",f);
				print_buf(array,n,buf);
			}
			return *field+1;
		}
//以下长度指示器变成2字节
	}else if((datatype==165)&&(scale!=0xffff)){//varbinary
		if(*(short *)field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 2;
		}else{
			n=sprintf(array,"BLOB");
			print_buf(array,n,buf);
			return *(short *)field+2;
		}
	}else if((datatype==165)&&(scale==0xffff)){//varbinary(MAX)
		//解析varbinary(MAX)类型的field
		if(*(long *)field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
                        return 12;
		}else{
			n=sprintf(array,"BLOB");//有用的就这一句，至于下面的循环过程还要走是为了累加出chunk_number，从而计算出此field的长度返回
			print_buf(array,n,buf);
			int i;
			int chunk_number=0;
			u_char *p=field+8;
			u_char *q;
			while((*(int *)p)!=0){
				q=p+4;
				for(i=0;i<(*(int *)p);i++){
					q++;
				}
				p+=((*(int *)p)+4);
				chunk_number++;
			}
			chunk_number++;
			return *(long *)field+chunk_number*4+8;
		}	
	}else if(datatype==173){//binary
		if(*(short *)field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 2;
		}else{
			n=sprintf(array,"BLOB");
			print_buf(array,n,buf);
			return *(short *)field+2;
		}
	}else if(datatype==175){//char
		if(*(short *)field==0||*(short *)field==0xffff){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 2;
		}else{
			int i;
			u_char *p=field+2;
			for(i=0;i<(*(short *)field);i++){
				n=sprintf(array,"%c",*p);
				print_buf(array,n,buf);
				p++;
			}
			return *(short *)field+2;
		}
	}else if((datatype==167)&&(scale!=0xffff)){//varchar
		if(*(short *)field==0||*(short *)field==0xffff){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 2;
		}else{
			int i;
			u_char *p=field+2;
			for(i=0;i<(*(short *)field);i++){
				n=sprintf(array,"%c",*p);
				print_buf(array,n,buf);
				p++;
			}
			return *(short *)field+2;
		}
	}else if((datatype==167)&&(scale==0xffff)){//varchar(MAX)
		if(*(long *)field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
                        return 12;
		}else{
			int i;
			int chunk_number=0;
			u_char *p=field+8;
			u_char *q;
			while((*(int *)p)!=0){
				q=p+4;
				for(i=0;i<(*(int *)p);i++){
					n=sprintf(array,"%c",*q);
					print_buf(array,n,buf);
					q++;
				}
				p+=((*(int *)p)+4);
				chunk_number++;
			}
			chunk_number++;
			return *(long *)field+chunk_number*4+8;
		}	
	}else if(datatype==239){//nchar
		if(*(short *)field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 2;
		}else{
			int i;
			u_char *p=field+2;
			for(i=0;i<(*(short *)field);i+=2){
				n=sprintf(array,"%c",*p);
				print_buf(array,n,buf);
				p+=2;
			}
			return *(short *)field+2;
		}
	}else if((datatype==231)&&(scale!=0xffff)){//nvarchar或sysname
		if(*(short *)field==0){
			n=sprintf(array,"null");
			print_buf(array,n,buf);
			return 2;
		}else{
			int i;
			u_char *p=field+2;
			for(i=0;i<(*(short *)field);i+=2){
				n=sprintf(array,"%c",*p);
				print_buf(array,n,buf);
				p+=2;
			}
			return *(short *)field+2;
		}
	}else if((datatype==231)&&(scale==0xffff)){//nvarchar(MAX)
		if(*(long *)field==0){
                        n=sprintf(array,"null");
			print_buf(array,n,buf);
                        return 12;
		}else{
			int i;
			int chunk_number=0;
			u_char *p=field+8;
			u_char *q;
			while((*(int *)p)!=0){
				q=p+4;
				for(i=0;i<(*(int *)p);i+=2){
					n=sprintf(array,"%c",*q);
					print_buf(array,n,buf);
					q+=2;
				}
				p+=((*(int *)p)+4);
				chunk_number++;
			}
			chunk_number++;
			return *(long *)field+chunk_number*4+8;//注意这里返回field的长度利用了结构的头8个字节，但这8个字节靠不住，
		}					       //有时明明有内容，但其会返回-2，说是unknown_plp_length
	}else if(datatype==241){//xml，和3个MAX的格式是一样的。//此类型解析时对293行描述的问题做了改进，没有依赖结构的头8个字节给出的长度。
		if(*(long *)field==0){                         //而是自己定义length变量累加出所有plp chunk的长度。如果其他三个MAX出了问题
                        n=sprintf(array,"null");    		       //也依照这里改进
			print_buf(array,n,buf);
                        return 12;
		}else{
			int i;
			int chunk_number=0;
			u_char *p=field+8;
			u_char *q;
			long length=0;
			while((*(int *)p)!=0){
				length+=*(int *)p;
				q=p+4;
				for(i=0;i<(*(int *)p);i+=2){
					n=sprintf(array,"%c",*q);
					print_buf(array,n,buf);
					q+=2;
				}
				p+=((*(int *)p)+4);
				chunk_number++;
			}
			chunk_number++;
			return length+chunk_number*4+8;
		}	
	//很特别的4个row格式解析		
	}else if(datatype==34||datatype==35||datatype==98||datatype==99){
		int length=*(int *)(field+1+(*(char *)field)+8);
		int i;
		u_char *p=field+1+(*(char *)field)+8+4;
		if(length!=0){
			for(i=0;i<length;i++){	
				n=sprintf(array,"%c",*p);
				print_buf(array,n,buf);
				if(datatype==99){
					p+=2;
				}else{
					p++;
				}
			}
		}else{
			n=sprintf(array,"null");
			print_buf(array,n,buf);
		}
		return 1+(*(char *)field)+8+4+(*(int *)(field+1+(*(char *)field)+8));
	}
}
