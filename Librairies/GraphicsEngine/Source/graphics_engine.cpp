#include <graphics_engine.h>

#include <stdexcept>
#include <cstdlib>
#include <cstdint> 
#define NOMINMAX
#include <limits> 
#include <algorithm> 


#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <swap_chain_support_details.h>
#include <queue_family_indices.h>

namespace shiny
{
	void GraphicsEngine::Run()
	{
		window_.InitWindow(kWidth, kHeight);
		InitVulkan();
		MainLoop();
		Cleanup();
	}

#pragma region Functions
#pragma region Main_loop
	void GraphicsEngine::InitVulkan()
	{
		instance_.InitInstance();

		validation_layers_.SetupDebugMessenger(instance_.GetInstance());

		surface_.InitSurface(instance_.GetInstance(), window_.GetWindow());

		vulkan_device_.InitVulkanDevice(&instance_.GetInstance(), &surface_.GetSurface());

		swap_chain_.InitSwapChain(window_.GetWindow(), &instance_.GetInstance(), &surface_.GetSurface(), &vulkan_device_.GetLogicalDevice(), &vulkan_device_.GetPhysicalDevice());

		uniform_buffer_.InitUniformBuffer(vulkan_device_.GetLogicalDevice(), vulkan_device_.GetPhysicalDevice());

		graphics_pipeline_.CreateGraphicsPipeline(&vulkan_device_.GetLogicalDevice(), &uniform_buffer_.GetDescriptorSetLayout(), &swap_chain_.GetExtent(), &swap_chain_.GetImageFormat());

		swap_chain_.CreateFrameBuffers(*graphics_pipeline_.GetRenderPass());

		command_buffer_.InitCommandBuffer(&vulkan_device_.GetPhysicalDevice(), &surface_.GetSurface(), &vulkan_device_.GetLogicalDevice(), &swap_chain_, &graphics_pipeline_);

		vertex_buffer_.InitVertexBuffer(&vulkan_device_.GetLogicalDevice(), &vulkan_device_.GetPhysicalDevice(), &command_buffer_.GetCommandPool(), &vulkan_device_.GetGraphicsQueue());

		CreateSyncObjects();
	}

	void GraphicsEngine::MainLoop()
	{
		while (!glfwWindowShouldClose(window_.GetWindow())) {
			glfwPollEvents();
			DrawFrame();
		}

		vkDeviceWaitIdle(vulkan_device_.GetLogicalDevice());
	}

	void GraphicsEngine::Cleanup()
	{
		swap_chain_.Destroy();

		vertex_buffer_.Destroy();

		for (size_t i = 0; i < kMaxFramesnFlight; i++) {
			vkDestroySemaphore(vulkan_device_.GetLogicalDevice(), render_finished_semaphore_[i], nullptr);
			vkDestroySemaphore(vulkan_device_.GetLogicalDevice(), image_available_semaphore_[i], nullptr);
			vkDestroyFence(vulkan_device_.GetLogicalDevice(), in_flight_fence_[i], nullptr);
		}

		command_buffer_.Destroy();

		graphics_pipeline_.Cleanup();

		uniform_buffer_.Destroy();

		vulkan_device_.Destroy();

		surface_.Destroy(instance_.GetInstance());

		validation_layers_.Destroy(instance_.GetInstance());

		instance_.Destroy();
		window_.Destroy();
	}
#pragma endregion Main_loop

#pragma region SyncObjects
	void GraphicsEngine::CreateSyncObjects()
	{
		image_available_semaphore_.resize(kMaxFramesnFlight);
		render_finished_semaphore_.resize(kMaxFramesnFlight);
		in_flight_fence_.resize(kMaxFramesnFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < kMaxFramesnFlight; i++) {
			if (vkCreateSemaphore(vulkan_device_.GetLogicalDevice(), &semaphoreInfo, nullptr, &image_available_semaphore_[i]) != VK_SUCCESS ||
				vkCreateSemaphore(vulkan_device_.GetLogicalDevice(), &semaphoreInfo, nullptr, &render_finished_semaphore_[i]) != VK_SUCCESS ||
				vkCreateFence(vulkan_device_.GetLogicalDevice(), &fenceInfo, nullptr, &in_flight_fence_[i]) != VK_SUCCESS) {

				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}
#pragma endregion SyncObjects

#pragma region Frame
	// Wait for the previous frame to finish
	// Acquire an image from the swap chain
	// Record a command buffer which draws the scene onto that image
	// Submit the recorded command buffer
	// Present the swap chain image
	void GraphicsEngine::DrawFrame()
	{
		vkWaitForFences(vulkan_device_.GetLogicalDevice(), 1, &in_flight_fence_[current_frame_], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(vulkan_device_.GetLogicalDevice(), swap_chain_.GetSwapChain(), UINT64_MAX, image_available_semaphore_[current_frame_], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || framebuffer_resized_) {
			framebuffer_resized_ = false;
			swap_chain_.RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		// Only reset the fence if we are submitting work
		vkResetFences(vulkan_device_.GetLogicalDevice(), 1, &in_flight_fence_[current_frame_]);

		vkResetCommandBuffer(command_buffer_.GetCommandBuffer(current_frame_), 0);

		uniform_buffer_.UpdateUniformBuffer(swap_chain_.GetExtent(), current_frame_);

		command_buffer_.RecordCommandBuffer(command_buffer_.GetCommandBuffer(current_frame_), imageIndex, vertex_buffer_, &uniform_buffer_.GetDescriptorSet(current_frame_));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { image_available_semaphore_[current_frame_] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_buffer_.GetCommandBuffer(current_frame_);


		VkSemaphore signalSemaphores[] = { render_finished_semaphore_[current_frame_] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(vulkan_device_.GetGraphicsQueue(), 1, &submitInfo, in_flight_fence_[current_frame_]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swap_chain_.GetSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(vulkan_device_.GetPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_) {
			framebuffer_resized_ = false;
			swap_chain_.RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		current_frame_ = (current_frame_ + 1) % kMaxFramesnFlight;
	}
#pragma endregion Frame
#pragma endregion Functions
}