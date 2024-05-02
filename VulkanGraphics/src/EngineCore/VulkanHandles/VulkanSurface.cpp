#include "VulkanSurface.h"

/****************************************************************************************
* Function Argument 1: Used to call the window handle's create window surface function  *
*                      that in turn  calls the glfw function for surface creation       *
* Function Argument 2: The instance object of vulkan SDK is needed for surface creation *
****************************************************************************************/
void VulkanSurfaceHandle::CreateVulkanSurface(const GlfwWindowHandle& window, const VkInstance& vk_instance)
{
	window.CreateVulkanWindowSurface(vk_instance, vk_surface);
}

/*****************************************************************************************      
* Function Argument 1: The instance object of vulkan SDK is need for surface destruction *
*****************************************************************************************/
void VulkanSurfaceHandle::Cleanup(const VkInstance& vk_instance)
{
	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
}