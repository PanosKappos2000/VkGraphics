#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "VulkanDevice.h"

class VulkanGraphicsPipelineHandle
{
public:
	//Creates the render pass needed for pipeline creation
	void CreateRenderPass(const VkFormat& swapchainFormat, const VkDevice& device);

	//Creates the graphics pipeline after reading shader code, specifying fixed functions,
	//and creating the pipeline layout
	void CreateGraphicsPipeline(const VkDevice& device, const VkExtent2D& swapchainExtent);

	void Cleanup(const VkDevice& device);

	/* Member variable getters */
	inline const VkPipelineLayout& GetVulkanSDKPipelineLayout() const { return vk_pipelineLayout; }

	inline const VkRenderPass& GetVulkanSDKRenderPass() const { return vk_renderPass; }

	inline const VkPipeline& GetVulkanSDKGraphicsPipeline() const { return vk_graphicsPipeline; }
	/* End member variable getters */
private:
	void ReadFile(const std::string& filename, std::vector<char>& byteCode);

	//Creates the shader module used to wrap around the code of the shaders
	void CreateShaderModule(const std::vector<char>& code, const VkDevice& device, 
		VkShaderModule& shader);
private:
	//Holds the graphics pipeline object 
	VkPipeline vk_graphicsPipeline;

	//Used to specify the layouts used to pass uniform variables to shaders
	VkPipelineLayout vk_pipelineLayout;

	//Holds important information about rendering operations
	//( color and depth buffers, samples to use for them)
	VkRenderPass vk_renderPass;
};