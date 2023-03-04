# VulkanTracer (RAY-X)

The VulkanTracer is a ray tracing module using VULKAN by KHRONOS GROUP to efficiently trace rays with hardware acceleration.

In the current version the Vulkan Engine is a Compute Class that runs as much parallel as possible on the GPU.

Current procedure:

* generate rays
* create input and output buffers
* initialize Vulkan
* run the main loop
* clean up

initVulkan():

* create a Vulkan instance
* set up the debug messenger
* pick the physical device and create a logical device to access it
* create the input and output buffers
* create the descriptors to connect the buffers to the shader
* create a compute pipeline and a command buffer for the shader

mainLoop():

* run the command buffer
* read the data from the output buffer
---
# **How the VulkanTracer works**

![vulkan_flow](/docs/src/res/vulkan_flow.png)

## _Buffer types used:_

**A uniform buffer** (`VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER`) is a descriptor type associated with a buffer resource directly, described in a shader as a structure with various members that load operations can be performed on. [More here](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#descriptorsets-storagebuffer)

**A storage buffer** (`VK_DESCRIPTOR_TYPE_STORAGE_BUFFER`) is a descriptor type associated with a buffer resource directly, described in a shader as a structure with various members that load, store, and atomic operations can be performed on.

---
# Buffers:
| Buffers | Size ( vkDeviceSize)              | Usage                        | Name               |
|---------|-----------------------------------|------------------------------|--------------------|
| 0       | NumverOfRays*RAY_DOUBLE_AMOUNT    | Transfer_DST\|STORAGE_BUFFER | Ray Buffer         |
| 1       | NumberOfRays*RAY_DOUBLE_AMOUNT    | Transfer_SRC\|STORAGE_BUFFER | Output Buffer      |
| 2       | Quadric_parm+beamlineSize         | STORAGE_BUFFER               | Quadric Buffer     |
| 3       | 100                               | STORAGE_BUFFER               | Buffer for xyznull |
| 4       | xxxxxxxxxxxxxxx                   | STORAGE_BUFFER               | materialIndexBuf   |
| 5       | xxxxxxxxxxxxxxx                   | STORAGE_BUFFER               | materialBuf        |
| 6       | xxxxxxxxxxxxxxx                   | STORAGE_BUFFER               | debugBuffer        |
| X       | min(GPU_MAX_STAGING,numberOfRays) | STORAGE\|DST\|SRC            | Staging Buffer     |





## Buffer Usages:

`VK_BUFFER_USAGE_TRANSFER_SRC_BIT` specifies that the buffer can be used as the source of a transfer command (see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT).

`VK_BUFFER_USAGE_TRANSFER_DST_BIT` specifies that the buffer can be used as the destination of a transfer command.
