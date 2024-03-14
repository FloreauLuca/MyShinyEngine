#pragma once
#include <vector>

#include <vulkan/vulkan.h>
#include <optional>
#include <GLFW/glfw3.h>

#include <queue_family_indices.h>
#include <swap_chain_support_details.h>

namespace shiny
{
  class SwapChain
  {
  public:
    SwapChain() {}
    void InitSwapChain(
      GLFWwindow* window,
      VkInstance* instance,
      VkSurfaceKHR* surface,
      VkDevice* logical_device,
      VkPhysicalDevice* physical_device) {
       window_ = window;
        instance_ = instance;
        surface_ = surface;
        logical_device_ = logical_device;
        physical_device_ = physical_device;
        CreateSwapChain();
        CreateImageViews();
    }

    void Destroy();

    std::vector<VkFramebuffer>* GetSwapChainFrameBuffers() { return &swap_chain_framebuffers_; }
    VkExtent2D& GetExtent() { return swap_chain_extent_; }
    VkFormat& GetImageFormat() { return swap_chain_image_format_; }
    VkSwapchainKHR& GetSwapChain() { return swap_chain_; }

    void CreateSwapChain();

    void CreateFrameBuffers(VkRenderPass& render_pass);

    void CreateImageViews();
    void RecreateSwapChain();
  private:
#pragma region SwapChain
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CleanupSwapChain();
#pragma endregion SwapChain

    GLFWwindow* window_ = nullptr;
    VkInstance* instance_ = nullptr;
    VkSurfaceKHR* surface_ = nullptr;
    VkDevice* logical_device_ = nullptr;
    VkPhysicalDevice* physical_device_ = nullptr;
    VkRenderPass* render_pass_ = nullptr;

    std::vector<VkFramebuffer> swap_chain_framebuffers_;

    VkSwapchainKHR swap_chain_ = nullptr;
    std::vector<VkImage> swap_chain_images_ = std::vector<VkImage>();
    VkFormat swap_chain_image_format_ = VK_FORMAT_UNDEFINED;
    VkExtent2D swap_chain_extent_ = VkExtent2D();
    std::vector<VkImageView> swap_chain_images_views_ = std::vector<VkImageView>();

  };
}