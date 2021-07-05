#include "uthash.h"
#include <stdlib.h>   /* malloc */
#include <stdio.h>    /* printf */
#define _GNU_SOURCE
#define _GNU_SOURCE
#include <sched.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <pcre.h>

#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <poll.h>
#define NETMAP_WITH_LIBS

#define _GNU_SOURCE
#include <pthread.h>



typedef struct example_user_t {
    int id;
    int cookie;
    UT_hash_handle hh;
} example_user_t;

pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    mutex_config = PTHREAD_MUTEX_INITIALIZER;



example_user_t *users=NULL;



void* tcptable_time_process(){
	struct example_user_t *users2,*tmp=NULL;
	while(1){

		if (pthread_mutex_lock(&mutex) != 0) {
		fprintf(stderr,"thread can't acquire readlock\n");
		continue;
		}

//    example_user_t *user, *users=NULL;
//    for(user=users; user != NULL; user=(example_user_t*)(user->hh.next)) {
//        printf("user %d, cookie %d\n", user->id, user->cookie);
//    }

		printf("tables:\n");
		HASH_ITER(hh, users, users2, tmp) {
			printf("id:\n| %d| |\n", users->id);
			//if(my_tcpcon->time1%2==0){
			//HASH_DEL( my_tcptable, my_tcpcon);
      			//free(my_tcpcon);
			//}
    		}

		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return 0;
}



int test(){

}

int main(int argc,char *argv[]) {
int test=0x02;
int ifsyn= (test >> (9-7 - 1)) & 1;
printf("test:%d",ifsyn);

/*


	pthread_t tcptable_time_process1;
	pthread_create(&tcptable_time_process1,NULL,tcptable_time_process,NULL);
	pthread_detach(tcptable_time_process1);
test();
    int i;

    example_user_t *user=NULL;

    for(i=0;i<10;i++) {
        if ( (user = (example_user_t*)malloc(sizeof(example_user_t))) == NULL) exit(-1);
        user->id = 3;
        user->cookie = 5;
        HASH_ADD_INT(users,id,user);
    }



while(1){

sleep(1);
}
*/
   return 0;
}
