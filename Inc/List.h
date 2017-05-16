#ifndef __LIST_H__
#define __LIST_H__
#ifdef __cplusplus
extern "C"{
#endif
//#define DOUBLE_WAY

typedef struct __ListNode ListNode, *pListNode;
typedef int (*CompareFunDelegate)(void* a, void *b);
struct __ListNode
{
	#ifdef DOUBLE_WAY
	pListNode Pre;
	#endif
	pListNode Next;
	
	void * Buffer;
};
typedef struct __ListHead List, *pList;
struct __ListHead
{
	pListNode Head;
	pListNode Tal;
	unsigned int Count;
	CompareFunDelegate CompareFun;
};
#define NULL_NODE (pListNode)0
#define List_Value (l, t) (*((t*)l->Buffer))

void List_Initilize(pList list);
void List_Add_Head(pList list, void * Data);
void List_Add_Tal(pList list, void * Data);
void List_Remove(pList list, pListNode node);
void List_Free(pList list);
void List_CompareFunSetup(pList list, CompareFunDelegate CompareFun);
void List_Insert(pList list, void * Data);
pListNode List_Find(pList list, void * DataToFind);
void List_Order(pList list);
pListNode List_FindMinimum(pList list);
pListNode List_FindMaximum(pList list);
#ifdef __cplusplus
}
#endif
#endif
