#ifndef __QUEUE_H__
#define __QUEUE_H__
#ifdef __cplusplus
extern "C"{
#endif
#define QUEUE_SIZE 1024
#define QUEUE_MASK (QUEUE_SIZE - 1)
typedef struct
{
	volatile unsigned char buffer[QUEUE_SIZE];
	volatile int head,tal;
}Queue, *pQueue;

unsigned char Queue_Empty(pQueue q);
unsigned char Queue_Full(pQueue q);
unsigned char Queue_Pop(pQueue q);
unsigned char Queue_GetPop(pQueue q);
void Queue_Push(pQueue q,unsigned char value);
void Queue_Initilize(pQueue q);
int Queue_Size(pQueue q);
#ifdef __cplusplus
}
#endif
#endif
