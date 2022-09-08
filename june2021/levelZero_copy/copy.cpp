#include "ze_api.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

/**
 * How to compile?
 *  gcc -std=c++14 -O0 -fpermissive -rdynamic -fPIC copyLevelZeroSPIRV.cpp -o copyLevelZeroSPIRV /path/to/libze_loader.so -lstdc++
 *
 *
 * OpenCL Kernel:
 *
 * __kernel void initValues(__global long* input, __global long* output) {
 *      uint idx = get_global_id(0);
 *       output[idx] = input[idx];
 * }
 *
 * Compile OpenCL to SPIRV ($1 is the name of the file):
 *
 * $ clang -cc1 -triple spir $1.cl -O0 -finclude-default-header -emit-llvm-bc -o $1.bc
 * $ llvm-spirv $1.bc -o $1.spv
 *
 */

#define VALIDATECALL(myZeCall) \
    if (myZeCall != ZE_RESULT_SUCCESS){ \
        std::cout << "Error at "       \
            << #myZeCall << ": "       \
            << __FUNCTION__ << ": "    \
            << __LINE__ << std::endl;  \
        std::terminate(); \
    }

int main(int argc, char **argv) {

    const long value = 120012;

    // Initialization
    VALIDATECALL(zeInit(ZE_INIT_FLAG_GPU_ONLY));

    // Get the driver
    uint32_t driverCount = 0;
    VALIDATECALL(zeDriverGet(&driverCount, nullptr));

    ze_driver_handle_t driverHandle;
    VALIDATECALL(zeDriverGet(&driverCount, &driverHandle));

    // Create the context
    ze_context_desc_t contextDescription = {};
    contextDescription.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC;
    ze_context_handle_t context;
    VALIDATECALL(zeContextCreate(driverHandle, &contextDescription, &context));

    // Get the device
    uint32_t deviceCount = 0;
    VALIDATECALL(zeDeviceGet(driverHandle, &deviceCount, nullptr));

    ze_device_handle_t device;
    VALIDATECALL(zeDeviceGet(driverHandle, &deviceCount, &device));

    // Print some properties of the device
    ze_device_properties_t deviceProperties = {};
    VALIDATECALL(zeDeviceGetProperties(device, &deviceProperties));

    std::cout << "Device   : " << deviceProperties.name << "\n"
              << "Type     : " << ((deviceProperties.type == ZE_DEVICE_TYPE_GPU) ? "GPU" : "FPGA") << "\n"
              << "Vendor ID: " << std::hex << deviceProperties.vendorId << std::dec << "\n";

    // Create a command queue
    uint32_t numQueueGroups = 0;
    VALIDATECALL(zeDeviceGetCommandQueueGroupProperties(device, &numQueueGroups, nullptr));
    if (numQueueGroups == 0) {
        std::cout << "No queue groups found\n";
        std::terminate();
    } else {
        std::cout << "#Queue Groups: " << numQueueGroups << std::endl;
    }
    std::vector<ze_command_queue_group_properties_t> queueProperties(numQueueGroups);
    VALIDATECALL(zeDeviceGetCommandQueueGroupProperties(device, &numQueueGroups, queueProperties.data()));

    ze_command_queue_handle_t cmdQueue;
    ze_command_queue_desc_t cmdQueueDesc = {};

    for (uint32_t i = 0; i < numQueueGroups; i++) {
        if (queueProperties[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) {
            cmdQueueDesc.ordinal = i;
        }
    }

    cmdQueueDesc.index = 0;
    cmdQueueDesc.mode = ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;
    VALIDATECALL(zeCommandQueueCreate(context, device, &cmdQueueDesc, &cmdQueue));

    // Create a command list
    ze_command_list_handle_t cmdList;
    ze_command_list_desc_t cmdListDesc = {};
    cmdListDesc.commandQueueGroupOrdinal = cmdQueueDesc.ordinal;
    VALIDATECALL(zeCommandListCreate(context, device, &cmdListDesc, &cmdList));

    // Create two buffers
    const int items = 1;
    constexpr size_t allocSize = items * sizeof(long);
    ze_device_mem_alloc_desc_t memAllocDesc;
    memAllocDesc.flags = ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED;
    memAllocDesc.ordinal = 0;

    void *inputBuffer = nullptr;
    VALIDATECALL(zeMemAllocDevice(context, &memAllocDesc, allocSize, 1, device, &inputBuffer));

    void *outputBuffer = nullptr;
    VALIDATECALL(zeMemAllocDevice(context, &memAllocDesc, allocSize, 1, device, &outputBuffer));

    std::vector<uint64_t> input(items);
    for (int i = 0; i < items; i++) {
        input[i] = value;
    }

    VALIDATECALL(zeCommandListAppendMemoryCopy(cmdList, inputBuffer, input.data(), allocSize, nullptr, 0, nullptr));
    VALIDATECALL(zeCommandListAppendBarrier(cmdList, nullptr, 0, nullptr));


    // Module Initialization
    ze_module_handle_t module = nullptr;
    ze_kernel_handle_t kernel = nullptr;
    std::ifstream file("opencl-copy.spv", std::ios::binary);

    if (file.is_open()) {
        file.seekg(0, file.end);
        auto length = file.tellg();
        file.seekg(0, file.beg);

        std::unique_ptr<char[]> spirvInput(new char[length]);
        file.read(spirvInput.get(), length);

        ze_module_desc_t moduleDesc = {};
        ze_module_build_log_handle_t buildLog;
        moduleDesc.format = ZE_MODULE_FORMAT_IL_SPIRV;
        moduleDesc.pInputModule = reinterpret_cast<const uint8_t *>(spirvInput.get());
        moduleDesc.inputSize = length;
        moduleDesc.pBuildFlags = "";

        auto status = zeModuleCreate(context, device, &moduleDesc, &module, &buildLog);
        if (status != ZE_RESULT_SUCCESS) {
            size_t szLog = 0;
            zeModuleBuildLogGetString(buildLog, &szLog, nullptr);
            char* stringLog = (char*)malloc(szLog);
            zeModuleBuildLogGetString(buildLog, &szLog, stringLog);
            std::cout << "Build log: " << stringLog << std::endl;
        }
        VALIDATECALL(zeModuleBuildLogDestroy(buildLog));

        ze_kernel_desc_t kernelDesc = {};
        kernelDesc.pKernelName = "initValues";
        VALIDATECALL(zeKernelCreate(module, &kernelDesc, &kernel));

        uint32_t groupSizeX = 1u;
        uint32_t groupSizeY = 1u;
        uint32_t groupSizeZ = 1u;
        VALIDATECALL(zeKernelSuggestGroupSize(kernel, items, 1U, 1U, &groupSizeX, &groupSizeY, &groupSizeZ));
        VALIDATECALL(zeKernelSetGroupSize(kernel, groupSizeX, groupSizeY, groupSizeZ));

        // Push arguments
        VALIDATECALL(zeKernelSetArgumentValue(kernel, 0, sizeof(inputBuffer), &inputBuffer));
        VALIDATECALL(zeKernelSetArgumentValue(kernel, 1, sizeof(outputBuffer), &outputBuffer));

        // Kernel thread-dispatch
        ze_group_count_t dispatch;
        dispatch.groupCountX = 1u;
        dispatch.groupCountY = 1u;
        dispatch.groupCountZ = 1u;

        // Launch kernel on the GPU
        VALIDATECALL(zeCommandListAppendLaunchKernel(cmdList, kernel, &dispatch, nullptr, 0, nullptr));
        VALIDATECALL(zeCommandListAppendBarrier(cmdList, nullptr, 0, nullptr));

        file.close();
    } else {
        std::cout << "SPIR-V binary file not found\n";
        std::terminate();
    }


    std::vector<uint64_t> output(items);

    VALIDATECALL(zeCommandListAppendMemoryCopy(cmdList, output.data(), outputBuffer, allocSize, nullptr, 0, nullptr));
    VALIDATECALL(zeCommandListAppendBarrier(cmdList, nullptr, 0, nullptr));

    // Close list abd submit for execution
    VALIDATECALL(zeCommandListClose(cmdList));
    VALIDATECALL(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr));
    VALIDATECALL(zeCommandQueueSynchronize(cmdQueue, std::numeric_limits<uint64_t>::max()));


    // Validate
    bool outputValidationSuccessful = true;

    for (int i = 0; i < items; i++) {
        std::cout << "Output: " << output[i] << std::endl;
        if (output[i] != value) {
            outputValidationSuccessful = false;
            break;
        }
    }

    std::cout << "\nZello World Results validation " << (outputValidationSuccessful ? "PASSED" : "FAILED") << "\n";

    // Cleanup
    VALIDATECALL(zeMemFree(context, inputBuffer));
    VALIDATECALL(zeMemFree(context, outputBuffer));
    VALIDATECALL(zeCommandListDestroy(cmdList));
    VALIDATECALL(zeCommandQueueDestroy(cmdQueue));
    VALIDATECALL(zeContextDestroy(context));

    return 0;
}

