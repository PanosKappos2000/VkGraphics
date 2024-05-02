#pragma once

#include <string>
#include <set>
#include "VulkanInstance.h"

//Holds the extensions that we are going to need for the device to have
const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

//Used to hold the index of a queue family and a boolean that is true if the queue family is found
struct QueueFamilyIndexChecker
{
	uint32_t index;
	bool indexFound = false;
};

//Holds the GPU's queue family indices
struct QueueFamilyIndices
{
	QueueFamilyIndexChecker graphics;
	QueueFamilyIndexChecker present;
};

//Holds the GPU's swapchain support capabilities
struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

/*****************************************************
* Holds the vulkan SDK's device class,               *
* needed to interface with the GPU                   *
* It also holds the queue family indices of the GPU, *
* details about its swapchain support,               *
* and the vulkan SDK's queue objects for the GPU     *
******************************************************/
class VulkanDeviceHandle
{
public:
	//Constructor explicitly defined to give initial values the objects held by the class
	VulkanDeviceHandle();

	//Creates the logical device after finding the physical GPU
	void CreateVulkanLogicalDevice(const VulkanInstanceHandle& instance, const VkSurfaceKHR& vk_surface);

	void Cleanup();

	/* Member variable getters */
	inline const VkDevice& GetVulkanSDKLogicalDevice() const { return vk_device; }

	inline uint32_t GetQueueFamilyGraphicsIndex() const { return m_GPUQueueFamilyIndices.graphics.index; }

	inline uint32_t GetQueueFamilyPresentIndex() const { return m_GPUQueueFamilyIndices.present.index; }

	inline const SwapchainSupportDetails& GetSwapchainSupportDetails() const {
		return m_GPUSwapchainSupportDetails;
	}

	inline const VkQueue& GetVulkanSDKGraphicsQueue() const { return vk_graphicsQueue; }

	inline const VkQueue& GetVulkanSDKPresentQueue() const { return vk_presentQueue; }
	/* End member variable getters */
private:
	//Finds all the available graphics cards and checks which one is suitable and saves it for logical device creation
	void ChoosePhysicalDevice(const VkInstance& vk_instance, const VkSurfaceKHR& vk_surface);

	//Called by the ChoosePhysicalDevice function to see if a device is suitable
	bool CheckDeviceSuitability(const VkPhysicalDevice& device, const VkSurfaceKHR& vk_surface);

	//Called by the CheckDeviceSuitability function to save the device's queue family indices if they exist
	void FindDeviceQueueFamilyIndices(const VkPhysicalDevice& device, const VkSurfaceKHR& vk_surface);

	//Called by CheckDeviceSuitability to see if a device supports the extensions specified in the deviceExtensions array
	bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);

	//Called by the CheckDeviceSuitability function to save the device's swapchain support details
	void GetDeviceSwapchainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);


	//Called after a GPU has been chosen and creates the logical device to interface with it
	void SetupLogicalDevice(const VkInstance& vk_instance);
private:
	//Vulkan SDK physical device object, used to represent the chosen GPU that the app will interface with
	VkPhysicalDevice vk_GraphicsCard;

	//Holds the chosen GPU's queue family indices
	QueueFamilyIndices m_GPUQueueFamilyIndices;

	//Holds the chosen GPU's swaphcain support details
	SwapchainSupportDetails m_GPUSwapchainSupportDetails;

	//Device class of the vulkan SDK, used to interface with the chosen GPU
	VkDevice vk_device;

	//Queue class of the vulkan SDK for the graphics queue which was automatically created with the logical device
	VkQueue vk_graphicsQueue;

	//Queue class of the vulkan SDK for the present queue which was automatically created with the logical device
	VkQueue vk_presentQueue;
};