#include <uniform_buffer.h>

#include <array>
#include <stdexcept>

#include <buffer.h>
#include <command_buffer.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace shiny
{
	void UniformBuffer::InitUniformBuffer(VkDevice& logical_device, VkPhysicalDevice& physical_device)
	{
		logical_device_ = &logical_device;
		physical_device_ = &physical_device;

		CreateDescriptorSetLayout();
		CreateUniformBuffer();
		CreateDescriptorPool();
		CreateDescriptorSets();

		ConfigureDescriptor();
	}

	void UniformBuffer::Destroy()
	{
		for (size_t i = 0; i < CommandBuffer::kMaxFramesInFlight; i++) {
			vkDestroyBuffer(*logical_device_, uniform_buffers_[i], nullptr);
			vkFreeMemory(*logical_device_, uniform_buffers_memory_[i], nullptr);
		}

		vkDestroyDescriptorSetLayout(*logical_device_, descriptor_set_layout_, nullptr);

		vkDestroyDescriptorPool(*logical_device_, descriptor_pool_, nullptr);
	}

	void UniformBuffer::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(*logical_device_, &layoutInfo, nullptr, &descriptor_set_layout_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void UniformBuffer::UpdateUniformBuffer(VkExtent2D& extent, uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniform_buffers_mapped_[currentImage], &ubo, sizeof(ubo));
	}

	void UniformBuffer::CreateUniformBuffer()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniform_buffers_.resize(CommandBuffer::kMaxFramesInFlight);
		uniform_buffers_memory_.resize(CommandBuffer::kMaxFramesInFlight);
		uniform_buffers_mapped_.resize(CommandBuffer::kMaxFramesInFlight);

		for (size_t i = 0; i < CommandBuffer::kMaxFramesInFlight; i++) {
			Buffer::CreateBuffer(*logical_device_, *physical_device_, bufferSize, 
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				uniform_buffers_[i], uniform_buffers_memory_[i]);

			vkMapMemory(*logical_device_, uniform_buffers_memory_[i], 0, bufferSize, 0, &uniform_buffers_mapped_[i]);
		}
	}
	void UniformBuffer::CreateDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(CommandBuffer::kMaxFramesInFlight);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(CommandBuffer::kMaxFramesInFlight);


		if (vkCreateDescriptorPool(*logical_device_, &poolInfo, nullptr, &descriptor_pool_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void UniformBuffer::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(CommandBuffer::kMaxFramesInFlight, descriptor_set_layout_);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptor_pool_;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(CommandBuffer::kMaxFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		descriptor_sets_.resize(CommandBuffer::kMaxFramesInFlight);
		if (vkAllocateDescriptorSets(*logical_device_, &allocInfo, descriptor_sets_.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	void UniformBuffer::ConfigureDescriptor()
	{
		for (size_t i = 0; i < CommandBuffer::kMaxFramesInFlight; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniform_buffers_[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptor_sets_[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(*logical_device_, 1, &descriptorWrite, 0, nullptr);
		}
	}
}