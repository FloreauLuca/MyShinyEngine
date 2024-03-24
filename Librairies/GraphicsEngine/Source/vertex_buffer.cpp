#include <vertex_buffer.h>

#include <array>
#include <stdexcept>

#include <buffer.h>

namespace shiny
{
	VkVertexInputBindingDescription Vertex::GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 2>  Vertex::GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	void VertexBuffer::InitVertexBuffer(VkDevice* logical_device, VkPhysicalDevice* physical_device, VkCommandPool* commandPool, VkQueue* graphicsQueue)
	{
		logical_device_ = logical_device;
		physical_device_ = physical_device;

		VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Buffer::CreateBuffer(*logical_device, *physical_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*logical_device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices_.data(), (size_t)bufferSize);
		vkUnmapMemory(*logical_device, stagingBufferMemory);

		Buffer::CreateBuffer(*logical_device, *physical_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

		Buffer::CopyBuffer(*logical_device, *commandPool, *graphicsQueue, stagingBuffer, vertex_buffer_, bufferSize);

		vkDestroyBuffer(*logical_device, stagingBuffer, nullptr);
		vkFreeMemory(*logical_device, stagingBufferMemory, nullptr);
	}

	void VertexBuffer::Destroy()
	{
		vkDestroyBuffer(*logical_device_, vertex_buffer_, nullptr);
		vkFreeMemory(*logical_device_, vertex_buffer_memory_, nullptr);
	}
}