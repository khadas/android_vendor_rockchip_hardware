// FIXME: your file license if you have one

#include "RKNeuralnetworks.h"
#include "utils.h"

#ifndef IMPL_RKNN
#define IMPL_RKNN 0
#else
#define IMPL_RKNN 1
#endif

#if IMPL_RKNN
#include "prebuilts/librknnrt/include/rknn_api.h"
#endif

#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>

using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::hidl_memory;
using namespace std;

namespace rockchip::hardware::neuralnetworks::implementation {

V1_0::ErrorStatus toErrorStatus(int ret) {
    return (V1_0::ErrorStatus)ret;
}

static rknn_query_cmd to_rknnapi(V1_0::RKNNQueryCmd cmd) {
    switch (cmd) {
        case V1_0::RKNNQueryCmd::RKNN_QUERY_IN_OUT_NUM:
            return RKNN_QUERY_IN_OUT_NUM;
        case V1_0::RKNNQueryCmd::RKNN_QUERY_INPUT_ATTR:
            return RKNN_QUERY_INPUT_ATTR;
        case V1_0::RKNNQueryCmd::RKNN_QUERY_OUTPUT_ATTR:
            return RKNN_QUERY_OUTPUT_ATTR;
        case V1_0::RKNNQueryCmd::RKNN_QUERY_PERF_DETAIL:
            return RKNN_QUERY_PERF_DETAIL;
        case V1_0::RKNNQueryCmd::RKNN_QUERY_PERF_RUN:
            return::RKNN_QUERY_PERF_RUN;
        case V1_0::RKNNQueryCmd::RKNN_QUERY_SDK_VERSION:
            return RKNN_QUERY_SDK_VERSION;
        default:
            return RKNN_QUERY_CMD_MAX;
    }
}

static rknn_tensor_type to_rknnapi(V1_0::RKNNTensorType type) {
    switch (type) {
        case V1_0::RKNNTensorType::RKNN_TENSOR_FLOAT32:
            return RKNN_TENSOR_FLOAT32;
        case V1_0::RKNNTensorType::RKNN_TENSOR_FLOAT16:
            return RKNN_TENSOR_FLOAT16;
        case V1_0::RKNNTensorType::RKNN_TENSOR_INT8:
            return RKNN_TENSOR_INT8;
        case V1_0::RKNNTensorType::RKNN_TENSOR_UINT8:
            return RKNN_TENSOR_UINT8;
        case V1_0::RKNNTensorType::RKNN_TENSOR_INT16:
            return RKNN_TENSOR_INT16;
        default:
            return RKNN_TENSOR_TYPE_MAX;
    }
}

static rknn_tensor_format to_rknnapi(V1_0::RKNNTensorFormat format) {
    switch (format) {
        case V1_0::RKNNTensorFormat::RKNN_TENSOR_NCHW:
            return RKNN_TENSOR_NCHW;
        case V1_0::RKNNTensorFormat::RKNN_TENSOR_NHWC:
            return RKNN_TENSOR_NHWC;
        default:
            return RKNN_TENSOR_FORMAT_MAX;
    }
}

// Methods from ::rockchip::hardware::neuralnetworks::V1_0::IRKNeuralnetworks follow.
Return<void> RKNeuralnetworks::rknnInit(const ::rockchip::hardware::neuralnetworks::V1_0::RKNNModel& model, uint32_t size, uint32_t flag, rknnInit_cb _hidl_cb) {
    RECORD_TAG();
    g_debug_pro = property_get_bool("persist.vendor.rknndebug", false);
    sp<IMemory> pMem = mapMemory(model.modelData);
    void *pData = pMem->getPointer();
    ALOGI("%s: %s", __func__, (char *)pData);
#if IMPL_RKNN
    ret = rknn_init(&ctx, pData, size, flag);
#else
    ALOGI("%s: %s", __func__, (char *)pData);
#endif
    _hidl_cb(toErrorStatus(ret), ctx);
    return Void();
}

Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> RKNeuralnetworks::rknnDestory(uint64_t context) {
    RECORD_TAG();
#if IMPL_RKNN
    //CheckContext();
    ret = rknn_destroy(context);
#endif
    return ::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus {toErrorStatus(ret)};
}

Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> RKNeuralnetworks::rknnQuery(uint64_t context, ::rockchip::hardware::neuralnetworks::V1_0::RKNNQueryCmd cmd, const hidl_memory& info, uint32_t size) {
    RECORD_TAG();
#if IMPL_RKNN
    //CheckContext();
    sp<IMemory> pMem = mapMemory(info);
    void *pData = pMem->getPointer();
    pMem->update();
    ret = rknn_query(context, to_rknnapi(cmd), pData, size);
    pMem->commit();
#endif
    return ::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus {toErrorStatus(ret)};
}

Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> RKNeuralnetworks::rknnInputsSet(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::Request& request) {
    RECORD_TAG();
#if IMPL_RKNN
    sp<IMemory> pMem = mapMemory(request.pool);
    pMem->update();
    void *pData = pMem->getPointer();

    rknn_input temp_inputs[request.n_inputs];
    for (int i = 0; i < request.n_inputs; i++) {
        temp_inputs[i].index = request.inputs[i].index;
        temp_inputs[i].buf = request.inputs[i].buf.offset + (char *)pData;
        temp_inputs[i].size = request.inputs[i].buf.length;
        temp_inputs[i].pass_through = request.inputs[i].pass_through;
        temp_inputs[i].type = to_rknnapi(request.inputs[i].type);
        temp_inputs[i].fmt = to_rknnapi(request.inputs[i].fmt);
    }
    ret = rknn_inputs_set(context, request.n_inputs, temp_inputs);
    pMem->commit();
#endif
    return ::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus {toErrorStatus(ret)};
}

Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> RKNeuralnetworks::rknnRun(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::RKNNRunExtend& extend) {
    RECORD_TAG();
#if IMPL_RKNN
    rknn_run_extend temp_extend = {
        .frame_id = extend.frame_id,
    };
    ret = rknn_run(context, &temp_extend);
#endif
    return ::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus {toErrorStatus(ret)};
}

Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> RKNeuralnetworks::rknnOutputsGet(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::Response& response, const ::rockchip::hardware::neuralnetworks::V1_0::RKNNOutputExtend& extend) {
    RECORD_TAG();
#if IMPL_RKNN
    //CheckContext();
    sp<IMemory> pMem = mapMemory(response.pool);
    pMem->update();
    void *pData = pMem->getPointer();

    rknn_output pOutputs[response.n_outputs];
    for (int i = 0; i < response.n_outputs; i++) {
        pOutputs[i].want_float = response.outputs[i].want_float;
        pOutputs[i].is_prealloc = response.outputs[i].is_prealloc;
        pOutputs[i].buf = response.outputs[i].buf.offset + (char *)pData;
        pOutputs[i].size = response.outputs[i].buf.length;
        pOutputs[i].index = i;
    }
    rknn_output_extend temp_extend = {
        .frame_id = extend.frame_id,
    };
    ret = rknn_outputs_get(context, response.n_outputs, pOutputs, &temp_extend);
    // commit to response.
    pMem->commit();
#endif
    return ::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus {toErrorStatus(ret)};
}

Return<::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus> RKNeuralnetworks::rknnOutputsRelease(uint64_t context, const ::rockchip::hardware::neuralnetworks::V1_0::Response& response) {
    RECORD_TAG();
#if IMPL_RKNN
    //CheckContext();
    sp<IMemory> pMem = mapMemory(response.pool);
    pMem->update();
    void *pData = pMem->getPointer();

    rknn_output pOutputs[response.n_outputs];
    for (int i = 0; i < response.n_outputs; i++) {
        pOutputs[i].want_float = response.outputs[i].want_float;
        pOutputs[i].is_prealloc = response.outputs[i].is_prealloc;
        pOutputs[i].buf = response.outputs[i].buf.offset + (char *)pData;
        pOutputs[i].size = response.outputs[i].buf.length;
        pOutputs[i].index = i;
    }
    ret = rknn_outputs_release(context, response.n_outputs, pOutputs);
    pMem->commit();
#endif
    return ::rockchip::hardware::neuralnetworks::V1_0::ErrorStatus {toErrorStatus(ret)};
}

Return<void> RKNeuralnetworks::registerCallback(const sp<::rockchip::hardware::neuralnetworks::V1_0::ILoadModelCallback>& loadCallback, const sp<::rockchip::hardware::neuralnetworks::V1_0::IGetResultCallback>& getCallback) {
    RECORD_TAG();
#if IMPL_RKNN
    if (loadCallback != nullptr) {
        ALOGI("Register LoadCallback Successfully!");
        // TODO set callback
        //_load_cb = loadCallback;
    } else {
        ALOGE("Register LoadCallback Failed!");
    }
    if (getCallback != nullptr) {
        ALOGI("Register GetCallback Successfully!");
        // TODO set callback
        //_get_cb = getCallback;
    } else {
        ALOGE("Register GetCallback Failed!");
    }
#endif
    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

V1_0::IRKNeuralnetworks* HIDL_FETCH_IRKNeuralnetworks(const char* /* name */) {
    RECORD_TAG();
#if IMPL_RKNN
    ALOGI("Linked RKNeuralnetworks and rknn_api.");
#endif
    return new RKNeuralnetworks();
}
//
}  // namespace rockchip::hardware::neuralnetworks::implementation
