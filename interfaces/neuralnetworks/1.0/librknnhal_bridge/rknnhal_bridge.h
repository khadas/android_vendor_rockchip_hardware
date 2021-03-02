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
#include <algorithm>
#include <queue>
#include <sys/time.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct ARKNNHAL ARKNNHAL;

// From rknn api
int ARKNN_client_create(ARKNNHAL **hal);
int ARKNN_init(ARKNNHAL *hal, rknn_context* context, void *model, uint32_t size, uint32_t flag);
int ARKNN_destroy(ARKNNHAL *hal, rknn_context context);
int ARKNN_query(ARKNNHAL *hal, rknn_context context, rknn_query_cmd cmd, void* info, uint32_t size);
int ARKNN_inputs_set(ARKNNHAL *hal, rknn_context context, uint32_t n_inputs, rknn_input inputs[]);
int ARKNN_run(ARKNNHAL *hal, rknn_context context, rknn_run_extend* extend);
int ARKNN_outputs_get(ARKNNHAL *hal, rknn_context context, uint32_t n_outputs, rknn_output outputs[], rknn_output_extend* extend);
int ARKNN_outputs_release(ARKNNHAL *hal, rknn_context context, uint32_t n_ouputs, rknn_output outputs[]);

__END_DECLS
