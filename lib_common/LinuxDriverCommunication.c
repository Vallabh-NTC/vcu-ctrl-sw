// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#include "lib_common/I_Communication.h"
#include "lib_rtos/lib_rtos.h"
#include <errno.h>

static int32_t Open(AL_ICommunication* pCommunication, char const* device)
{
  (void)pCommunication;
  return (int32_t)(intptr_t)Rtos_DriverOpen(device);
}

static void Close(AL_ICommunication* pCommunication, int32_t fd)
{
  (void)pCommunication;
  Rtos_DriverClose((void*)(intptr_t)fd);
}

static AL_ECommunicationError ErrnoToDriverError(int32_t err)
{
  if(err == ENOMEM)
    return COMMUNICATION_ERROR_NO_MEMORY;

  if(err == EINVAL || err == EPERM)
    return COMMUNICATION_ERROR_CHANNEL;

  return COMMUNICATION_ERROR_UNKNOWN;
}

static AL_ECommunicationError PostMessage(AL_ICommunication* pCommunication, int32_t fd, uint32_t messageId, void* data, bool isBlocking)
{
  (void)pCommunication;

  while(true)
  {
    int32_t iRet;

    if(messageId != AL_POLL_MSG)
      iRet = Rtos_DriverIoctl((void*)(intptr_t)fd, messageId, data);
    else
    {
      Rtos_PollCtx* ctx = (Rtos_PollCtx*)data;
      iRet = Rtos_DriverPoll((void*)(intptr_t)fd, ctx);

      if(iRet == 0)
        return COMMUNICATION_TIMEOUT;
    }

    int32_t errdrv = errno;

    if(iRet < 0)
    {
      /* posix -> EAGAIN == EWOULDBLOCK */
      if(((errdrv == EAGAIN) && isBlocking) || (errdrv == EINTR))
        continue;
      return ErrnoToDriverError(errdrv);
    }

    return COMMUNICATION_SUCCESS;
  }
}

static AL_TCommunicationVTable const linuxDriverVtable =
{
  &Open,
  &Close,
  &PostMessage,
};

static AL_ICommunication linuxDriver =
{
  &linuxDriverVtable
};

AL_ICommunication* AL_GetLinuxDriverCommunication(void)
{
  return &linuxDriver;
}
