#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
//This macro will have GLFW include its own definitions for vulkan
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define WINDOW_STANDARD_WIDTH		720
#define WINDOW_STANDARD_HEIGHT		560


/***************************************************************
* Holds the window opened with glfw and handles its operations *
****************************************************************/
class GlfwWindowHandle
{
public:
	//Constructor, explicitly defined to initialize member variables
	GlfwWindowHandle();

	//Initializes glfw and creates the primary window
	void CreateGlfwWindow();

	//Terminates glfw after destroying the window
	void Cleanup();

	/* Class getters */
	const GLFWwindow* const GetGlfwWindow() const
	{ 
		return glfw_window;
	}

	uint32_t GetExtensionCount() const
	{
		return m_extensionCount;
	}

	const char** GetExtensionNames() const
	{
		return m_extensionNames;
	}

	inline uint32_t GetWindowWidth() const { return WINDOW_STANDARD_WIDTH; }

	inline uint32_t GetWindowHeight() const { return WINDOW_STANDARD_HEIGHT; }
	/* End of class getters */

	//Wrapper for the poll events glfw function
	void CheckEvents();

	//Wrapper for the window should close glfw function
	bool CheckIfWindowShouldClose() const;

	//Creates the abstract window surface used for vulkan to interact with the window system
	void CreateVulkanWindowSurface(const VkInstance& vk_instance, VkSurfaceKHR& vk_surface) const;

private:
	
public:

private:
	//The pointer to the actual glfw window
	GLFWwindow* glfw_window;

	//Extensions needed for glfw to work with vulkan
	uint32_t m_extensionCount;
	const char** m_extensionNames;
};