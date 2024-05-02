#include "VulkanDevice.h"

VulkanDeviceHandle::VulkanDeviceHandle()
	:vk_GraphicsCard{VK_NULL_HANDLE}, m_GPUQueueFamilyIndices(),
	m_GPUSwapchainSupportDetails(), vk_device(), vk_graphicsQueue(),
	vk_presentQueue()
{

}

/****************************************************************************************************************
* Function Argument 1: The Vulkan SDK's instance object is needed for the creation of the logical device        *
* Function Argument 2: The Vulkan SDK's surface object is needed to find the device's swapchain support details *
****************************************************************************************************************/
void VulkanDeviceHandle::CreateVulkanLogicalDevice(const VulkanInstanceHandle & instance, const VkSurfaceKHR& vk_surface)
{
	ChoosePhysicalDevice(instance.GetVulkanSDKInstance(), vk_surface);
	SetupLogicalDevice(instance.GetVulkanSDKInstance());
}

/*********************************************************************************************************
* Function Argument 1: The Vulkan SDK's instance object is needed for the creation of the logical device *
*********************************************************************************************************/
void VulkanDeviceHandle::SetupLogicalDevice(const VkInstance& vk_instance)
{
	/*Initializing an array of create info sturcts for all queue families chosen from the GPU*/
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	//Passing the queue families to a set, as it will save unique ones
	std::set<uint32_t> uniqueQueueFamilies = { m_GPUQueueFamilyIndices.graphics.index, 
		m_GPUQueueFamilyIndices.present.index };
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) 
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	/* Array of create info structs complete */

	/* Initializing device features struct that enables the device features needed for the application */
	VkPhysicalDeviceFeatures deviceFeatures{};
	/* Device Features struct complete */

	/* Initializing create info struct for device */
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	//Passing the previously defined queue create infos to interface with the GPU's queue families
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	//Passing the previously defined device features struct to enable the features defined
	createInfo.pEnabledFeatures = &deviceFeatures;
	//Passing the constant deviceExtensions array to enable the extensions needed for the application
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	/* Create info struct complete */

	//Creating the device and checking if its creation was succesful
	VkResult deviceResult = vkCreateDevice(vk_GraphicsCard, &createInfo, nullptr, &vk_device);
	if (deviceResult != VK_SUCCESS)
		__debugbreak();

	//Saving the graphics queue and the present queue, based on the indices found from the GPU
	vkGetDeviceQueue(vk_device, m_GPUQueueFamilyIndices.graphics.index, 0, &vk_graphicsQueue);
	vkGetDeviceQueue(vk_device, m_GPUQueueFamilyIndices.present.index, 0, &vk_presentQueue);

}

/*****************************************************************************************************************
* Function Argument 1: The Vulkan SDK's instance object is needed to find the available GPUs                     *
* Function Argument 2: The Vulkan SDK's surface object is needed to find the device's swapchain support details, *
*					   and if its queue families have surface support											 *
*****************************************************************************************************************/
void VulkanDeviceHandle::ChoosePhysicalDevice(const VkInstance& vk_instance, const VkSurfaceKHR& vk_surface)
{
	/* Finding all the available graphics cards */
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vk_instance, &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vk_instance, &deviceCount, devices.data());
	/* Available graphics cards found */

	//Checking all of the graphics cards for suitability
	for (const VkPhysicalDevice& device : devices)
	{
		//If a suitable GPU is found, the loop breaks and that device is used
		if (CheckDeviceSuitability(device, vk_surface))
		{
			vk_GraphicsCard = device;
			break;
		}
	}

	//If no suitable graphics card is found, the application cannot continue
	if (vk_GraphicsCard == VK_NULL_HANDLE)
		__debugbreak();
}

