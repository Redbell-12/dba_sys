#include "tds.h"
static TIME t;
/*此函数只有convert_time使用*/
static int mult10(int n){
	int sum=10,m;
	if(n==0)return 1;
	for(m=1;m<n;m++)sum*=10;
	return sum;
}
TIME * convert_smalldatetime(const void *p)
{
	unsigned short year_mon_day=*(short *)p;
	t.year=1900+year_mon_day/365;
	t.mon=year_mon_day%365/30;
	t.day=year_mon_day%365%30;
	unsigned short hour_min=*(short *)(p+2);
	t.hour=hour_min/60;
	t.min=hour_min%60;
	return &t;
}
/*本函数要求处理的时间不早于1900年，1753到1900年的时间没有实现*/
TIME * convert_datetime(const void *p)
{
	/*年月日*/
	unsigned int year_mon_day=*(int *)p;
	t.year=1900+year_mon_day/365;
	t.mon=year_mon_day%365/30;
	t.day=year_mon_day%365%30;
	/*时分秒*/
	unsigned int hour_min_sec=(*(int *)(p+4))/300;//官方手册上规定要先除以300，不知有何意义
	t.hour=hour_min_sec/3600;
	t.min=hour_min_sec%3600/60;
	t.sec=hour_min_sec%3600%60;
	return &t;
}
TIME * convert_date(const void *p)
{
	unsigned int year_mon_day=*(int *)p;
	year_mon_day&=0xffffff;
	t.year=year_mon_day/365;
	t.mon=year_mon_day%365/30;
	t.day=year_mon_day%365%30;
	return &t;
}
TIME * convert_time(const u_char *p,int scale)
{
	double field_val;
	if(0<=scale&&scale<=2){//占3个字节
		field_val=(*(int *)(p+1))&0xffffff;
	}else if(3<=scale&&scale<=4){//占4个字节
		field_val=*(int *)(p+1);
	}else if(5<=scale&&scale<=7){//占5个字节
		field_val=(*(long *)(p+1))&0xffffffffff;
	}
	int hour_min_sec=(int)(field_val/mult10(scale));	
	t.hour=hour_min_sec/3600;
	t.min=hour_min_sec%3600/60;
	t.sec=hour_min_sec%3600%60;
	t.last=(int)((field_val/mult10(scale)-(double)hour_min_sec)*mult10(scale));
	return &t;
}	
/*此类型是date和time的组合*/
TIME * convert_datetime2(const u_char *p,int scale)
{
	convert_time(p,scale);
	p+=(*p-3+1);//指针移到日期部分
	convert_date(p);
	return &t;
}
	
/*此类型在datetime2的基础上加上了时区的概念*/
TIME * convert_datetimeoffset(const void *p)
{
	
}

/*decimal或numeric*/
double convert_decimal_numeric(const u_char *p,int scale)
{
	/**p可等于5 9 13 17即数据长度就是4 8 12 16四种情况，
        还有一个字节是用来指示正负的，0是负，1是正
	把这些长度的值取出来除以10的scale次方就是最终结果	
	现在的问题是4 8好取用int和long就可以了，但12字节和16字节的怎么取？*/
	double decimal_numeric=0.0;
	if(*p==5){
		decimal_numeric=(float)*(int *)(p+2);
		decimal_numeric=decimal_numeric/mult10(scale);
	}else if(*p==9){
		decimal_numeric=(double)*(long *)(p+2);
		decimal_numeric=decimal_numeric/mult10(scale);
	}
	return decimal_numeric;
}
/*
int main()
{
	char arr[]={0x05,0x01,0x10,0x5c,0xc8,0x0d};
	double f=convert_decimal_numeric(arr,7);
	//printf("%u/%u/%u %u:%u:%u\n",t->year,t->mon,t->day,t->hour,t->min,t->sec);
	printf("%f\n",f);
}
*/
