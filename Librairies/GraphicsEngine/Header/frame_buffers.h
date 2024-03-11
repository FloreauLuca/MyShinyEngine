#include <string>
#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

class FrameBuffers
{
public:
	void CreateFrameBuffers(VkDevice* logical_device, VkRenderPass* render_pass, std::vector<VkImageView>* swap_chain_images_views, VkExtent2D* extent, VkFormat* format);

    std::vector<VkFramebuffer>* GetSwapChainFrameBuffers() { return &swap_chain_framebuffers_; }

    void CleanUp();

private :
    VkDevice* logical_device_;
    VkRenderPass* render_pass_;
    std::vector<VkImageView>* swap_chain_images_views_;

    VkExtent2D* extent_;
    VkFormat* format_;

    std::vector<VkFramebuffer> swap_chain_framebuffers_;
};