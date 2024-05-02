#include "GlfwWindowHandle.h"

GlfwWindowHandle::GlfwWindowHandle()
	:glfw_window{ nullptr },m_extensionCount{0}, 
	m_extensionNames{nullptr}
{

}

void GlfwWindowHandle::CreateGlfwWindow()
{
	//Initializing glfw and checking if it was succesful
	int glfwSuccess = glfwInit();
	if (!glfwSuccess)
		__debugbreak();

	//Explicitly stating that glfw should not create an Opengl context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Creating the window and checking if its creation was succesful
	glfw_window = glfwCreateWindow(WINDOW_STANDARD_WIDTH, WINDOW_STANDARD_HEIGHT, "Vulkan", nullptr, nullptr);
	if (!glfw_window)
	{
		__debugbreak();
	}

	//Using glfw's built in function to get required extension names and count
	m_extensionNames = glfwGetRequiredInstanceExtensions(&m_extensionCount);
}

void GlfwWindowHandle::CreateVulkanWindowSurface(const VkInstance& vk_instance, VkSurfaceKHR& vk_surface) const
{
	//Using glfw's built in function to initialize a window surface for vulkan to use
	VkResult surfaceSuccess = glfwCreateWindowSurface(vk_instance, glfw_window, nullptr, &vk_surface);
	if (surfaceSuccess != VK_SUCCESS)
		__debugbreak();
}

void GlfwWindowHandle::CheckEvents()
{
	glfwPollEvents();
}

bool GlfwWindowHandle::CheckIfWindowShouldClose() const
{
	return glfwWindowShouldClose(glfw_window);
}

void GlfwWindowHandle::Cleanup()
{
	glfwDestroyWindow(glfw_window);
	glfwTerminate();
}