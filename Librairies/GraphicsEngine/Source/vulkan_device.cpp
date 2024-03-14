#include <vulkan_device.h>

#include <optional>
#include <stdexcept>
#include <iostream>
#include <set>
#include <validation_layers.h>
#include <swap_chain.h>

namespace shiny
{
#pragma region LogicalDevice
	void VulkanDevice::InitVulkanDevice(VkInstance* instance, VkSurfaceKHR* surface)
	{
		instance_ = instance;
		surface_ = surface;

		PickPhysicalDevice();

		CreateLogicalDevice();

	}

	void VulkanDevice::Destroy()
	{
		vkDestroyDevice(logical_device_, nullptr);
	}

	void VulkanDevice::PickPhysicalDevice() {
		physical_device_ = VK_NULL_HANDLE;
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(*instance_, &deviceCount, nullptr);
		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(*instance_, &deviceCount, devices.data());

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

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device) {

		bool queueFamilyComplete = QueueFamilyIndices::FindQueueFamilies(device, *surface_).IsComplete();

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			swapChainAdequate = SwapChainSupportDetails::QuerySwapChainSupport(device, *surface_).IsComplete();
		}

		return queueFamilyComplete && extensionsSupported && swapChainAdequate;
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(kDeviceExtensions_.begin(), kDeviceExtensions_.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
#pragma endregion PhysicalDevice

#pragma region LogicalDevice
	void VulkanDevice::CreateLogicalDevice() {
		QueueFamilyIndices indices = QueueFamilyIndices::FindQueueFamilies(physical_device_, *surface_);

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

		createInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions_.size());
		createInfo.ppEnabledExtensionNames = kDeviceExtensions_.data();

		ValidationLayers::AddValidationLayer(createInfo);

		if (vkCreateDevice(physical_device_, &createInfo, nullptr, &logical_device_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(logical_device_, indices.graphicsFamily.value(), 0, &graphics_queue_);
		vkGetDeviceQueue(logical_device_, indices.presentFamily.value(), 0, &present_queue_);
	}

#pragma endregion LogicalDevice
}