#include <rockchip/hardware/neuralnetworks/1.0/IRKNeuralnetworks.h>
#include <log/log.h>
#include <iostream>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>

#include <errno.h>
#include "../default/prebuilts/librknnrt/include/rknn_api.h"

#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <cstddef>
#include <algorithm>
#include <queue>
#include <sys/time.h>

#include "RockchipNeuralnetworksBuilder.h"
#include "HalInterfaces.h"
#include "rknnhal_bridge.h"
#include <cutils/properties.h>

using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hardware::hidl_memory;

using namespace std;
using namespace rockchip::hardware::neuralnetworks::V1_0;
using namespace android;

using namespace rockchip::nn;

using ::android::hardware::Return;
using ::android::hardware::Void;
static bool g_debug_pro = 0;

#define CHECK_AND_GET_CLIENT() \
    if (g_debug_pro) ALOGE("%s", __func__); \
    if (!hal) { \
        ALOGE("Hal obj is nullptr!"); \
        return -1; \
    } \
    RockchipNeuralnetworksBuilder* client = reinterpret_cast<RockchipNeuralnetworksBuilder*>(hal)

// From rknn api
int ARKNN_client_create(ARKNNHAL **hal) {
    g_debug_pro = property_get_bool("persist.vendor.rknndebug", false);
    ALOGE("%s %d", __func__, g_debug_pro);

    auto nnb = std::make_unique<RockchipNeuralnetworksBuilder>();
    *hal = reinterpret_cast<ARKNNHAL*>(nnb.release());
    return 0;
}

int ARKNN_init(ARKNNHAL *hal, rknn_context* context, void *model, uint32_t size, uint32_t flag) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_init(context, model, size, flag);
    return ret;
}

int ARKNN_destroy(ARKNNHAL *hal, rknn_context context) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_destroy(context);
    return ret;
}

int ARKNN_query(ARKNNHAL *hal, rknn_context context, rknn_query_cmd cmd, void* info, uint32_t size) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_query(context, cmd, info, size);
    return ret;
}

int ARKNN_inputs_set(ARKNNHAL *hal, rknn_context context, uint32_t n_inputs, rknn_input inputs[]) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_inputs_set(context, n_inputs, inputs);
    return ret;
}

int ARKNN_run(ARKNNHAL *hal, rknn_context context, rknn_run_extend* extend) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_run(context, extend);
    return ret;
}

int ARKNN_outputs_get(ARKNNHAL *hal, rknn_context context, uint32_t n_outputs, rknn_output outputs[], rknn_output_extend* extend) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_outputs_get(context, n_outputs, outputs, extend);
    return ret;
}

int ARKNN_outputs_release(ARKNNHAL *hal, rknn_context context, uint32_t n_outputs, rknn_output outputs[]) {
    CHECK_AND_GET_CLIENT();
    int ret = client->rknn_outputs_release(context, n_outputs, outputs);
    return ret;
}
