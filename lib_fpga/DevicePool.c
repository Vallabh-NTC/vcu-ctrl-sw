// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "lib_rtos/lib_rtos.h"

#include "DevicePool.h"

struct FileDesc
{
  char* filename;
  int32_t iRefCount;
  int32_t fd;
};

#define POOL_SIZE 32

struct DevicePool
{
  struct FileDesc pPool[POOL_SIZE];
  AL_MUTEX pLock;
};

static bool DevicePool_Init(struct DevicePool* pDP)
{
  pDP->pLock = Rtos_CreateMutex();

  if(pDP->pLock == NULL)
    return false;

  return true;
}

static void DevicePool_Deinit(struct DevicePool* pDP)
{
  if(pDP->pLock != NULL)
    Rtos_DeleteMutex(pDP->pLock);
  pDP->pLock = NULL;
}

static struct FileDesc* DevicePool_FindEntryByFd(struct DevicePool* pDP, int32_t fd)
{
  for(int8_t i = 0; i < POOL_SIZE; ++i)
  {
    struct FileDesc* pCur = &pDP->pPool[i];

    if(pCur->iRefCount && fd == pCur->fd)
      return pCur;
  }

  return NULL;
}

static struct FileDesc* DevicePool_FindEntryByName(struct DevicePool* pDP, char const* filename)
{
  for(int8_t i = 0; i < POOL_SIZE; ++i)
  {
    struct FileDesc* pCur = &pDP->pPool[i];

    if((pCur->iRefCount > 0) && (strcmp(filename, pCur->filename) == 0))
      return pCur;
  }

  return NULL;
}

static struct FileDesc* DevicePool_FindFreeEntry(struct DevicePool* pDP)
{
  for(int8_t i = 0; i < POOL_SIZE; ++i)
  {
    struct FileDesc* pCur = &pDP->pPool[i];

    if(pCur->iRefCount == 0)
      return pCur;
  }

  return NULL;
}

static int32_t DevicePool_Open(struct DevicePool* pDP, char const* filename)
{
  int32_t iRet = 0;

  Rtos_GetMutex(pDP->pLock);

  struct FileDesc* pCur = DevicePool_FindEntryByName(pDP, filename);

  if(pCur == NULL)
  {
    pCur = DevicePool_FindFreeEntry(pDP);

    if(!pCur)
    {
      iRet = -1;
      goto exit;
    }

    pCur->filename = strdup(filename);
    pCur->fd = open(filename, O_RDWR);

    if(pCur->fd < 0)
    {
      iRet = -1;
      free(pCur->filename);
      goto exit;
    }
  }

  pCur->iRefCount++;
  iRet = pCur->fd;

  exit:
  Rtos_ReleaseMutex(pDP->pLock);
  return iRet;
}

static int32_t DevicePool_Close(struct DevicePool* pDP, int32_t fd)
{
  int32_t iRet = 0;

  Rtos_GetMutex(pDP->pLock);

  struct FileDesc* pEntry = DevicePool_FindEntryByFd(pDP, fd);

  if(pEntry == NULL)
  {
    /* We don't have this file descriptor */
    iRet = -1;
    goto exit;
  }

  Rtos_Assert(pEntry->iRefCount > 0);

  pEntry->iRefCount -= 1;

  if(pEntry->iRefCount == 0)
  {
    free(pEntry->filename);
    iRet = close(pEntry->fd);
  }

  exit:
  Rtos_ReleaseMutex(pDP->pLock);

  return iRet;
}

#include <stdlib.h>

static bool g_DevicePoolInit;
static struct DevicePool g_DevicePool;

static void AL_DevicePool_Deinit(void)
{
  DevicePool_Deinit(&g_DevicePool);
}

static bool AL_DevicePool_Init(void)
{
  atexit(&AL_DevicePool_Deinit);
  return DevicePool_Init(&g_DevicePool);
}

int32_t AL_DevicePool_Open(char const* filename)
{
  if(!g_DevicePoolInit)
  {
    if(!AL_DevicePool_Init())
      return -1;
    g_DevicePoolInit = true;
  }

  return DevicePool_Open(&g_DevicePool, filename);
}

int32_t AL_DevicePool_Close(int32_t fd)
{
  return DevicePool_Close(&g_DevicePool, fd);
}
