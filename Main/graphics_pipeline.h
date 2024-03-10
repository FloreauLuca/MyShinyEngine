#include <string>
#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

class GraphicsPipeline
{
public:
	void CreateGraphicsPipeline(VkDevice* logical_device);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
private : 

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
};