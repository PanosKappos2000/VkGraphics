#pragma once

#include "EngineCore/VulkanHandles/VulkanInstance.h"

/***********************************************
* Holds the window surface of the vulkan SDK,  *
* required to interface with the window system *
***********************************************/
class VulkanSurfaceHandle
{
public:
	//Creates the vulkan surface, handled by a glfw built in function
	void CreateVulkanSurface(const GlfwWindowHandle& window, const VkInstance& vk_instance);

	//Destroys the vulkan surface
	void Cleanup(const VkInstance& vk_instance);

	/* Member variable getters */
	const VkSurfaceKHR& GetVulkanSDKSurface() const
	{
		return vk_surface;
	}
	/* End member variable getters */
private:

private:
	//The surface class of the vuklan SDK
	VkSurfaceKHR vk_surface;
};