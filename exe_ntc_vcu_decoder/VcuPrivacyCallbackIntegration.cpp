// SPDX-License-Identifier: MIT

#include "VcuPrivacyCallbackIntegration.hpp"

#if defined(NTC_ENABLE_PRIVACY_GDB_EVIDENCE)
#include "vcu_privacy_gdb_evidence.h"
#endif

namespace ntc_vcu {
namespace {

PrivacyCallbackDecision AdapterFailure(
  DecodedBufferAdapterStatus adapterStatus)
{
  return PrivacyCallbackDecision {
    adapterStatus,
    NTC_PRIVACY_CALLBACK_INVALID_ARGUMENT,
    false,
  };
}

} // namespace

PrivacyCallbackDecision ApplyFullFramePrivacyValidation(
  AL_TBuffer* pFrame)
{
  DecodedNv12View view {};
  DecodedBufferAdapterStatus const adapterStatus =
    MakeDecodedNv12View(pFrame, &view);

  if(adapterStatus != DecodedBufferAdapterStatus::kOk)
    return AdapterFailure(adapterStatus);

  NtcPrivacyNv12Frame frame {
    view.yPlane,
    view.uvPlane,
    view.yPhysicalAddress,
    view.uvPhysicalAddress,
    view.width,
    view.height,
    view.yPitch,
    view.uvPitch,
  };

  NtcPrivacyRect const rectangle {
    0,
    0,
    static_cast<int32_t>(view.width),
    static_cast<int32_t>(view.height),
  };

#if defined(NTC_ENABLE_PRIVACY_GDB_EVIDENCE)
  NtcPrivacyCallbackResult const callbackResult =
    NtcPrivacyProcessNv12FrameWithEvidence(&frame, &rectangle, 1U);
#else
  NtcPrivacyCallbackResult const callbackResult =
    NtcPrivacyProcessNv12Frame(&frame, &rectangle, 1U);
#endif

  return PrivacyCallbackDecision {
    adapterStatus,
    callbackResult.status,
    callbackResult.output_allowed != 0U,
  };
}

} // namespace ntc_vcu
