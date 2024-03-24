#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace shiny
{
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer() {}
		void InitUniformBuffer(VkDevice& logical_device, VkPhysicalDevice& physical_device);
		void Destroy();

		void UpdateUniformBuffer(VkExtent2D& extent, uint32_t currentImage);

		VkDescriptorSetLayout& GetDescriptorSetLayout() { return descriptor_set_layout_; }
		VkDescriptorSet& GetDescriptorSet(uint32_t imageIndex) { return descriptor_sets_[imageIndex]; }
		
	private:
		void CreateDescriptorSetLayout();
		void CreateUniformBuffer();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		void ConfigureDescriptor();

		VkDevice* logical_device_ = nullptr;
		VkPhysicalDevice* physical_device_ = nullptr;

		std::vector<VkBuffer> uniform_buffers_ = std::vector<VkBuffer>();
		std::vector<VkDeviceMemory> uniform_buffers_memory_;
		std::vector<void*> uniform_buffers_mapped_ = std::vector<void*>();

		VkDescriptorSetLayout descriptor_set_layout_ = nullptr;

		VkDescriptorPool descriptor_pool_ = nullptr;
		std::vector<VkDescriptorSet> descriptor_sets_ = std::vector<VkDescriptorSet>();
	};
}