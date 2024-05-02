#include "VulkanSwapchain.h"

VulkanSwapchainHandle::VulkanSwapchainHandle()
	:vk_swapchain(), vk_images(), vk_extent(), vk_imageFormat()
{

}

/**********************************************************************************************
* Function Argument 1: The vulkan SDK's surface object has to be passed in the create info    *
*                      struct of the swaphcain before the swapchain is created                *
* Function Argument 2: The vulkan device handle is needed to access the Vulkan SDK's device   *
*					   for the creation of the swapchain and to access the graphics card's    *
*					   swapchain support details, to choose the ideal settings                *
* Function Argument 3: The window is used to access its width and height to set the swaphcain *
*                      extent correctly
**********************************************************************************************/
void VulkanSwapchainHandle::CreateSwapchain(const VkSurfaceKHR& surface,
	const VulkanDeviceHandle& device, const GlfwWindowHandle& window)
{
	//Retrieving the GPU's swaphcain support details
	SwapchainSupportDetails swapchainSupport = device.GetSwapchainSupportDetails();

	/* Choosing the best settings for the swapchain */
	VkSurfaceFormatKHR format = ChooseSwapchainSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR present = ChooseSwapchainPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = ChooseSwapchainExtent(swapchainSupport.surfaceCapabilities, window);
	//Setting the amount of images to have in the swapchain
	uint32_t imageCount = swapchainSupport.surfaceCapabilities.minImageCount + 1;
	//Checking if there is a maximum amount of images and if there is, making sure it is not exceeded
	if (swapchainSupport.surfaceCapabilities.maxImageCount > 0 &&
		swapchainSupport.surfaceCapabilities.maxImageCount < imageCount)
	{
		imageCount = swapchainSupport.surfaceCapabilities.maxImageCount;
	}
	//Saving the image format and the swapchain extent
	vk_imageFormat = format.format;
	vk_extent = extent;
	/* Swapchain settings complete */

	/* Initializing create info struct for the swaphcain */
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.imageExtent = extent;
	createInfo.presentMode = present;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = vk_imageFormat;
	createInfo.imageColorSpace = format.colorSpace;
	//This should be 1, unless it's a stereoscopic 3D application
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//Different settings should be chosen depending on if 
	//all operations are excecuted on the same queue family or not
	uint32_t graphicsQueueIndex = device.GetQueueFamilyGraphicsIndex();
	uint32_t presentQueueIndex = device.GetQueueFamilyPresentIndex();
	if (graphicsQueueIndex != presentQueueIndex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[] = { graphicsQueueIndex, presentQueueIndex };
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; 
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapchainSupport.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	//Clipping increases performance, as the colors of pixels that are obscured are ignored
	createInfo.clipped = VK_TRUE;
	//This is used when the swapcahain is unoptimized and a new one needs to be created
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	/* Swapchain create info struct complete */

	//Creating the swaphcain and checking it its creation was succesful
	VkResult swapchainResult = vkCreateSwapchainKHR(device.GetVulkanSDKLogicalDevice(), &createInfo,
		nullptr, &vk_swapchain);
	if (swapchainResult != VK_SUCCESS)
	{
		__debugbreak();
	}

	//Retrieving the images from the swapchain
	vkGetSwapchainImagesKHR(device.GetVulkanSDKLogicalDevice(), vk_swapchain, &imageCount, nullptr);
	vk_images.resize(imageCount);
	vkGetSwapchainImagesKHR(device.GetVulkanSDKLogicalDevice(), vk_swapchain, &imageCount, vk_images.data());
}

/********************************************************************************
* Function Argument 1: An array of the available formats retrieved from the GPU *
********************************************************************************/
VkSurfaceFormatKHR VulkanSwapchainHandle::ChooseSwapchainSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//Checking all the available formats
	for (const VkSurfaceFormatKHR& format : availableFormats)
	{
		//If the GPU supports a format with the desired settings, it returns that format
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
			format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	//If the desired settings are not found, the first member of the array is returned
	return availableFormats[0];
}

/**************************************************************************************
* Function Argument 1: An array of the available present modes retrieved from the GPU *
***************************************************************************************/
VkPresentModeKHR VulkanSwapchainHandle::ChooseSwapchainPresentMode(
	const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//Checking all available present modes for the desired setting
	for (const VkPresentModeKHR& presentMode : availablePresentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

/*********************************************************************************************
* Function Argument 1: The surface capabilities of the GPU, needed to check			         *
*					   the surface extent that is supported									 *
* Function Argument 2: The window handle, used to access the width and height of the window  *
*********************************************************************************************/
VkExtent2D VulkanSwapchainHandle::ChooseSwapchainExtent(
	const VkSurfaceCapabilitiesKHR& surfaceCapabilities, const GlfwWindowHandle& window)
{
	//If the surafce extent width is set to the max of unit32_t, it returns the current extent
	if (surfaceCapabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) 
	{
		return surfaceCapabilities.currentExtent;
	}
	else 
	{
		uint32_t width = window.GetWindowWidth();
		uint32_t height = window.GetWindowHeight();

		VkExtent2D actualExtent = {(width),(height)};

		actualExtent.width = std::clamp(actualExtent.width, 
			surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, 
			surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VulkanSwapchainHandle::Cleanup(const VkDevice& device)
{
	vkDestroySwapchainKHR(device, vk_swapchain, nullptr);
}