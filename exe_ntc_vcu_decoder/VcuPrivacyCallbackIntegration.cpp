// SPDX-License-Identifier: MIT

#include "VcuPrivacyCallbackIntegration.hpp"

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

  NtcPrivacyCallbackResult const callbackResult =
    NtcPrivacyProcessNv12Frame(&frame, &rectangle, 1U);

  return PrivacyCallbackDecision {
    adapterStatus,
    callbackResult.status,
    callbackResult.output_allowed != 0U,
  };
}

} // namespace ntc_vcu
