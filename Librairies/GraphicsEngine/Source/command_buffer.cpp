
#include <command_buffer.h>
#include <stdexcept>
#include <swap_chain.h>

namespace shiny
{
	void CommandBuffer::InitCommandBuffer(
		VkPhysicalDevice* physical_device, VkSurfaceKHR* surface,
		VkDevice* logical_device, VkRenderPass* render_pass,
		SwapChain* swap_chain, VkPipeline* pipeline) 
	{
		physical_device_ = physical_device;
		surface_ = surface;
		logical_device_ = logical_device;
		render_pass_ = render_pass;
		swap_chain_ = swap_chain;
		pipeline_ = pipeline;
		swap_chain_extent_ = &swap_chain_->GetExtent();

		CreateCommandPool();
		CreateCommandBuffer();
	}

	void CommandBuffer::Destroy() {
		vkDestroyCommandPool(*logical_device_, command_pool_, nullptr);
	}

	void CommandBuffer::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::FindQueueFamilies(*physical_device_, *surface_);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(*logical_device_, &poolInfo, nullptr, &command_pool_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void CommandBuffer::CreateCommandBuffer()
	{
		command_buffers_.resize(kMaxFramesnFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = command_pool_;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)command_buffers_.size();

		if (vkAllocateCommandBuffers(*logical_device_, &allocInfo, command_buffers_.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void CommandBuffer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VertexBuffer& vertexBuffer)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = *render_pass_;
		renderPassInfo.framebuffer = swap_chain_->GetSwapChainFrameBuffers()->at(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = *swap_chain_extent_;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline_);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swap_chain_extent_->width);
		viewport.height = static_cast<float>(swap_chain_extent_->height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = *swap_chain_extent_;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { *vertexBuffer.GetVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexBuffer.GetVerticesSize()), 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}