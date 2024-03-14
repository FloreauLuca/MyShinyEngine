#pragma once
#include <vector>

#include <vulkan/vulkan.h>

#include <validation_layers.h>

namespace shiny
{
class Instance
{
public:
	void InitInstance();
	void Destroy();

	VkInstance& GetInstance() { return instance_; }
private:
	static std::vector<const char*> GetRequiredExtensions();

	VkInstance instance_ = nullptr;
};
}