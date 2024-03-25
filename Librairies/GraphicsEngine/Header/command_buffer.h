#pragma once
#include <vector>

#include <vulkan/vulkan.h>
#include <swap_chain.h>
#include <vertex_buffer.h>
#include <graphics_pipeline.h>

namespace shiny
{
class CommandBuffer
{
public:
	CommandBuffer() {}
	void InitCommandBuffer(
		VkPhysicalDevice* physical_device, VkSurfaceKHR* surface,
		VkDevice* logical_device, SwapChain* swap_chain, GraphicsPipeline* graphics_pipeline);
	void Destroy();

	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VertexBuffer& vertexBuffer, VkDescriptorSet* descriptor_set);




	VkCommandBuffer& GetCommandBuffer(uint32_t bufferIndex) { return command_buffers_[bufferIndex]; }
	VkCommandPool& GetCommandPool() { return command_pool_; }

	static const int kMaxFramesInFlight = 2;
private:
	void CreateCommandPool();
	void CreateCommandBuffer();

	VkCommandPool command_pool_ = nullptr;
	std::vector<VkCommandBuffer> command_buffers_ = std::vector<VkCommandBuffer>();

	VkPhysicalDevice* physical_device_ = nullptr;
	VkSurfaceKHR* surface_ = nullptr;
	VkDevice* logical_device_ = nullptr;
	VkRenderPass* render_pass_ = nullptr;

	SwapChain* swap_chain_ = nullptr;
	VkExtent2D* swap_chain_extent_ = nullptr;

	GraphicsPipeline* graphics_pipeline_ = nullptr;
};
}