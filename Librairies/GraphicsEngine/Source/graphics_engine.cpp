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

void GraphicsEngine::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}

#pragma region Functions
#pragma region Main_loop
void GraphicsEngine::InitWindow()
{
	glfwInit();

	// Disable opengl client
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window_ = glfwCreateWindow(kWidth, kHeight, "MyShinyEngine", nullptr, nullptr);
	glfwSetWindowUserPointer(window_, this);
	glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
}

void GraphicsEngine::InitVulkan()
{
	CreateInstance();

	SetupDebugMessenger();
	
	CreateSurface();
	
	PickPhysicalDevice();
	
	CreateLogicalDevice();
	
	CreateSwapChain();
	
	CreateImageViews();

	graphics_pipeline_ = GraphicsPipeline();
	graphics_pipeline_.CreateGraphicsPipeline(&logical_device_, &swap_chain_extent_, &swap_chain_image_format_);

	frame_buffers_ = FrameBuffers();
	frame_buffers_.CreateFrameBuffers(&logical_device_, graphics_pipeline_.GetRenderPass(), &swap_chain_images_views_, &swap_chain_extent_, &swap_chain_image_format_);

	CreateCommandPool();
	CreateCommandBuffer();

	CreateSyncObjects();
}

void GraphicsEngine::MainLoop()
{
	while (!glfwWindowShouldClose(window_)) {
		glfwPollEvents();
		DrawFrame();
	}

	vkDeviceWaitIdle(logical_device_);
}

void GraphicsEngine::Cleanup()
{
	CleanupSwapChain();

	for (size_t i = 0; i < kMaxFramesnFlight; i++) {
		vkDestroySemaphore(logical_device_, render_finished_semaphore_[i], nullptr);
		vkDestroySemaphore(logical_device_, image_available_semaphore_[i], nullptr);
		vkDestroyFence(logical_device_, in_flight_fence_[i], nullptr);
	}

	vkDestroyCommandPool(logical_device_, command_pool_, nullptr);

	graphics_pipeline_.Cleanup();

	vkDestroyDevice(logical_device_, nullptr);

	if (kEnableValidationLayers_) {
		DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
	}

	vkDestroySurfaceKHR(instance_, surface_, nullptr);

	vkDestroyInstance(instance_, nullptr);

	glfwDestroyWindow(window_);

	glfwTerminate();
}
#pragma endregion Main_loop

#pragma region Instance
void GraphicsEngine::CreateInstance()
{
	if (kEnableValidationLayers_ && !CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// Application infos
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "MyShinyEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "MyShinyEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	// Instance Create Infos
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Extensions
	std::vector<const char*> extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	// Validation Layers
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (kEnableValidationLayers_) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers_.size());
		createInfo.ppEnabledLayerNames = kValidationLayers_.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> vkEnxtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkEnxtensions.data());
	std::cout << "Available extensions:\n";

	for (const auto& extension : vkEnxtensions)
	{
		std::cout << '\t' << extension.extensionName << '\n';
	}
}
#pragma endregion Instance

#pragma region Validation_layers
void GraphicsEngine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

void GraphicsEngine::SetupDebugMessenger() {
	if (!kEnableValidationLayers_) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debug_messenger_) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

const bool GraphicsEngine::CheckValidationLayerSupport() const {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : kValidationLayers_) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> GraphicsEngine::GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (kEnableValidationLayers_) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
#pragma endregion Validation_layers

#pragma region Surface
void GraphicsEngine::CreateSurface() {
	if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}
#pragma endregion Surface

#pragma region PhysicalDevice
void GraphicsEngine::PickPhysicalDevice() {
	physical_device_ = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

	std::cout << "Available devices :\n";

	for (const VkPhysicalDevice& device : devices)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		std::cout << '\t' << deviceProperties.deviceName << '\n';
	}


	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			physical_device_ = device;
			break;
		}
	}

	if (physical_device_ == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool GraphicsEngine::IsDeviceSuitable(VkPhysicalDevice device) {

	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

GraphicsEngine::QueueFamilyIndices GraphicsEngine::FindQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.IsComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool GraphicsEngine::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(kDeviceExtensions.begin(), kDeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}
#pragma endregion PhysicalDevice

#pragma region LogicalDevice
void GraphicsEngine::CreateLogicalDevice() {
	QueueFamilyIndices indices = FindQueueFamilies(physical_device_);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = kDeviceExtensions.data();

	if (kEnableValidationLayers_) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers_.size());
		createInfo.ppEnabledLayerNames = kValidationLayers_.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physical_device_, &createInfo, nullptr, &logical_device_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(logical_device_, indices.graphicsFamily.value(), 0, &graphics_queue_);
	vkGetDeviceQueue(logical_device_, indices.presentFamily.value(), 0, &present_queue_);
}

GraphicsEngine::SwapChainSupportDetails GraphicsEngine::QuerySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
	}

	return details;
}
#pragma endregion LogicalDevice

