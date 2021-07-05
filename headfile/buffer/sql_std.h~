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
/*
 * 判断用户输入的sql语句中的命令类型是否合法。参数type用于选择命令集
 * type==0时表示选用all_com命令集，目前mysql,sqlserver,oracle都使用这一命令集
 * 当type==1是选用的是all_com_redis命令，这是redis使用的
 */
int is_legal(const u_char *com,int len,int type)
{
	/*所有命令类型*/
	u_char *all_com[]={ "select", "insert", "update", "flush", "delete", "create", "set", "drop", "show", "alter", "use",
		"grant", "truncate", "backup", "dbcc", "source", NULL
	};
	u_char *all_com_redis[]={
"mget", "mset", "lset", "strlen", "set", "auth", "echo", "ping", "quit", "select", "lindex", "linsert", "llen", "lpop", "lpush",
"mget", "mset", "lset", "strlen", "set", "auth", "echo", "ping", "quit", "select", "lindex", "linsert", "llen", "lpop", "lpush", "lpushx", 
"lrange", "lrem", "bitpos", "decr", "decrby", "get", "getbit", "getrange", "getset", "incr", "sadd", "scard", "sdiff", "sdiffstore", "sinter",
"sinterstore", "sismember", "smembers", "smove", "spop", "zadd", "zcard", "zcount", "zincrby", "zinterstore", "zlexcount", "zrange", "zrangebylex",
"zrangebyscore", "zrank", "zrem", "zremrangebylex", "zremrangebyrank", "zremrangebyscore", "zrevrange", "zrevrangebylex", "zrevrangebyscore", 
"zrevrank", "zscan", "zscore", "hdel", "hexists", "hget", "hgetall", "hincrby", "hincrbyfloat", "hkeys", "hlen", "hmget", "hmset", "hscan",
"hset", "hsetnx", "cluster addslots", "cluster count-failure-reports", "cluster countkeysinslot", "cluster delslots", "cluster failover",
"cluster forget", "cluster getkeysinslot", "cluster info", "cluster keyslot", "cluster meet", "cluster nodes", "cluster replicate", "cluster reset",
"cluster saveconfig", "cluster set-config-epoch", "cluster setslot", "cluster slaves", "cluster slots", "readonly", "readwrite", "geoadd",
"geodist", "geohash", "geopos", "georadius", "georadiusbymember", "hstrlen", "hvals", "pfadd", "pfcount", "pfmerge", "blpop", "brpop", 
"brpoplpush", "ltrim", "rpop", "rpoplpush", "rpush", "rpushx", "psubscribe", "publish", "pubsub", "punsubscribe", "subscribe", "unsubscribe",
"eval", "evalsha", "script debug", "script exists", "script flush", "script kill", "script load", "bgrewriteaof", "bgsave", "client getname",
"client kill", "client list", "client pause", "client reply", "client setname", "command", "command count", "command getkeys", "command info", 
"config get", "config resetstat", "config rewrite", "config set", "dbsize", "debug object", "debug segfault", "flushall", "flushdb", "info",
"lastsave", "monitor", "role", "save", "shutdown", "slaveof", "slowlog", "sync", "time", "srandmember", "srem", "sscan", "sunion", "sunionstore",
"zunionstore", "append", "bitcount", "bitfield", "bitop", "incrby", "incrbyfloat", "msetnx", "psetex", "setbit", "setex", "setnx", "setrange", 
"discard", "exec", "multi", "unwatch", "watch",NULL
	};
	u_char **all_com_type[]={all_com,all_com_redis};

	//截取命令放入com_type数组
	u_char com_type[len+1];
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
	for(i=0;all_com_type[type][i]!=NULL&&strcmp(com_type,all_com_type[type][i]);i++){
		;
	}
	if(all_com_type[type][i]==NULL)
		return 1;//没有匹配到
	else
		return 0;//匹配到了
}
