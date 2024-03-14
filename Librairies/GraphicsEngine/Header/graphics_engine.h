#pragma once
#include <graphics_pipeline.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <window.h>
#include <instance.h>
#include <validation_layers.h>
#include <surface.h>
#include <vulkan_device.h>
#include <swap_chain.h>
#include <command_buffer.h>

namespace shiny
{
	class GraphicsEngine
	{
	public:
		void Run();
	private:
#pragma region Functions
#pragma region Main_loop
		void InitVulkan();

		void MainLoop();

		void Cleanup();

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
			auto app = reinterpret_cast<GraphicsEngine*>(glfwGetWindowUserPointer(window));
			app->framebuffer_resized_ = true;
		}
#pragma endregion Main_loop

#pragma region SyncObjects
		void CreateSyncObjects();
#pragma endregion SyncObjects

#pragma region Frame
		void DrawFrame();
#pragma region Frame
#pragma endregion Functions

#pragma region Members
		const uint32_t kWidth = 800;
		const uint32_t kHeight = 600;
		const int kMaxFramesnFlight = 2;


		std::vector<VkSemaphore> image_available_semaphore_;
		std::vector<VkSemaphore> render_finished_semaphore_;
		std::vector<VkFence> in_flight_fence_;

		bool framebuffer_resized_ = false;

		uint32_t current_frame_ = 0;


		const std::vector<const char*> kDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

#pragma endregion Members

		Window window_ = Window();
		Instance instance_ = Instance();
		ValidationLayers validation_layers_ = ValidationLayers();
		Surface surface_ = Surface();
		VulkanDevice vulkan_device_ = VulkanDevice();
		SwapChain swap_chain_ = SwapChain();
		GraphicsPipeline graphics_pipeline_;
		CommandBuffer command_buffer_ = CommandBuffer();
	};
}