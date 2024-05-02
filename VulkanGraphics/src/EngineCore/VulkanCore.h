#pragma once

#include "EngineCore/Window/GlfwWindowHandle.h"
#include "EngineCore/VulkanHandles/VulkanInstance.h"
#include "EngineCore/VulkanHandles/VulkanSurface.h"
#include "EngineCore/VulkanHandles/VulkanDevice.h"
#include "EngineCore/VulkanHandles/VulkanSwapchain.h"
#include "EngineCore/VulkanHandles/VulkanImageViews.h"
#include "EngineCore/VulkanHandles/VulkanGraphicsPipeline.h"



#define PRIMARY_WINDOW_WIDTH 720
#define PRIMARY_WINDOW_HEIGHT 560

/**************************************************
* Holds an array that stores all the framebuffers *
* created based on the image views				  *
**************************************************/
class VulkanFramebufferHandle
{
public:
	//Fills the array of framebuffers by creating one based on specific image views and render pass
	void CreateFramebuffers(const std::vector<VkImageView>& imageViews,
		const VkRenderPass& renderPass, const VkExtent2D& swapchainExtent,
		const VkDevice& device);

	void Cleanup(const VkDevice& device);

	/* Member variable getters */
	inline const std::vector<VkFramebuffer>& GetVulkanSDKFramebuffers() const { return vk_framebuffers; }
	/* Member variable getters end */
private:

private:
	//Holds all the vulkan SDK framebuffers based on the image views
	std::vector<VkFramebuffer> vk_framebuffers;
};


/******************************************************************
* Holds the command buffers used to make various vulkan commands, *
* and the command pool that allocates them						  *
******************************************************************/
class VulkanCommandBufferHandle
{
public:
	//Creates the command buffer and the command pool
	void CreateCommandBuffer(const VulkanDeviceHandle& device);

	void Cleanup(const VkDevice& device);

	void RecordCommandBuffer(const VulkanSwapchainHandle& swapchain, 
		const VulkanGraphicsPipelineHandle& graphicsPipeline,const VulkanFramebufferHandle& framebuffer,
		uint32_t imageIndex);

	inline const VkCommandPool& GetVulkanSDKCommandPool() const { return vk_commandPool; }

	inline const VkCommandBuffer& GetVulkanSDKCommandBuffer() const { return vk_commandBuffer; }
private:
	//Called by CreateCommandBuffer to create the command pool before creating the command buffer
	void CreateCommandPool(const VulkanDeviceHandle& device);

	//Called by CreateCommandBuffer to create the actual command buffer after the command pool
	void CreateCommandBufferInner(const VkDevice& device);
private:
	//Holds the command pool
	VkCommandPool vk_commandPool;

	VkCommandBuffer vk_commandBuffer;
};



class VulkanSyncObjectsHandle
{
public:
	void CreateSyncObjects(const VkDevice& device);

	void Cleanup(const VkDevice& device);
public:
	VkSemaphore vk_imageAvailableSemaphore;
	VkSemaphore vk_renderFinishedSemaphore;
	VkFence vk_inFlightFence;
};



/****************************************************************
* Holds one of each vulkan handle objects defined previously,   *
* and is responsible for calling and operating vulkan functions *
****************************************************************/
class VulkanTriangle
{	
public:
	//The constructor is explicitly defined for placeholder intializations for all the handles
	VulkanTriangle();

	//Used in the main loop to excecute all the rendering operations that this class is responsible for
	void RunTriangle();

private:
	void VulkanInit();

	void DrawFrame();

	static std::vector<char> ReadFile(const std::string& filename);

	//Cleans up all of the vulkan handles that were explicitly created
	void VulkanDestroy();

private:
	//Used to initialize the window system and interface with it
	GlfwWindowHandle m_windowHandle;

	//Used to initialize the instance and access when it's needed
	VulkanInstanceHandle m_vulkanInstance;

	//Used to initialize the window surface for vulkan and access when it's needed
	VulkanSurfaceHandle m_vulkanSurface;

	//Used to intialize the vulkan device, interface with it and its queues and access support details of the GPU
	VulkanDeviceHandle m_vulkanDevice;

	//Used to initialize the swapchain, interface with it and access its details and images
	VulkanSwapchainHandle m_vulkanSwapchain;

	//Used to initialize the image views, and interface with the array that holds them
	VulkanImageViewsHandle m_vulkanImageViews;

	//Initializes the render pass and the graphics pipeline and sets them according to the application's needs
	VulkanGraphicsPipelineHandle m_vulkanPipeline;

	VulkanFramebufferHandle m_vulkanFramebuffers;

	VulkanCommandBufferHandle m_vulkanCommandBuffer;

	VulkanSyncObjectsHandle m_vulkanSyncObjects;
};