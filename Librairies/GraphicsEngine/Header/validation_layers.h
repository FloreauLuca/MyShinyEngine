#pragma once
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

namespace shiny
{
class ValidationLayers
{
public:
  static void AddValidationLayer(VkInstanceCreateInfo* createInfo);
  static void AddValidationLayer(VkDeviceCreateInfo* createInfo);
  static const bool CheckValidationLayerSupport();
  static void AddValidationExtensions(std::vector<const char*>* extensions);

  void SetupDebugMessenger(VkInstance* instance);

  void Destroy(VkInstance* instance);
private:
  static VkResult CreateDebugUtilsMessengerEXT(
    VkInstance* instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

  static void DestroyDebugUtilsMessengerEXT(VkInstance* instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

  static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  inline static const std::vector<const char*> kValidationLayers_ = {
    "VK_LAYER_KHRONOS_validation"
  };

#ifdef NDEBUG
  static const bool kEnableValidationLayers_ = false;
#else
  static const bool kEnableValidationLayers_ = true;
#endif

  VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
};
}