// SPDX-FileCopyrightText: © 2026 Allegro DVT <github-ip@allegrodvt.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "lib_common/SeiInternal.h"
#include "lib_common/BufferSeiMeta.h"
#include "lib_common_dec/RbspParser.h"
#include "lib_common_dec/DecCallbacks.h"
#include "Aup.h"

/*****************************************************************************/
typedef enum
{
  AL_SEI_PARSE_RESULT_UNKNOWN_SEI,
  AL_SEI_PARSE_RESULT_PARSED,
  AL_SEI_PARSE_RESULT_PARSING_ERROR,
}AL_ESeiParseResult;

typedef AL_ESeiParseResult (* AL_PFN_ParseOneSei)(AL_TRbspParser* pRP, AL_ESeiPayloadType ePayloadType, uint32_t uPayloadSize, AL_TAup* pOutputAup, bool* pCanSendToUser);

typedef struct AL_TSeiParserCtx
{
  AL_TAup* pOutputAup;
  AL_TSeiMetaData* pOutputMeta;
  AL_CB_ParsedSei* pSeiParsedCallback;
  AL_PFN_ParseOneSei pfnCustomSeiParsing;
}AL_TSeiParserCtx;

/*****************************************************************************/
void AL_SeiParser_Init(AL_TSeiParserCtx* pCtx, AL_TAup* pOutputAup, AL_TSeiMetaData* pOutputMeta, AL_CB_ParsedSei* pSeiParsedCallback);

/*****************************************************************************/
void AL_SeiParser_AddCustomSeiParsing(AL_TSeiParserCtx* pCtx, AL_PFN_ParseOneSei pfnCustomSeiParsing);

/*****************************************************************************/
bool AL_SeiParser_Parse(AL_TSeiParserCtx* pCtx, AL_TRbspParser* pRP, bool bIsPrefix);
