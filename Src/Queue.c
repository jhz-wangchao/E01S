#include "Queue.h"
unsigned char Queue_Empty(pQueue q)
{
	return (q -> head == q -> tal);
}
unsigned char Queue_Full(pQueue q)
{
	return (((q -> head + 1) & QUEUE_MASK) == q -> tal);
}
unsigned char Queue_Pop(pQueue q)
{
	unsigned char dat = 0;
	if(!Queue_Empty(q))
	{
		dat = q -> buffer[q -> tal];
		q -> tal = (q -> tal + 1) & QUEUE_MASK;
	}
	return dat;
}
void Queue_Push(pQueue q,unsigned char value)
{
	if(!Queue_Full(q))
	{
		q -> buffer[q -> head] = value;
		q -> head = (q -> head + 1) & QUEUE_MASK;
	}
}
void Queue_Initilize(pQueue q)
{
	int i;
	unsigned long * b = (unsigned long*)q->buffer;
	for(i=0;i<QUEUE_SIZE/sizeof(unsigned long); i++)
		b[i] = (unsigned long)-1;
	q -> head = 0;
	q -> tal = 0;
}
int Queue_Size(pQueue q)
{
	return ((QUEUE_SIZE + q -> head) - q -> tal) & QUEUE_MASK;
}
unsigned char Queue_GetPop(Queue* q)
{
	if(Queue_Empty(q))
	{
		return 0;
	}
	return q->buffer[q->tal];
}
