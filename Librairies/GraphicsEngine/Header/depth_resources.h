#pragma once
#include <vector>

#include <vulkan/vulkan.h>

namespace shiny
{
	class DepthResources
	{
	public:
		DepthResources() {}
		void InitDepthResources();
		void Destroy();

	private:
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);

		VkPhysicalDevice* physical_device_ = nullptr;

		VkImage depth_image_ = nullptr;
		VkDeviceMemory depth_image_memory_ = nullptr;
		VkImageView depth_image_view_ = nullptr;
	};
}