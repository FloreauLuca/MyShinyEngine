#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace shiny
{
class VulkanDevice
{
public:
	VulkanDevice() {}
	void InitVulkanDevice(VkInstance* instance, VkSurfaceKHR* surface);
	void Destroy();

	VkDevice& GetLogicalDevice() { return logical_device_; }
	VkPhysicalDevice& GetPhysicalDevice() { return physical_device_; }

	VkQueue& GetGraphicsQueue() { return graphics_queue_; }
	VkQueue& GetPresentQueue() { return present_queue_; }
private:
	void PickPhysicalDevice();

	bool IsDeviceSuitable(VkPhysicalDevice device);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	void CreateLogicalDevice();

	const std::vector<const char*> kDeviceExtensions_ = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkInstance* instance_ = nullptr;
	VkSurfaceKHR* surface_ = nullptr;

	VkPhysicalDevice physical_device_ = nullptr;
	VkDevice logical_device_ = nullptr;
	VkQueue graphics_queue_ = nullptr;
	VkQueue present_queue_ = nullptr;
};
}