#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace shiny
{
struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

class VertexBuffer
{
public:
	VertexBuffer() {}
	void InitVertexBuffer(VkDevice* logical_device, VkPhysicalDevice* physical_device);
	void Destroy();

	const size_t GetVerticesSize() const { return vertices_.size(); }
	const VkBuffer* GetVertexBuffer() const { return &vertex_buffer_; }
private:
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkDevice* logical_device_ = nullptr;
	VkPhysicalDevice* physical_device_ = nullptr;

	VkBuffer vertex_buffer_ = nullptr;
	VkDeviceMemory vertex_buffer_memory_ = nullptr;

	const std::vector<Vertex> vertices_ = {
		{{0.0f, -0.5f}, {0.75f, 0.25f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.5f, 0.65f}},
		{{-0.5f, 0.5f}, {0.3f, 0.0f, 1.0f}}
	};
};
}