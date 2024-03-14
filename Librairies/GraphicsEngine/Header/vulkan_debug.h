#pragma once
#include <vulkan/vulkan.h>

namespace shiny
{
class VulkanDebug
{
public:
	VulkanDebug() {}
	void Destroy() {}

private:
	VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
};
}