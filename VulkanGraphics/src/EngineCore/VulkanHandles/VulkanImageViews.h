#pragma once

#include "VulkanSwapchain.h"

/***************************************
* Holds the vulkan SDK's image views,  * 
* used to access the swapchain images, *
* and explain how to treat them		   *
***************************************/
class VulkanImageViewsHandle
{
public:
	//Creates all the image views based on the swaphcain images and saves them to an array
	void CreateImageViews(const VulkanSwapchainHandle& swapchain, const VkDevice& device);

	//Destroys all the image views created
	void Cleanup(const VkDevice& device);

	/* Member variable getters */
	inline const std::vector<VkImageView>& GetVulkanSDKImageViews() const { return vk_imageViews; }
	/* Member variable getters end */
private:

private:
	//Holds all the vulkan SDK image view that correspond to each swapchain image
	std::vector<VkImageView> vk_imageViews;
};