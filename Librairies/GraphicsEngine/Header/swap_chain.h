#pragma once
#include <vector>

#include <vulkan/vulkan.h>
#include <optional>
#include <GLFW/glfw3.h>
namespace shiny
{

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    bool IsComplete() {
      return !formats.empty() && !presentModes.empty();
    }
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice* device, VkSurfaceKHR* surface);
  };


  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool IsComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice* device, VkSurfaceKHR* surface);
  };


  class SwapChain
  {
  public:
    SwapChain(
      GLFWwindow* window,
      VkInstance* instance,
      VkSurfaceKHR* surface,
      VkDevice* logical_device,
      VkPhysicalDevice* physical_device,
      VkRenderPass* render_pass) :
      window_(window),
      instance_(instance),
      surface_(surface),
      logical_device_(logical_device),
      physical_device_(physical_device),
      render_pass_(render_pass) {}

    void Destroy();

    std::vector<VkFramebuffer>* GetSwapChainFrameBuffers() { return &swap_chain_framebuffers_; }
    VkExtent2D* GetExtent() { return &swap_chain_extent_; }

    void CreateSwapChain();

    void CreateFrameBuffers();

    void CreateImageViews();
  private:
#pragma region SwapChain
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CleanupSwapChain();

    void RecreateSwapChain();
#pragma endregion SwapChain

    GLFWwindow* window_ = nullptr;
    VkInstance* instance_ = nullptr;
    VkSurfaceKHR* surface_ = nullptr;
    VkDevice* logical_device_ = nullptr;
    VkPhysicalDevice* physical_device_ = nullptr;
    VkRenderPass* render_pass_ = nullptr;

    std::vector<VkFramebuffer> swap_chain_framebuffers_;

    VkSwapchainKHR swap_chain_;
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_;
    std::vector<VkImageView> swap_chain_images_views_;

  };
}