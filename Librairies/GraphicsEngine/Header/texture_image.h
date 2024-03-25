#pragma once
#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace shiny
{
	class TextureImage
	{
	public:
		TextureImage() {}
		void InitTextureImage(VkDevice& device, VkPhysicalDevice& physical_device, VkCommandPool& command_pool, VkQueue& graphics_queue);
		void Destroy();

		static void CreateImage(VkDevice& device, VkPhysicalDevice& physical_device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	private:

		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		std::string path_ = "./Data/piplup.jpg";

		VkDevice* device_ = nullptr;
		VkPhysicalDevice* physical_device_ = nullptr;
		VkCommandPool* command_pool_ = nullptr;
		VkQueue* graphics_queue_ = nullptr;

		VkImage texture_image_ = nullptr;
		VkDeviceMemory texture_image_memory_ = nullptr;
	};
}