namespace shiny
{
#include <vector>

#include <vulkan/vulkan.h>

class ValidationLayers
{
public:
	ValidationLayers() {}
	void Destroy() {}
private:
	const std::vector<const char*> kValidationLayers_ = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool kEnableValidationLayers_ = false;
#else
	const bool kEnableValidationLayers_ = true;
#endif
};
}