#pragma region SwapChain
void GraphicsEngine::CreateSwapChain() {
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physical_device_);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface_;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT 

	QueueFamilyIndices indices = FindQueueFamilies(physical_device_);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logical_device_, &createInfo, nullptr, &swap_chain_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &imageCount, nullptr);
	swap_chain_images_.resize(imageCount);
	vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &imageCount, swap_chain_images_.data());

	swap_chain_image_format_ = surfaceFormat.format;
	swap_chain_extent_ = extent;
}

VkSurfaceFormatKHR GraphicsEngine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR GraphicsEngine::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GraphicsEngine::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		// Use if screen is to wide
		int width, height;
		glfwGetFramebufferSize(window_, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void GraphicsEngine::CleanupSwapChain() {
	frame_buffers_.CleanUp();

	for (size_t i = 0; i < swap_chain_images_views_.size(); i++) {
		vkDestroyImageView(logical_device_, swap_chain_images_views_[i], nullptr);
	}

	vkDestroySwapchainKHR(logical_device_, swap_chain_, nullptr);
}

void GraphicsEngine::RecreateSwapChain() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(window_, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window_, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logical_device_);

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	frame_buffers_.CreateFrameBuffers(&logical_device_, graphics_pipeline_.GetRenderPass(), &swap_chain_images_views_, &swap_chain_extent_, &swap_chain_image_format_);
}
#pragma endregion SwapChain

#pragma region ImageViews
void GraphicsEngine::CreateImageViews() {
	swap_chain_images_views_.resize(swap_chain_images_.size());

	for (size_t i = 0; i < swap_chain_images_.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swap_chain_images_[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swap_chain_image_format_;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logical_device_, &createInfo, nullptr, &swap_chain_images_views_[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void GraphicsEngine::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physical_device_);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(logical_device_, &poolInfo, nullptr, &command_pool_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void GraphicsEngine::CreateCommandBuffer()
{
	command_buffers_.resize(kMaxFramesnFlight);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = command_pool_;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)command_buffers_.size();

	if (vkAllocateCommandBuffers(logical_device_, &allocInfo, command_buffers_.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void GraphicsEngine::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
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
	renderPassInfo.renderPass = *graphics_pipeline_.GetRenderPass();
	renderPassInfo.framebuffer = frame_buffers_.GetSwapChainFrameBuffers()->at(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swap_chain_extent_;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *graphics_pipeline_.GetGraphicsPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swap_chain_extent_.width);
	viewport.height = static_cast<float>(swap_chain_extent_.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swap_chain_extent_;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}
#pragma endregion ImageViews

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
		if (vkCreateSemaphore(logical_device_, &semaphoreInfo, nullptr, &image_available_semaphore_[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logical_device_, &semaphoreInfo, nullptr, &render_finished_semaphore_[i]) != VK_SUCCESS ||
			vkCreateFence(logical_device_, &fenceInfo, nullptr, &in_flight_fence_[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}
#pragma endregion SyncObjects

#pragma region Frame
// TODO
// Wait for the previous frame to finish
// Acquire an image from the swap chain
// Record a command buffer which draws the scene onto that image
// Submit the recorded command buffer
// Present the swap chain image
void GraphicsEngine::DrawFrame()
{
	vkWaitForFences(logical_device_, 1, &in_flight_fence_[current_frame_], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logical_device_, swap_chain_, UINT64_MAX, image_available_semaphore_[current_frame_], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || framebuffer_resized_) {
		framebuffer_resized_ = false;
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Only reset the fence if we are submitting work
	vkResetFences(logical_device_, 1, &in_flight_fence_[current_frame_]);

	vkResetCommandBuffer(command_buffers_[current_frame_], 0);

	RecordCommandBuffer(command_buffers_[current_frame_], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { image_available_semaphore_[current_frame_] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_buffers_[current_frame_];

	
	VkSemaphore signalSemaphores[] = { render_finished_semaphore_[current_frame_] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphics_queue_, 1, &submitInfo, in_flight_fence_[current_frame_]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swap_chain_ };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(present_queue_, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_) {
		framebuffer_resized_ = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	current_frame_ = (current_frame_ + 1) % kMaxFramesnFlight;
}
#pragma endregion Frame
#pragma endregion Functions
