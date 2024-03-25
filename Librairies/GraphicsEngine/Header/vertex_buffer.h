#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace shiny
{
struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription();

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

class VertexBuffer
{
public:
	VertexBuffer() {}
	void InitVertexBuffer(VkDevice* logical_device, VkPhysicalDevice* physical_device, VkCommandPool* commandPool, VkQueue* graphicsQueue);
	void Destroy();

	const size_t GetVerticesSize() const { return vertices_.size(); }
	const VkBuffer* GetVertexBuffer() const { return &vertex_buffer_; }

	const size_t GetIndicesSize() const { return indices_.size(); }
	const VkBuffer* GetIndicesBuffer() const { return &index_buffer_; }
private:

	void CreateVertexBuffer();
	void CreateIndexBuffer();

	VkDevice* logical_device_ = nullptr;
	VkPhysicalDevice* physical_device_ = nullptr;
	VkCommandPool* command_pool_ = nullptr;
	VkQueue* graphics_queue_ = nullptr;

	VkBuffer vertex_buffer_ = nullptr;
	VkDeviceMemory vertex_buffer_memory_ = nullptr;

	VkBuffer index_buffer_ = nullptr;
	VkDeviceMemory index_buffer_memory_ = nullptr;

	const std::vector<Vertex> vertices_ = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

		{{ -0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices_ = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};
};
}