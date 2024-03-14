#pragma once
#include <vector>

#include <vulkan/vulkan.h>
#include <swap_chain.h>

namespace shiny
{
class CommandBuffer
{
public:
	CommandBuffer(
		VkPhysicalDevice* physical_device, VkSurfaceKHR* surface,
		VkDevice* logical_device, VkRenderPass* render_pass,
		SwapChain* swap_chain, VkPipeline* pipeline);
	void Destroy();
private:
	void CreateCommandPool();
	void CreateCommandBuffer();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	VkCommandPool command_pool_ = nullptr;
	std::vector<VkCommandBuffer> command_buffers_ = std::vector<VkCommandBuffer>();

	VkPhysicalDevice* physical_device_ = nullptr;
	VkSurfaceKHR* surface_ = nullptr;
	VkDevice* logical_device_ = nullptr;
	VkRenderPass* render_pass_ = nullptr;

	SwapChain* swap_chain_ = nullptr;
	VkExtent2D* swap_chain_extent_ = nullptr;

	VkPipeline* pipeline_ = nullptr;

	const int kMaxFramesnFlight = 2;
};
}