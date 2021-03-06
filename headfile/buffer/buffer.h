#ifndef _BUFFER_H
#define _BUFFER_H

/*链表节点*/
typedef struct _buf_node{
        char *data;
        struct _buf_node *next;
        struct _buf_node *pre;
}buf_node;
/*缓冲区*/
typedef struct _buffer{
        buf_node *head;
        int lock;
}buffer;

int init_buffer(buffer *);
int print_buf(char *,int ,buffer *);
int set_timer();
#endif
