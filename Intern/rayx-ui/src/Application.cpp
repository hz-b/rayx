#include "Application.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>

#include "Data/Importer.h"
#include "ImGuiLayer.h"
#include "Writer/H5Writer.h"

// --------- Start of Application code --------- //
Application::Application(uint32_t width, uint32_t height, const char* name)
    : m_Window(width, height, name),                  //
      m_Device(m_Window),                             //
      m_SwapChain(m_Device, m_Window.getExtent()),    //
      m_ImGuiLayer(m_Window, m_Device, m_SwapChain),  //
      m_Scene(m_Device) {
    // Building the descriptor pool
    m_DescriptorPool = DescriptorPool::Builder(m_Device)
                           .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                           .build();
    // TODO: Move the rest of Vulkan initialization to its own classes
    initVulkan();
}

Application::~Application() {
    vkDestroyPipelineLayout(m_Device.device(), m_TrianglePipelineLayout, nullptr);
    vkDestroyPipelineLayout(m_Device.device(), m_LinePipelineLayout, nullptr);
    vkDestroyPipelineLayout(m_Device.device(), m_GridPipelineLayout, nullptr);

    m_TrianglePipeline->~GraphicsPipeline();
    m_LinePipeline->~GraphicsPipeline();
    m_GridPipeline->~GraphicsPipeline();

    vkDestroyRenderPass(m_Device.device(), m_SwapChain.getRenderPass(), nullptr);

    // TODO
    // for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    //     vkDestroyBuffer(m_Device.device(), m_UniformBuffers[i], nullptr);
    //     vkFreeMemory(m_Device.device(), m_UniformBuffersMemory[i], nullptr);
    // }

    m_VertexBuffer->~VertexBuffer();

    vkDestroyDevice(m_Device.device(), nullptr);
}

void Application::run() {
    // Get the render data
    auto vec = RAYX::RenderObjectVec(RAYX::getRenderData("PlaneMirror.rml"));
    RAYX::BundleHistory rays;
    readH5(rays, "PlaneMirror.h5", FULL_FORMAT);
    m_Scene.setup(vec, rays);

    // UBOs
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] =
            std::make_unique<Buffer>(m_Device, sizeof(Camera), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    // Descriptor set layout
    // auto m_DescriptorSetLayout = DescriptorSetLayout::Builder(m_Device)
    //                                  .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
    //                                  .build();                                                                      //

    std::vector<VkDescriptorSet> descriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < descriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*m_DescriptorSetLayout, *m_DescriptorPool).writeBuffer(0, &bufferInfo).build(descriptorSets[i]);
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    while (!m_Window.shouldClose()) {
        glfwPollEvents();

        m_ImGuiLayer.updateImGui();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        auto cameraSettings = m_ImGuiLayer.getCameraSettings();
        Camera camera = {
            .model = glm::mat4(1.0f),
            .view = cameraSettings.getViewMatrix(),
            .proj = cameraSettings.getProjectionMatrix(m_SwapChain.extentAspectRatio()),
            .n = cameraSettings.near,
            .f = cameraSettings.far,
        };

        FrameInfo frameInfo{m_CurrentFrame, frameTime, m_CommandBuffers[m_CurrentFrame], camera, descriptorSets[m_CurrentFrame]};

        // Update ubo
        uboBuffers[m_CurrentFrame]->writeToBuffer(&camera);
        uboBuffers[m_CurrentFrame]->flush();

        // Begin
        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain.getRenderPass();
        renderPassInfo.framebuffer = m_SwapChain.getFrameBuffer(m_CurrentFrame);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChain.getExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain.getExtent().width);
        viewport.height = static_cast<float>(m_SwapChain.getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain.getExtent()};
        vkCmdSetViewport(m_CommandBuffers[m_CurrentFrame], 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffers[m_CurrentFrame], 0, 1, &scissor);

        // Draw
        vkCmdBindPipeline(m_CommandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline->getHandle());
        vkCmdBindDescriptorSets(m_CommandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipelineLayout, 0, 1, &descriptorSets[0],
                                0, nullptr);
        vkCmdBindIndexBuffer(m_CommandBuffers[m_CurrentFrame], m_Scene.getIndexBuffer(Scene::Topography::TRIA_TOPOGRAPHY), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(m_CommandBuffers[m_CurrentFrame], (uint32_t)m_Scene.getIndexCount(Scene::Topography::TRIA_TOPOGRAPHY), 1, 0, 0, 0);

        // End
        vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);
        m_CurrentFrame = (m_CurrentFrame + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(m_Device.device());
}

void Application::initVulkan() {
    createGraphicsPipelineLayouts();
    createGraphicsPipelines();
    createCommandBuffers();
}

VkPipelineLayout createGraphicsPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout) {
    VkPipelineLayout pipelineLayout;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    return pipelineLayout;
}

void Application::createGraphicsPipelineLayouts() {
    // TODO: This should not be here
    m_DescriptorSetLayout = DescriptorSetLayout::Builder(m_Device)
                                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  //
                                .build();                                                                      //

    m_TrianglePipelineLayout = createGraphicsPipelineLayout(m_Device.device(), m_DescriptorSetLayout->getDescriptorSetLayout());
    m_LinePipelineLayout = createGraphicsPipelineLayout(m_Device.device(), m_DescriptorSetLayout->getDescriptorSetLayout());
    m_GridPipelineLayout = createGraphicsPipelineLayout(m_Device.device(), m_DescriptorSetLayout->getDescriptorSetLayout());
}

