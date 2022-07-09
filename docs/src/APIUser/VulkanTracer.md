# VulkanTracer (RAY-X)

The VulkanTracer is a ray tracing module using VULKAN by KHRONOS GROUP to efficiently trace rays with hardware acceleration.

In the current version the VulkanTracer class is simply called using the run() function. In future versions information about the rays and optical elements will be transfered.

Current procedure:

* generate rays
* create input and output buffers
* initialize Vulkan using initVulkan()
* run the main loop using mainLoop()
* clean up (free the memory allocated)

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

![vulkan_flow](/docs/src/uploads/fe29b8ceeb1467bec87aed5de3144cad/vulkan_flow.png)

## _Buffer types used:_

**A uniform buffer** (`VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER`) is a descriptor type associated with a buffer resource directly, described in a shader as a structure with various members that load operations can be performed on. [More here](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#descriptorsets-storagebuffer)

**A storage buffer** (`VK_DESCRIPTOR_TYPE_STORAGE_BUFFER`) is a descriptor type associated with a buffer resource directly, described in a shader as a structure with various members that load, store, and atomic operations can be performed on.

---


**Vulkan commands (Recording and pushing to pipeline):**

![CommandBuffers](/docs/src/uploads/ed43760242d13d1004607fef0131747e/CommandBuffers.png)

---

**Layout concept (applied on "Sets" using `storage buffer|uniform buffer`):**

![DescriptorLayouts](/docs/src/uploads/090e2e5c162e12db1f5a93a1b30dbb79/DescriptorLayouts.png)

---
# Buffers:
| Buffers | Size ( in Double)                 | Usage                        | Memory Property   | Name               |
|---------|-----------------------------------|------------------------------|-------------------|--------------------|
| 0       | NumverOfRays*RAY_DOUBLE_AMOUNT    | Transfer_DST\|STORAGE_BUFFER | DEVICE_LOCAL      | Ray Buffer         |
| 1       | NumberOfRays*RAY_DOUBLE_AMOUNT    | Transfer_SRC\|STORAGE_BUFFER | DEVICE_LOCAL      | Output Buffer      |
| 2       | Quadric_parm+beamlineSize         | STORAGE_BUFFER               | COHERANT\|VISIBLE | Quadric Buffer     |
| 3       | min(GPU_MAX_STAGING,numberOfRays) | STORAGE\|DST\|SRC            | COHERANT\|VISIBLE | Staging Buffer     |
| 4       | numberofRays*4                    | STORAGE_BUFFER               | DEVICE_LOCAL      | Buffer for xyznull |

## Buffer Usages:

`VK_BUFFER_USAGE_TRANSFER_SRC_BIT` specifies that the buffer can be used as the source of a transfer command (see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT).

`VK_BUFFER_USAGE_TRANSFER_DST_BIT` specifies that the buffer can be used as the destination of a transfer command.
