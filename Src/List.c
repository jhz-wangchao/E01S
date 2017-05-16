/*****************************************************************************
Copyright:  进化者
File name: List.c
Description: 提供链表算法
Author: wangchao
Version: V0.1
Date: 2017年2月28日
History: 
*****************************************************************************/
#include "List.h"
#include <stdlib.h>
#include <string.h>
static int list_compareNull(void*a,void*b){(void)a;(void)b;return 0;}
static pListNode list_newElement(void * Data)
{
	pListNode NewElement = (pListNode)malloc(sizeof(ListNode));
	
	NewElement->Buffer = Data;
	return NewElement;
}
void List_Initilize(pList list)
{
	list->Head = list->Tal = NULL_NODE;
	list->Count = 0;
	list->CompareFun = list_compareNull;
}
void List_Add_Head(pList list, void * Data)
{
	pListNode NewElement = list_newElement(Data);
	
	#ifdef DOUBLE_WAY
	NewElement->Pre = NULL_NODE;
	if(list->Head)list->Head->Pre = NewElement;
	#endif
	NewElement->Next = list->Head;
	list->Head = NewElement;
	if(!list->Count)
	{
		list->Tal = NewElement;
	}
	list->Count++;
}
void List_Add_Tal(pList list, void * Data)
{
	pListNode NewElement = list_newElement(Data);
	
	#ifdef DOUBLE_WAY
	NewElement->Pre = list->Tal;
	#endif
	if(list->Tal)list->Tal->Next = NewElement;
	NewElement->Next = NULL_NODE;
	list->Tal = NewElement;
	if(!list->Count)
	{
		list->Head = NewElement;
	}
	list->Count++;
}

void List_Remove(pList list, pListNode node)
{
	pListNode cur = list->Head;
	pListNode last = NULL_NODE;
	pListNode next = NULL_NODE;
	while(cur != NULL)
	{
		if(cur == node)
		{
			next = node->Next;
			if(last != NULL_NODE)
			{
				last->Next = next;
			}
			else
			{
				list->Head = next;
			}
			if(next != NULL_NODE)
			{
				#ifdef DOUBLE_WAY
				next->Pre = last;
				#endif
			}
			else
			{
				list->Tal = last;
			}
			//free(cur->Buffer);
			free(cur); 
			cur = NULL;
			list->Count--;
		}
		if(cur != NULL)
		{
			last = cur;
			cur = cur->Next;
		}
	}
}
void List_Free(pList list)
{
	pListNode n = list->Head, t = NULL_NODE;
	while(n != NULL_NODE)
	{
		t=n;
		n=t->Next;
		//free(t->Buffer);
		free(t);
	}
	list->Head = list->Tal = NULL_NODE;
	list->Count = 0;
}
void List_CompareFunSetup(pList list, CompareFunDelegate CompareFun)
{
	list->CompareFun = CompareFun;
}

pListNode List_Find(pList list, void * DataToFind)
{
	pListNode n = list->Head;
	if(list_compareNull == list->CompareFun)
	{
		return NULL_NODE;
	}
	while(n != NULL_NODE)
	{
		if(list->CompareFun(DataToFind, n->Buffer) == 0)
			return n;
		n = n->Next;
	}
	return NULL_NODE;
}
void List_Insert(pList list, void * Data)
{
	pListNode NewElement = list_newElement(Data);
	pListNode * p = &list->Head;
	while(*p)
	{
		if(list->CompareFun(Data, (*p)->Buffer) <=0)
			break;
		p = &((*p)->Next);
	}
	if(*p)
	{
		NewElement->Next = *p;
		#ifdef DOUBLE_WAY
		NewElement->Pre = (*p)->Pre;
		(*p)->Pre = NewElement;
		#endif
		*p = NewElement;
	}
	else
	{
		#ifdef DOUBLE_WAY
		NewElement->Pre = list->Tal;
		#endif
		if(list->Tal)list->Tal->Next = NewElement;
		NewElement->Next = NULL_NODE;
		list->Tal = NewElement;
		if(!list->Count)
		{
			list->Head = NewElement;
		}
	}
	list->Count++;
}
void List_Order(pList list)
{
	pListNode p = list->Head;
	pListNode s,r;
	void * t;
	if(0 == list->Count)return;
	if(list_compareNull == list->CompareFun)return;
	while(NULL_NODE != p->Next)
	{
		s = p;
		r = p->Next;
		while(NULL_NODE != r)
		{
			if(list->CompareFun(s->Buffer, r->Buffer) > 0)
			{
				s = r;
			}
			r = r->Next;
		}
		if(s!=p)
		{
			t = s->Buffer;
			s->Buffer = p->Buffer;
			p->Buffer = t;
		}
		p=p->Next;
	}
}
pListNode List_FindMinimum(pList list)
{
	pListNode p, m;
	if(0 == list->Count)return NULL_NODE;
	if(list_compareNull == list->CompareFun)return list->Head;
	for(m = p = list->Head; NULL_NODE != p; p = p->Next)
	{
		if(list->CompareFun(m->Buffer, p->Buffer) > 0)
		{
			m = p;
		}
	}
	return m;
}
pListNode List_FindMaximum(pList list)
{
	pListNode p, m;
	if(0 == list->Count)return NULL_NODE;
	if(list_compareNull == list->CompareFun)return list->Head;
	for(m = p = list->Head; NULL_NODE != p; p = p->Next)
	{
		if(list->CompareFun(m->Buffer, p->Buffer) < 0)
		{
			m = p;
		}
	}
	return m;
}
