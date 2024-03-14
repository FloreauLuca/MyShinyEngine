#pragma once
#include <vector>

#include <vulkan/vulkan.h>

#include <validation_layers.h>

namespace shiny
{
class Instance
{
public:
	Instance();
	void Destroy();
private:
	static std::vector<const char*> GetRequiredExtensions();

	VkInstance instance_ = nullptr;
	ValidationLayers validationLayers_ = ValidationLayers();
};
}