#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

namespace shiny
{
static class ValidationLayers
{
public:
  static void AddValidationLayer(VkInstanceCreateInfo* createInfo);
  static const bool CheckValidationLayerSupport();
  void Destroy(VkInstance* instance);
private:
  static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
      func(instance, debugMessenger, pAllocator);
    }
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      // Message is important enough to show
      std::cerr << "validation layer: " << messageSeverity << messageType << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
  }

  void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void SetupDebugMessenger();
  std::vector<const char*> GetRequiredExtensions();

  const std::vector<const char*> kValidationLayers_ = {
    "VK_LAYER_KHRONOS_validation"
  };

#ifdef NDEBUG
  const bool kEnableValidationLayers_ = false;
#else
  const bool kEnableValidationLayers_ = true;
#endif

  VkDebugUtilsMessengerEXT debug_messenger_;
};
}