#pragma once

#include <algorithm>
#include "VulkanDevice.h"

/*************************************************************************
* Holds the vulkan SDK's vulkan swaphcain object,						 *
* used as the owner of the buffers that the application will render to.  *
* It also holds an array of images in the swaphcain,					 *
* and the format and extent of the swaphcain							 *
*************************************************************************/
class VulkanSwapchainHandle
{
public:
	//Class constructor explicitly defined to give initial values to the member variables
	VulkanSwapchainHandle();

	//Used to create the swapchain and retrieve the images in it to save in the array member
	void CreateSwapchain(const VkSurfaceKHR& surface, const VulkanDeviceHandle& device, const GlfwWindowHandle& window);

	//Used to destroy the swaphcain
	void Cleanup(const VkDevice& device);

	/* Member variable getters */
	inline const VkSwapchainKHR& GetVulkanSDKSwapchain() const { return vk_swapchain; }

	inline const std::vector<VkImage>& GetSwapchainImages() const { return vk_images; }

	inline const VkExtent2D& GetSwapchainExtent() const { return vk_extent; }

	inline const VkFormat& GetSwapchainImageFormat() const { return vk_imageFormat; }
	/* Member variable getters end */

private:
	//Called by CreateSwapchain, to choose the optimal surface format(color depth) for the swapchain
	VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	//Called by CreateSwapchain, to choose the optimal presentation mode
	// (conditions for "swapping" images to the screen) for the swapchain
	VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	//Called by CreateSwapchain, to choose the optimal extent (resolution of images in swap chain) for the swapchain
	VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, 
		const GlfwWindowHandle& window);

private:
	//Holds the vulkan SDK's swaphcain object
	VkSwapchainKHR vk_swapchain;

	//An array that includes all the images found in the swapchain
	std::vector<VkImage> vk_images;

	//Holds the chosen swapchain extent (resolution of images in swapchain)
	VkExtent2D vk_extent;

	//Holds the chosen image format 
	VkFormat vk_imageFormat;
};