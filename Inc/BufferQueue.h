#ifndef __BUFFERQUEUE_H__
#define __BUFFERQUEUE_H__
#ifdef __cplusplus
extern "C"{
#endif
#include "List.h"
typedef struct
{
	List list;
	volatile int BufferSize;
}BufferQueue;

void BQ_Init(BufferQueue *q, int BufferSize);
void BQ_Clear(BufferQueue *q);
unsigned char BQ_Empty(BufferQueue *q);
unsigned char BQ_Full(BufferQueue *q);
void BQ_Pop(BufferQueue *q);
unsigned char* BQ_GetPop(BufferQueue* q);
unsigned char* BQ_GetHead(BufferQueue* q);
unsigned char* BQ_NewBuffer(BufferQueue *q);
int BQ_Size(BufferQueue *q);
#ifdef __cplusplus
}
#endif
#endif
