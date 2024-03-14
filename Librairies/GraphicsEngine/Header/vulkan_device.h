#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace shiny
{
class VulkanDevice
{
public:
	VulkanDevice(VkInstance* instance, VkSurfaceKHR* surface);
	void Destroy();
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

	VkPhysicalDevice physical_device_;
	VkDevice logical_device_ = nullptr;
	VkQueue graphics_queue_ = nullptr;
	VkQueue present_queue_ = nullptr;
};
}