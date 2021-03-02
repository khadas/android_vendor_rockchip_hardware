// FIXME: your file license if you have one

#include "LoadModelCallback.h"
#include "utils.h"

namespace rockchip::hardware::neuralnetworks::implementation {

// Methods from ::rockchip::hardware::neuralnetworks::V1_0::ILoadModelCallback follow.
Return<void> LoadModelCallback::notify(::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus status, const ::rockchip::hardware::neuralnetworks::V1_0::RKNNDeviceID& pdevs) {
    RECORD_TAG();
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//ILoadModelCallback* HIDL_FETCH_ILoadModelCallback(const char* /* name */) {
    //return new LoadModelCallback();
//}
//
}  // namespace rockchip::hardware::neuralnetworks::implementation
