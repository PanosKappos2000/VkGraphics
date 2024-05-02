#include "VulkanCore.h"

VulkanTriangle::VulkanTriangle()
	:m_windowHandle(), m_vulkanInstance(), m_vulkanSurface(),
	m_vulkanDevice(), m_vulkanSwapchain(), m_vulkanImageViews(),
	m_vulkanPipeline()
{

}

void VulkanTriangle::VulkanInit()
{
	//The window will be created first, since its used in vulkan instance creation
	m_windowHandle.CreateGlfwWindow();

	//The instance is created first as the Vulkan SDK cannot be accessed without it
	m_vulkanInstance.CreateVulkanInstance(m_windowHandle);

	//After the instance, the surface will be created
	m_vulkanSurface.CreateVulkanSurface(m_windowHandle, m_vulkanInstance.GetVulkanSDKInstance());

	//The logical device is created after the surface, as it needs the surface to find the device's swapchain support details
	m_vulkanDevice.CreateVulkanLogicalDevice(m_vulkanInstance, m_vulkanSurface.GetVulkanSDKSurface());

	//The swaphcain is created after the surface, as it needs the device, the window and the surface for its creation
	m_vulkanSwapchain.CreateSwapchain(m_vulkanSurface.GetVulkanSDKSurface(),
		m_vulkanDevice, m_windowHandle);

	//The image views are created after the swaphcain, as they are based on the swaphcain images
	m_vulkanImageViews.CreateImageViews(m_vulkanSwapchain, m_vulkanDevice.GetVulkanSDKLogicalDevice());

	//Creating the render pass before the graphics pipeline, 
	//as it needs to be passed in the pipeline's create info struct
	m_vulkanPipeline.CreateRenderPass(m_vulkanSwapchain.GetSwapchainImageFormat(), 
		m_vulkanDevice.GetVulkanSDKLogicalDevice());

	//Creating the graphics pipeline after the render pass
	m_vulkanPipeline.CreateGraphicsPipeline(m_vulkanDevice.GetVulkanSDKLogicalDevice(), 
		m_vulkanSwapchain.GetSwapchainExtent());

	//Creating the framebuffers based on the image views and each compatible with our render pass
	m_vulkanFramebuffers.CreateFramebuffers(m_vulkanImageViews.GetVulkanSDKImageViews(),
		m_vulkanPipeline.GetVulkanSDKRenderPass(), m_vulkanSwapchain.GetSwapchainExtent(),
		m_vulkanDevice.GetVulkanSDKLogicalDevice());

	m_vulkanCommandBuffer.CreateCommandBuffer(m_vulkanDevice);

	m_vulkanSyncObjects.CreateSyncObjects(m_vulkanDevice.GetVulkanSDKLogicalDevice());
}

std::vector<char> VulkanTriangle::ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) 
	{
		__debugbreak();
	}

	size_t fileSize = file.tellg();
	std::vector<char> byteCode(fileSize);
	file.seekg(0);
	file.read(byteCode.data(), fileSize);

	file.close();
	return byteCode;
}

void VulkanTriangle::VulkanDestroy()
{
	// Since a lot of the cleanup code requires the vulkan logical device,
	// it gets referenced here to avoid calling the getter function every time 
	const VkDevice& device = m_vulkanDevice.GetVulkanSDKLogicalDevice();

	/**************************************************************************************
	* Vulkan objects will have to be cleaned up in opposite order to their initialization *
	**************************************************************************************/
	m_vulkanSyncObjects.Cleanup(device);
	m_vulkanCommandBuffer.Cleanup(device);
	m_vulkanFramebuffers.Cleanup(device);
	m_vulkanPipeline.Cleanup(device);
	m_vulkanImageViews.Cleanup(device);
	m_vulkanSwapchain.Cleanup(device);
	m_vulkanDevice.Cleanup();
	m_vulkanSurface.Cleanup(m_vulkanInstance.GetVulkanSDKInstance());
	m_vulkanInstance.Cleanup();
	m_windowHandle.Cleanup();
}

void VulkanTriangle::RunTriangle()
{
	VulkanInit();
	while (!m_windowHandle.CheckIfWindowShouldClose())
	{
		m_windowHandle.CheckEvents();
		DrawFrame();
	}
	vkDeviceWaitIdle(m_vulkanDevice.GetVulkanSDKLogicalDevice());
	VulkanDestroy();
}

void VulkanTriangle::DrawFrame()
{
	//Waiting for the previous frame and reseting the fence when we get the signal
	vkWaitForFences(m_vulkanDevice.GetVulkanSDKLogicalDevice(), 1, &m_vulkanSyncObjects.vk_inFlightFence,
		VK_TRUE, UINT64_MAX);
	vkResetFences(m_vulkanDevice.GetVulkanSDKLogicalDevice(), 1, &m_vulkanSyncObjects.vk_inFlightFence);

	//We'll need an image index to give to the present queue later
	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_vulkanDevice.GetVulkanSDKLogicalDevice(), m_vulkanSwapchain.GetVulkanSDKSwapchain(),
		UINT64_MAX, m_vulkanSyncObjects.vk_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	//Resetting the command buffer and recording it
	vkResetCommandBuffer(m_vulkanCommandBuffer.GetVulkanSDKCommandBuffer(), 0);
	m_vulkanCommandBuffer.RecordCommandBuffer(m_vulkanSwapchain, m_vulkanPipeline, m_vulkanFramebuffers, imageIndex);

	//Create the submit info needed to submit the queue
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//Specifying that an image should become available before executing color attachment
	VkSemaphore waitSemaphores[] = { m_vulkanSyncObjects.vk_imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	//Passing the command buffer which has already been recorded
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_vulkanCommandBuffer.GetVulkanSDKCommandBuffer();

	//Specifying that a signal should be sent that render has finished once the queue is done
	VkSemaphore signalSemaphores[] = { m_vulkanSyncObjects.vk_renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkQueueSubmit(m_vulkanDevice.GetVulkanSDKGraphicsQueue(), 1, &submitInfo, m_vulkanSyncObjects.vk_inFlightFence);

	//Now that graphics has been submitted, the frame can be presented back to the swapchain
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	//The presentation should wait for rendering operations to finish
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	//Submitting the swapchain and the image index
	VkSwapchainKHR swapchains[] = { m_vulkanSwapchain.GetVulkanSDKSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(m_vulkanDevice.GetVulkanSDKPresentQueue(), &presentInfo);
}





void VulkanSyncObjectsHandle::CreateSyncObjects(const VkDevice& device)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//Create the fence into the signaled state so that we don't wait infinitely for the first frame
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult imageViewSemaphoreSuccess = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vk_imageAvailableSemaphore);
	VkResult renderSemaphoreSuccess = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &vk_renderFinishedSemaphore);
	VkResult frameFenceSuccess = vkCreateFence(device, &fenceInfo, nullptr, &vk_inFlightFence);

	if (imageViewSemaphoreSuccess != VK_SUCCESS || renderSemaphoreSuccess != VK_SUCCESS || frameFenceSuccess != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void VulkanSyncObjectsHandle::Cleanup(const VkDevice& device)
{
	vkDestroySemaphore(device, vk_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(device, vk_renderFinishedSemaphore, nullptr);
	vkDestroyFence(device, vk_inFlightFence, nullptr);
}