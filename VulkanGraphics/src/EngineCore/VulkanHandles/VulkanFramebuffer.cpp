#include "EngineCore/VulkanCore.h"


void VulkanFramebufferHandle::CreateFramebuffers(const std::vector<VkImageView>& imageViews, 
    const VkRenderPass& renderPass, const VkExtent2D& swapchainExtent, const VkDevice& device)
{
    //Iterating through all image view to create a framebuffer for each one
	vk_framebuffers.resize(imageViews.size());
	for (size_t i = 0; i < imageViews.size(); ++i)
	{
		VkImageView attachments[] = { imageViews[i] };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        //The render pass needs to be compatible with the framebuffer,
        //meaning they need to have the same number and type of attachments
        framebufferInfo.renderPass = renderPass;
        //Passing the image views
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchainExtent.width;
        framebufferInfo.height = swapchainExtent.height;
        //Our swapchain images are single images, so layers should be 1
        framebufferInfo.layers = 1;

        //Creating a framebuffer and adding it to the array
        VkResult framebufferResult = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vk_framebuffers[i]);
        if (framebufferResult != VK_SUCCESS)
        {
            __debugbreak();
        }
	}
}

void VulkanFramebufferHandle::Cleanup(const VkDevice& device)
{
    for (const VkFramebuffer& framebuffer : vk_framebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}