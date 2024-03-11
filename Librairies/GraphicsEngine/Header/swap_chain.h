namespace shiny
{
#include <vector>

#include <vulkan/vulkan.h>

class SwapChain
{
public:
    SwapChain() {}
    void Destroy() {}

private:
    VkDevice* logical_device_;
    VkRenderPass* render_pass_;
    std::vector<VkImageView>* swap_chain_images_views_;

    VkExtent2D* extent_;
    VkFormat* format_;

    std::vector<VkFramebuffer> swap_chain_framebuffers_;

    VkSwapchainKHR swap_chain_;
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_;
    std::vector<VkImageView> swap_chain_images_views_;

};
}