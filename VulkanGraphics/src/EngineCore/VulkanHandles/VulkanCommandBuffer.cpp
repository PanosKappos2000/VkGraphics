#include "EngineCore/VulkanCore.h"

void VulkanCommandBufferHandle::CreateCommandBuffer(const VulkanDeviceHandle& device)
{
	CreateCommandPool(device);
	CreateCommandBufferInner(device.GetVulkanSDKLogicalDevice());
}

void VulkanCommandBufferHandle::RecordCommandBuffer(const VulkanSwapchainHandle& swapchain,
	const VulkanGraphicsPipelineHandle& graphicsPipeline, const VulkanFramebufferHandle& framebuffer,
	uint32_t imageIndex)
{
	//Starting the command buffer
	VkCommandBufferBeginInfo commandBufferBegin{};
	commandBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBegin.flags = 0; 
	commandBufferBegin.pInheritanceInfo = nullptr; 

	VkResult commandBufferBeginResult = vkBeginCommandBuffer(vk_commandBuffer, &commandBufferBegin);
	if (commandBufferBeginResult != VK_SUCCESS)
	{
		__debugbreak();
	}

	//Starting the render pass
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = graphicsPipeline.GetVulkanSDKRenderPass();
	renderPassInfo.framebuffer = (framebuffer.GetVulkanSDKFramebuffers())[imageIndex];
	renderPassInfo.renderArea.extent = swapchain.GetSwapchainExtent();
	renderPassInfo.renderArea.offset = { 0, 0 };

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(vk_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//Starting the vulkan pipeline
	vkCmdBindPipeline(vk_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.GetVulkanSDKGraphicsPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain.GetSwapchainExtent().width);
	viewport.height = static_cast<float>(swapchain.GetSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vk_commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain.GetSwapchainExtent();
	vkCmdSetScissor(vk_commandBuffer, 0, 1, &scissor);

	//Start drawing
	vkCmdDraw(vk_commandBuffer, 3, 1, 0, 0);

	//Ending the render pass and the command buffer
	vkCmdEndRenderPass(vk_commandBuffer);
	VkResult endCommandBufferResult = vkEndCommandBuffer(vk_commandBuffer);
	if (endCommandBufferResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void VulkanCommandBufferHandle::CreateCommandPool(const VulkanDeviceHandle& device)
{
	/* Initializing create info struct for command pool */
	VkCommandPoolCreateInfo commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//This command pool is being used for graphics commands, so it gets the graphics queue family
	commandPoolInfo.queueFamilyIndex = device.GetQueueFamilyGraphicsIndex();
	//A command buffer will be recorded every frame
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult commandPoolResult = vkCreateCommandPool(device.GetVulkanSDKLogicalDevice(),
		&commandPoolInfo, nullptr, &vk_commandPool);
	if (commandPoolResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void VulkanCommandBufferHandle::CreateCommandBufferInner(const VkDevice& device)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vk_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkResult commandBufferResult = vkAllocateCommandBuffers(device, &allocInfo, &vk_commandBuffer);
	if (commandBufferResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void VulkanCommandBufferHandle::Cleanup(const VkDevice& device)
{
	vkDestroyCommandPool(device, vk_commandPool, nullptr);
}