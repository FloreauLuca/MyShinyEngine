#include <instance.h>

#include <iostream>

#include <validation_layers.h>
#include <GLFW/glfw3.h>

namespace shiny
{
void Instance::InitInstance()
{
  if (!ValidationLayers::CheckValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  // Application infos
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "MyShinyEngine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "MyShinyEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

  // Instance Create Infos
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Extensions
  std::vector<const char*> extensions = GetRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  ValidationLayers::AddValidationLayer(createInfo, debugCreateInfo);

  if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> vkEnxtensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, vkEnxtensions.data());
  std::cout << "Available extensions:\n";

  for (const auto& extension : vkEnxtensions)
  {
    std::cout << '\t' << extension.extensionName << '\n';
  }
}

std::vector<const char*> Instance::GetRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    ValidationLayers::AddValidationExtensions(extensions);

    return extensions;
}

void Instance::Destroy() {
  vkDestroyInstance(instance_, nullptr);
}
}