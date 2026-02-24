// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_rtos/types.h"

#define AL_POISONOUS1 ((void*)(intptr_t)0xdeadbeef)
#define AL_POISONOUS2 ((void*)(intptr_t)0xcafecafe)

typedef struct AL_TListHead
{
  struct AL_TListHead* pNext, * pPrev;
}AL_ListHead;

#define containerOf(ptr, type, member) \
  ((type*)((char*)(ptr) - offsetof(type, member)))

#define AL_ListEntry(ptr, type, member) \
  containerOf(ptr, type, member)

#define AL_ListFirstEntry(ptr, type, member) \
  AL_ListEntry((ptr)->pNext, type, member)

#define AL_ListNextEntry(pos, type, member) \
  AL_ListEntry((pos)->member.pNext, type, member)

#define AL_ListForEachEntry(pos, head, member) \
  for(pos = AL_ListFirstEntry(head, typeof(*pos), member); \
      &pos->member != (head); \
      pos = AL_ListNextEntry(pos, typeof(*pos), member))

#define AL_ListForEachEntrySafe(pos, next, head, member) \
  for(pos = AL_ListFirstEntry(head, typeof(*pos), member), \
      next = AL_ListNextEntry(pos, typeof(*pos), member); \
      &pos->member != (head); \
      pos = next, next = AL_ListNextEntry(next, typeof(*pos), member))

#define AL_ListForEachEntryWithType(pos, head, member, type) \
  for(pos = AL_ListFirstEntry(head, type, member); \
      &pos->member != (head); \
      pos = AL_ListNextEntry(pos, type, member))

/**
* AL_ListFindEntryByAddr - Generic macro to find a list entry by address
* @head: The list_head pointer
* @type: The type of the structure containing the list
* @member: The name of the list_head member in the structure
* @target_addr: The expected address to match
* @result: Variable to store the result (output parameter)
*/
#define AL_ListFindEntryByAddr(head, type, member, target_addr, result) \
  do { \
    type* __entry = NULL, * __tmp; \
   \
    result = NULL; \
    AL_ListForEachEntrySafe(__entry, __tmp, head, member) { \
      if(__entry == (type*)(target_addr)){ \
        result = __entry; \
        break; \
      } \
    } \
  } while(0)

static inline int AL_ListEmpty(const AL_ListHead* pHead)
{
  return pHead->pNext == pHead;
}

static inline void AL_ListHeadInit(AL_ListHead* pHead)
{
  pHead->pNext = pHead;
  pHead->pPrev = pHead;
}

static inline void __ListAdd(AL_ListHead* pNew, AL_ListHead* pPrev, AL_ListHead* pNext)
{
  pNext->pPrev = pNew;
  pNew->pNext = pNext;
  pNew->pPrev = pPrev;
  pPrev->pNext = pNew;
}

static inline void AL_ListAddTail(AL_ListHead* pNew, AL_ListHead* pHead)
{
  __ListAdd(pNew, pHead->pPrev, pHead);
}

static inline void __ListDel(AL_ListHead* pPrev, AL_ListHead* pNext)
{
  pNext->pPrev = pPrev;
  pPrev->pNext = pNext;
}

static inline void AL_ListDel(AL_ListHead* pEntry)
{
  __ListDel(pEntry->pPrev, pEntry->pNext);
  pEntry->pNext = (AL_ListHead*)AL_POISONOUS1;
  pEntry->pPrev = (AL_ListHead*)AL_POISONOUS2;
}
