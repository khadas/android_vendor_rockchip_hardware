// FIXME: your file license if you have one

#pragma once

#include <rockchip/hardware/neuralnetworks/1.0/IRKNeuralnetworks.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "LoadModelCallback.h"
#include "GetResultCallback.h"

namespace rockchip::hardware::neuralnetworks::implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct RKNeuralnetworks : public V1_0::IRKNeuralnetworks {
    // Methods from ::rockchip::hardware::neuralnetworks::V1_0::IRKNeuralnetworks follow.
    Return<void> rknnInit(const ::rockchip::hardware::neuralnetworks::V1_0::RKNNModel& model, uint32_t size, uint32_t flag, rknnInit_cb _hidl_cb) override;
    Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> rknnDestory(uint64_t context) override;
    Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> rknnQuery(uint64_t context, ::rockchip::hardware::neuralnetworks::V1_0::RKNNQueryCmd cmd, const hidl_memory& info, uint32_t size) override;
    Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> rknnInputsSet(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::Request& request) override;
    Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> rknnRun(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::RKNNRunExtend& extend) override;
    Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> rknnOutputsGet(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::Response& response, const ::rockchip::hardware::neuralnetworks::V1_0::RKNNOutputExtend& extend) override;
    Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> rknnOutputsRelease(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::Response& response) override;
    Return<void> registerCallback(const sp<::rockchip::hardware::neuralnetworks::V1_0::ILoadModelCallback>& loadCallback, const sp<::rockchip::hardware::neuralnetworks::V1_0::IGetResultCallback>& getCallback) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
private:
    LoadModelCallback _load_cb;
    GetResultCallback _get_cb;
#ifdef __arm__
    uint32_t ctx;
#else
    uint64_t ctx;
#endif
};

extern "C" V1_0::IRKNeuralnetworks* HIDL_FETCH_IRKNeuralnetworks(const char* name);

}  // namespace rockchip::hardware::neuralnetworks::implementation
