// SPDX-License-Identifier: MIT

#pragma once

#include "VcuDecodedBufferAdapter.hpp"
#include "vcu_privacy_callback.h"

namespace ntc_vcu {

struct PrivacyCallbackDecision
{
  DecodedBufferAdapterStatus adapterStatus;
  NtcPrivacyCallbackStatus callbackStatus;
  bool outputAllowed;
};

// Validation integration used until the detector supplies its complete region
// set. The complete decoded NV12 frame is selected deliberately so this path
// verifies callback ordering, in-place modification and output gating without
// making a face- or license-plate-detection claim.
//
// The function does not own, copy, reference, release or flush pFrame.
PrivacyCallbackDecision ApplyFullFramePrivacyValidation(
  AL_TBuffer* pFrame);

} // namespace ntc_vcu
