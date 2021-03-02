// FIXME: your file license if you have one

#include "GetResultCallback.h"
#include "utils.h"
namespace rockchip::hardware::neuralnetworks::implementation {

// Methods from ::rockchip::hardware::neuralnetworks::V1_0::IGetResultCallback follow.
Return<void> GetResultCallback::notify(::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus status, const ::rockchip::hardware::neuralnetworks::V1_0::Response& response) {
    RECORD_TAG();
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//IGetResultCallback* HIDL_FETCH_IGetResultCallback(const char* /* name */) {
    //return new GetResultCallback();
//}
//
}  // namespace rockchip::hardware::neuralnetworks::implementation