/****************************************************************************************************************
* Function Argument 1: The specific graphics card that is being	checked currently								*
* Function Argument 2: The Vulkan SDK's surface object is needed to find the device's swapchain support details *
****************************************************************************************************************/
bool VulkanDeviceHandle::CheckDeviceSuitability(const VkPhysicalDevice& device, const VkSurfaceKHR& vk_surface)
{
	/* Getting the device's properties and features */
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	/* Saved the device's properties and features */

	//If the GPU does not include the properties and features needed for our application, the GPU fails the test
	if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
		!(deviceFeatures.geometryShader))
	{
		return false;
	}

	//Finding the queue family indices of the device and saving them to memory
	FindDeviceQueueFamilyIndices(device, vk_surface);
	//If the required queue families are not found, the GPU fails the test
	if (!(m_GPUQueueFamilyIndices.graphics.indexFound) || !(m_GPUQueueFamilyIndices.present.indexFound))
	{
		return false;
	}

	//The GPU fails if it does not support the extensions in the constant deviceExtensions array
	if (!CheckDeviceExtensionSupport(device))
	{
		return false;
	}

	//Getting the swapchain support details of the device and saving them to memory
	GetDeviceSwapchainSupportDetails(device, vk_surface);
	//If the GPU does not support any surface formats or surface present modes
	if (m_GPUSwapchainSupportDetails.formats.empty() || m_GPUSwapchainSupportDetails.presentModes.empty())
	{
		return false;
	}

	return true;
}

/****************************************************************************************************************
* Function Argument 1: The specific graphics card that is being	checked currently								*
* Function Argument 2: The Vulkan SDK's surface object is needed to see if a queue family has surface support   *
****************************************************************************************************************/
void VulkanDeviceHandle::FindDeviceQueueFamilyIndices(const VkPhysicalDevice& device, const VkSurfaceKHR& vk_surface)
{
	/* Finding the GPU's queue families */
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	/* GPU queue families found */

	//Checking all of the queue families for graphics support
	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		//If a queue family with graphics support is found, its index is saved and the loop breaks
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			m_GPUQueueFamilyIndices.graphics.index = i;
			m_GPUQueueFamilyIndices.graphics.indexFound = true;
			break;
		}
	}

	//Checking all of the queue families for presentation support
	for (uint32_t i = 0; i < queueFamilies.size(); ++i)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface, &presentSupport);
		//If a queue family with presentation support is found, its index is saved and the loop breaks
		if (presentSupport)
		{
			m_GPUQueueFamilyIndices.present.index = i;
			m_GPUQueueFamilyIndices.present.indexFound = true;
			break;
		}
	} 
}

/**********************************************************************************
* Function Argument 1: The specific graphics card that is being	checked currently *
**********************************************************************************/
bool VulkanDeviceHandle::CheckDeviceExtensionSupport(const VkPhysicalDevice& device)
{
	/* Getting all the extensions supported by the GPU */
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	/* GPU extensions saved */

	//Saving the extensions in the device extensions array to a set to make the check easier
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	//Every extension is looked up on the set and erased if found
	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	//If the set is empty, it means that all the extensions were found
	return requiredExtensions.empty();
}

/****************************************************************************************************************
* Function Argument 1: The specific graphics card that is being	checked currently								*
* Function Argument 2: The Vulkan SDK's surface object is needed to find the device's swapchain support details *
****************************************************************************************************************/
void VulkanDeviceHandle::GetDeviceSwapchainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
{
	//The GPU's surface capabilities (min/max number of images in swap chain, min/max width and height of images) are saved
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, 
		&m_GPUSwapchainSupportDetails.surfaceCapabilities);

	/* Looking for all supported surface formats (pixel format, color space) */
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount)
	{
		m_GPUSwapchainSupportDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, 
			m_GPUSwapchainSupportDetails.formats.data());
	}
	/* Saved all supported formats, if any were found */

	/* Looking for all available presentation modes */
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount)
	{
		m_GPUSwapchainSupportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, 
			m_GPUSwapchainSupportDetails.presentModes.data());
	}
	/* Saved all supported presentation modes, if any were found */
}

void VulkanDeviceHandle::Cleanup()
{
	vkDestroyDevice(vk_device, nullptr);
}