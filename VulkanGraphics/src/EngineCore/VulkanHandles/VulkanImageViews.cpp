#include "VulkanImageViews.h"

/********************************************************************************************
* Function argument 1: The vulkan swapchain handle is needed to access the swapchain images *
*					   and the swapchain image format										*
* Function argument 2: The Vulkan SDK's device object is needed for image view creation     *
********************************************************************************************/
void VulkanImageViewsHandle::CreateImageViews(const VulkanSwapchainHandle& swapchain, 
	const VkDevice& device)
{
	//Resizing the image view array according to the swaphcain images
	vk_imageViews.resize(swapchain.GetSwapchainImages().size());
	for (uint32_t i = 0; i < vk_imageViews.size(); ++i)
	{
		/* Initializing create info struct for image view */
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = (swapchain.GetSwapchainImages())[i];
		//Specifying how the images the application renders to should be treated (2D, 3D etc)
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
		createInfo.format = swapchain.GetSwapchainImageFormat();
		//Default color mapping
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		//Specifies the image view's purpose
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		/* Create info struct complete */

		//Creating the image view and checking if it was succesful
		VkResult imageViewResults = vkCreateImageView(device, &createInfo, nullptr,
			&vk_imageViews[i]);
		if (imageViewResults != VK_SUCCESS)
		{
			__debugbreak();
		}
	}
}

void VulkanImageViewsHandle::Cleanup(const VkDevice& device)
{
	for (const VkImageView& imageView : vk_imageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}
}