void Application::createGraphicsPipelines() {
    // --- TRIANGLE PIPELINE ---
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    GraphicsPipelineCreateInfo trianglePipelineCreateInfo = {
        .renderPass = m_SwapChain.getRenderPass(),  //
        .pipelineLayout = m_TrianglePipelineLayout,
        .vertShaderPath = "../../../Intern/rayx-ui/src/Shaders/vert.spv",
        .fragShaderPath = "../../../Intern/rayx-ui/src/Shaders/frag.spv",
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .vertexInputInfo = vertexInputInfo,
    };
    m_TrianglePipeline = std::make_unique<GraphicsPipeline>(m_Device, trianglePipelineCreateInfo);

    // --- LINE PIPELINE ---
    GraphicsPipelineCreateInfo linePipelineCreateInfo = {
        .renderPass = m_SwapChain.getRenderPass(),  //
        .pipelineLayout = m_LinePipelineLayout,
        .vertShaderPath = "../../../Intern/rayx-ui/src/Shaders/vert.spv",
        .fragShaderPath = "../../../Intern/rayx-ui/src/Shaders/frag.spv",
        .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        .polygonMode = VK_POLYGON_MODE_LINE,
        .vertexInputInfo = vertexInputInfo,
    };
    m_LinePipeline = std::make_unique<GraphicsPipeline>(m_Device, linePipelineCreateInfo);

    // No vertex bindings for the grid pipeline
    vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;       // No vertex bindings
    vertexInputInfo.pVertexBindingDescriptions = nullptr;    // No vertex bindings
    vertexInputInfo.vertexAttributeDescriptionCount = 0;     // No vertex attributes
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;  // No vertex attributes

    // --- GRID PIPELINE ---
    GraphicsPipelineCreateInfo gridPipelineCreateInfo = {
        .renderPass = m_SwapChain.getRenderPass(),  //
        .pipelineLayout = m_GridPipelineLayout,
        .vertShaderPath = "../../../Intern/rayx-ui/src/Shaders/vertGrid.spv",
        .fragShaderPath = "../../../Intern/rayx-ui/src/Shaders/fragGrid.spv",
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .vertexInputInfo = vertexInputInfo,
    };
    m_GridPipeline = std::make_unique<GraphicsPipeline>(m_Device, gridPipelineCreateInfo);
}

void Application::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_Device.getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_Device.device(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_Device.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_Device.graphicsQueue());

    vkFreeCommandBuffers(m_Device.device(), m_Device.getCommandPool(), 1, &commandBuffer);
}

void Application::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                               VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_Device.device(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Device.device(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_Device.device(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_Device.device(), buffer, bufferMemory, 0);
}

void Application::createCommandBuffers() {
    m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Device.getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

    if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkClearDepthStencilValue clearDepthStencilValue = {};
    clearDepthStencilValue.depth = 1.0f;  // Set the depth clear value to 1.0
    clearDepthStencilValue.stencil = 0;   // You can set this to your desired stencil clear value

    VkClearValue clearColor = m_ImGuiLayer.getClearValue();
    clearColor.depthStencil = clearDepthStencilValue;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_SwapChain.getRenderPass();
    renderPassInfo.framebuffer = m_SwapChain.getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_SwapChain.getExtent();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Triangle pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline->getHandle());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_SwapChain.getExtent().width;
    viewport.height = (float)m_SwapChain.getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_SwapChain.getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {m_VertexBuffer->getVertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // TODO m_DescriptorSets[0]
    // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipelineLayout, 0, 1, &m_DescriptorSets[0], 0, nullptr);
    // vkCmdBindIndexBuffer(commandBuffer, m_TriangleIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
    // vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Scene.getIndices(Scene::TRIA_TOPOGRAPHY).size()), 1, 0, 0, 0);

    // TODO
    // Line pipeline
    // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_LinePipeline->getHandle());
    // vkCmdBindIndexBuffer(commandBuffer, m_LineIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
    // vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Scene.getIndices(Scene::LINE_TOPOGRAPHY).size()), 1, 0, 0, 0);

    // TODO
    // Grid pipeline
    // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GridPipeline->getHandle());
    // vkCmdDraw(commandBuffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Application::drawFrame() {
    uint32_t imageIndex;
    VkResult result = m_SwapChain.acquireNextImage(&imageIndex);

    // TODO
    // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    //    recreateSwapChain();
    //    return;
    // } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    //    throw std::runtime_error("failed to acquire swap chain image!");
    // }

    // TODO
    // updateUniformBuffer(imageIndex);

    // Render Frame
    vkResetCommandBuffer(m_CommandBuffers[imageIndex], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(m_CommandBuffers[imageIndex], imageIndex);

    // TODO
    // auto ImGuiCmdBuffer = m_ImGuiLayer.recordImGuiCommands(m_currentFrame, m_SwapChain.framebuffers[m_currentFrame], m_SwapChain.Extent);

    m_SwapChain.submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
}