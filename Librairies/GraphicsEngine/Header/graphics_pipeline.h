#pragma once
#include <string>
#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

class GraphicsPipeline
{
public:
	void CreateGraphicsPipeline(VkDevice* logical_device, VkExtent2D* extent, VkFormat* format);
	void Cleanup();

    VkRenderPass* GetRenderPass() { return &render_pass_; }
    VkPipeline* GetGraphicsPipeline() { return &graphics_pipeline_; }
private : 

    VkShaderModule CreateShaderModule(const std::vector<char>& code);

#pragma region RenderPass
    void CreateRenderPass();
#pragma endregion RenderPass

    static std::vector<char> ReadFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file : " + filename);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    VkDevice* logical_device_;
    VkExtent2D* extent_;
    VkFormat* format_;

    VkPipelineLayout pipeline_layout_;
    VkShaderModule vert_shader_module_;
    VkShaderModule frag_shader_module_;
    VkRenderPass render_pass_;
    VkPipeline graphics_pipeline_;

};