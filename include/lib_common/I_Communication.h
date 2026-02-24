// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

/******************************************************************************
   \addtogroup Communication
   !@{
   \file
 *****************************************************************************/
#pragma once

#include "lib_rtos/types.h"

typedef enum AL_ECommunicationError
{
  COMMUNICATION_SUCCESS,
  COMMUNICATION_ERROR_UNKNOWN,
  COMMUNICATION_ERROR_NO_MEMORY,
  COMMUNICATION_ERROR_CHANNEL,
  COMMUNICATION_TIMEOUT,
}AL_ECommunicationError;

#define AL_POLL_MSG 0xfffffffc

/*****************************************************************************
    \brief Interfaces with a device.
    The device can either be the interface of a kernel pCommunication like al5e, al5r or al5d
    or it could also be a socket, this is implementation dependant.
    \see AL_GetHardwareCommunication for the kernel pCommunication implementation
*****************************************************************************/
typedef struct AL_ICommunication AL_ICommunication;
typedef struct AL_TCommunicationVTable
{
  int32_t (* pfnOpen)(AL_ICommunication* pCommunication, const char* device);
  void (* pfnClose)(AL_ICommunication* pCommunication, int32_t fd);
  AL_ECommunicationError (* pfnPostMessage)(AL_ICommunication* pCommunication, int32_t fd, uint32_t messageId, void* data, bool isBlocking);
}AL_TCommunicationVTable;

struct AL_ICommunication
{
  AL_TCommunicationVTable const* vtable;
};

static inline
int32_t AL_ICommunication_Open(AL_ICommunication* pCommunication, char const* device)
{
  return pCommunication->vtable->pfnOpen(pCommunication, device);
}

static inline
void AL_ICommunication_Close(AL_ICommunication* pCommunication, int32_t fd)
{
  pCommunication->vtable->pfnClose(pCommunication, fd);
}

static inline
AL_ECommunicationError AL_ICommunication_PostBlockingMessage(AL_ICommunication* pCommunication, int32_t fd, uint32_t messageId, void* data)
{
  return pCommunication->vtable->pfnPostMessage(pCommunication, fd, messageId, data, true);
}

static inline
AL_ECommunicationError AL_ICommunication_PostNonBlockingMessage(AL_ICommunication* pCommunication, int32_t fd, uint32_t messageId, void* data)
{
  return pCommunication->vtable->pfnPostMessage(pCommunication, fd, messageId, data, false);
}

/*!@}*/
