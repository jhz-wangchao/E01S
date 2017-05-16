#include "BufferQueue.h"
#include "stdlib.h"
#include <string.h>
void BQ_Init(BufferQueue *q, int BufferSize)
{
	List_Initilize(&q->list);
	q -> BufferSize = BufferSize;
}
void BQ_Clear(BufferQueue *q)
{
	pListNode p;
	for(p = q->list.Head; NULL_NODE != p; p = p->Next)
	{
		free(p->Buffer);
	}
	List_Free(&q->list);
}
unsigned char BQ_Empty(BufferQueue *q)
{
	return q->list.Count == 0;
}
unsigned char BQ_Full(BufferQueue *q)
{
	return 0;
}
void BQ_Pop(BufferQueue *q)
{
	if(!BQ_Empty(q))
	{
		free(q->list.Head->Buffer);
		List_Remove(&q->list, q->list.Head);
	}
}
unsigned char* BQ_GetPop(BufferQueue* q)
{
	return q->list.Head->Buffer;
}
unsigned char* BQ_GetHead(BufferQueue* volatile q)
{
	return q->list.Tal->Buffer;
}
unsigned char* BQ_NewBuffer(BufferQueue * volatile q)
{
	unsigned char* rtn = ((unsigned char*)0);
	if(!BQ_Full(q))
	{
		rtn = malloc(q->BufferSize);
		List_Add_Tal(&q->list, rtn);
	}
	return rtn;
}
int BQ_Size(BufferQueue *q)
{
	return q->list.Count;
}
