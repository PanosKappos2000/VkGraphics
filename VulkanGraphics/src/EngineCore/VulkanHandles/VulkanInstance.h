#pragma once

#include <iostream>
#include "EngineCore/Window/GlfwWindowHandle.h"


/**********************************************
* Holds the instance class of the vulkan SDK, * 
* required to interface with the SDK itself   *
**********************************************/
class VulkanInstanceHandle
{
public:
	//Creates the vulkan instance
	void CreateVulkanInstance(const GlfwWindowHandle& window);

	//Cleans up the vulkan instance and other member variables
	void Cleanup();

	/* Member variable getters */
	const VkInstance& GetVulkanSDKInstance() const
	{
		return vk_instance;
	}
	/* End member variable getters */
private:

private:
	//The instance class of the vuklan SDK
	VkInstance vk_instance;
};