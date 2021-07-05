#include "tds.h"
#include "../buffer/buffer.h"
/*此函数不用*/
void analysis_column(u_char *column)
{
	int i;
	int column_name_length=*(column+14);
	u_char *column_name=column+14+1;
	for(i=0;i<column_name_length;i++){
		printf("%c",*column_name);
		column_name+=2;
	}
	printf("\n");
}
/*
 * 解析一个row报文，输出其中的各个field值，由于sqlserver中，field类型不同解析方法就不同
 * 所以analysis_row在解析每个field时要实时参照当前列的类型然后决定解析策略，所以analysis_row
 * 函数接受三个参数：row报文，记载了各列的类型的数组，列数。另外由于报文中没有给出一个row的长度
 * 所以analysis_row在解析一个row的同时计算出此row的长度返回给调用者。
 */
long analysis_row(u_char *row,const COLUMN *column_attribute,int column_number,buffer *buf)
{
	//row指向row报文的数据部分--第一个field，准备解析：先从column_attribute
	//数组中获得此field的类型，然后调用ms_field_analysis函数，将类型和field指针传给它，它就会解析并输出
	//field的内容，然后ms_field_analysis返回这个field长度，这里就利用这个长度使row跳向下一个field，然后再次调用
	//ms_field_analysis函数解析出下一个field内容，如此循环column_number次便解析出了一个row中所有的field
	int field_length,i;
	long row_length=0;//用于累计当前解析的row的总长度
	for(i=0;i<column_number;i++){
		field_length=ms_field_analysis(row,column_attribute[i].datatype,column_attribute[i].scale,buf);
		row+=field_length;
		row_length+=field_length;
		if(i!=column_number-1)
			print_buf("&&&",3,buf);
		else 
			print_buf("$$$",3,buf);
	}
	return row_length;
}
/*解析一个nbcrow报文*/
long analysis_nbcrow(u_char *row,const COLUMN *column_attribute,int column_number,buffer *buf)
{
	int field_length,i;
	/*用于累计当前解析的row的总长度*/
	long row_length=0;
	/*操作位图的指针*/
	/*检查位图的方法是：按字节检查。先检查位图的第一字节，当第一字节的8位都检查完了
 	就将位图指针移动一个字节指向位图的第二个字节，依次检查完位图的所有字节*/
	u_char *bitmap=row-1;
	/*位图所占字节数*/
	int bytes_of_bitmap=column_number/8+1;
	/*row跳过位图指向此记录中的第一个field*/
	row+=bytes_of_bitmap;
	for(i=0;i<column_number;i++){
		if((i%8)==0){
			bitmap++;
		}
		if((*bitmap>>(i%8))&0x01){
			print_buf("null",4,buf);
		}else{
			field_length=ms_field_analysis(row,column_attribute[i].datatype,column_attribute[i].scale,buf);
			row+=field_length;
			row_length+=field_length;
		}
		if(i!=column_number-1)
			print_buf("&&&",3,buf);
		else 
			print_buf("$$$",3,buf);
	}
	/*nbcrow比一般row多一个位图，所以nbcrow的长度应该是各个field的长度+位图长度，
 	所以这里把位图长度加上才能使ms_result_set函数中正确跳到下一个row*/
	return row_length+bytes_of_bitmap;
	
}
	
void ms_result_set(u_char *columns,buffer *buf)
{
	int i;
	/*数组元素中依次存放每个列的类型(有某些列还存放了scale)。这里假定一张表中，列的数量不会超过255*/
  	/*定义成静态是因为analysis_row函数要是用此数组中的值*/
	static COLUMN column_attribute[255]={{0,0}};
	/*计算column个数*/
	unsigned short column_number=(*((unsigned short *)(columns)));
	u_char *column=columns+2;
	if((column_number!=0x00)&&(column_number!=0xffff)){
		for(i=0;i<column_number;i++){
			/*此循环的任务就是将cloumn指针正确移动到接下来的row报文
 			思路就是先移动6个字节，得到此column的类型，然后调用datatype_length
			函数根据类型判断出此column的总字节数（column字节数随列类型的不同而不同），
			字节数确定以后就可以顺利将指针移到下一个column。然后循环。如此
			column_number次，column指针就移动到了row报文了。同时也把column_number
			个列的类型值放到了column_attribute数组中了。接下去解析row
			的时候会要用到列的类型。不同的列类型决定了此列的值的长度指示器
			所占的字节数。也决定了列值是字符型还是二进制型*/
			column_attribute[i].datatype=*(column+6);
			/*此句目前为解析time datetime2 timedateoffset三种类型使用，因为解析他们的时候需要用到其cloumn中的scale值，
 			详细可见官方手册p41，而其他类型暂时不需要，若后期需要也可加上*/
			if((*(column+6)==41)||(*(column+6)==42)||(*(column+6)==43)){
				column_attribute[i].scale=*(column+7);
			}
			/*此句为解析decimal和numeric所用，这两个类型也要用到scale，但和time类型的scale并不一样*/
			if((*(column+6)==106)||(*(column+6)==108)){
				column_attribute[i].scale=*(column+9);//这里的scale是由用户调整比例决定的
			}
			/*3个MAX类型的类型值和其对应的非MAX类型类型值是一样的，如varchar(MAX)的类型值是167，而varchar的类型值也是167
 			所以我们碰到167类型的column时要区分是MAX还是非MAX的，区分的点就在largetypesize的值，所以我们这里若碰到这三个
			类型的column时就将largetypesize的值取到column_attribute[i].scale中，以便ms_field_analysis函数中判断使用*/
			if((*(column+6)==167)||(*(column+6)==165)||(*(column+6)==231)){
				column_attribute[i].scale=*((unsigned short *)(column+7));
			}
			column+=datatype_length(column+6);
		}
	}else{
		printf("no columns\n");
		return;
	}
	/*所有column过完之后，指针来到row*/
	u_char *row=column;
	long row_length;
	/*如果column报文之后的第一个字节是0xd1，说明是row报文，解析之*/
	/*不知道column报文之后除了是row报文还会有什么？还可能是done报文，即这个表目前没有记录
 	。还有可能是0xd2，nbcrow报文。还有可能是order报文*/
	while(1){
		if(*row==0xd1){//row报文
			row_length=analysis_row(row+1,column_attribute,column_number,buf);//解析row报文
			row=row+row_length+1;//跳到下一个row报文
		}else if(*row==0xd2){//nbcrow报文
			row_length=analysis_nbcrow(row+1,column_attribute,column_number,buf);//解析nbcrow报文
			row=row+row_length+1;//跳到下一个row报文
		}else if(*row==0xa9){//order报文
			/*跳过order报文*/
			row+=((*(short *)(row+1))+3);
		}else if(*row==0xfd){//done报文
			break;
		}else{
			printf("找不到报文类型\n");
			break;
		}
	}
}
void response(u_char *payload,buffer *buf)
{
	switch(*(payload+8))
	{
		case 0x81:ms_result_set(payload+8+1,buf);break;//跳过0x81字节，指向column个数
	}
}
