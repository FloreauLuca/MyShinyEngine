#include <graphics_pipeline.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class GraphicsEngine
{
public:
	void Run();
private:
#pragma region Functions
#pragma region Main_loop
	void InitWindow();

	void InitVulkan();

	void MainLoop();

	void Cleanup();
#pragma endregion Main_loop

#pragma region Instance
	void CreateInstance();
#pragma endregion Instance

#pragma region Validation_layers
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

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void SetupDebugMessenger();

	const bool CheckValidationLayerSupport() const;

	std::vector<const char*> GetRequiredExtensions();

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
#pragma endregion Validation_layers

#pragma region Surface
	void CreateSurface();
#pragma endregion Surface

#pragma region PhysicalDevice
	void PickPhysicalDevice();

	bool IsDeviceSuitable(VkPhysicalDevice device);

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool IsComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
#pragma endregion PhysicalDevice

#pragma region LogicalDevice
	void CreateLogicalDevice();

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
#pragma endregion LogicalDevice

#pragma region SwapChain
	void CreateSwapChain();

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
#pragma endregion SwapChain

#pragma region ImageViews
	void CreateImageViews();
#pragma endregion ImageViews
#pragma endregion Functions

#pragma region Members
	const uint32_t kWidth = 800;
	const uint32_t kHeight = 600;

	GLFWwindow* window_;
	VkInstance instance_;
	VkDebugUtilsMessengerEXT debug_messenger_;
	VkPhysicalDevice physical_device_;
	VkDevice logical_device_;
	VkQueue graphics_queue_;
	VkSurfaceKHR surface_;
	VkQueue present_queue_;
	VkSwapchainKHR swap_chain_;
	std::vector<VkImage> swap_chain_images_;
	VkFormat swap_chain_image_format_;
	VkExtent2D swap_chain_extent_;
	std::vector<VkImageView> swap_chain_images_views_;

	GraphicsPipeline graphics_pipeline_;

	const std::vector<const char*> kValidationLayers_ = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> kDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool kEnableValidationLayers_ = false;
#else
	const bool kEnableValidationLayers_ = true;
#endif
#pragma endregion Members
};