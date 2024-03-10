#include <frame_buffers.h>

#include <iostream>

void FrameBuffers::CreateFrameBuffers(VkDevice* logical_device, VkRenderPass* render_pass, std::vector<VkImageView>* swap_chain_images_views, VkExtent2D* extent, VkFormat* format)
{
	logical_device_ = logical_device;
	render_pass_ = render_pass;
	swap_chain_images_views_ = swap_chain_images_views;
	extent_ = extent;
	format_ = format;

	swap_chain_framebuffers_.resize(swap_chain_images_views_->size());

    for (size_t i = 0; i < swap_chain_images_views_->size(); i++) {
        VkImageView attachments[] = {
            swap_chain_images_views_->at(i)
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = *render_pass_;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent_->width;
        framebufferInfo.height = extent_->height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(*logical_device_, &framebufferInfo, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void FrameBuffers::CleanUp()
{
    for (auto framebuffer : swap_chain_framebuffers_) {
        vkDestroyFramebuffer(*logical_device_, framebuffer, nullptr);
    }
}