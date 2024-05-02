#include "VulkanGraphicsPipeline.h"

/*************************************************************************************************
* Function argument 1: The swapchain format needs to be passed in the description struct for the * 
*					   attachment(s)															 *
* Function argument 2: The Vulkan SDK device is needed for the creation of the render pass		 *
*************************************************************************************************/
void VulkanGraphicsPipelineHandle::CreateRenderPass(const VkFormat& swapchainFormat, const VkDevice& device)
{
	/* Initializing attachment description struct */
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainFormat;
	//Not using multisampling so 1 sample
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//Clearing the framebuffer to black before rendering
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//Storing rendered objects so they can be shown on screen
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//Setting up what happens with the stencil buffer before and after drawing
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//Setting up how pixels are treated(important for texturing)
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	/* Attachment description struct complete */

	/* Initializing attachment reference struct, needed to pass an attachment to a render pass */
	VkAttachmentReference colorAttachmentRef{};
	//Specifying the index of the attachment that this references 
	// (the array is passed in the render pass create info sturct)
	colorAttachmentRef.attachment = 0;
	//The desired layout this attachment will have during a subpass
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	/* Attachment reference struct complete */

	/* Initializing subpass description struct */
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//Passing the color attachment reference(s) for the struct
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	/* Subpass description struct complete */

	/* Initializing render pass create info struct */
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//Passing the attachemnt(s)
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	//Passing the subpass(es)
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	/* Render pass create info complete */

	//Creating a subpass dependency for the render pass
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	//Creating the render pass and checking if its creation was succesful
	VkResult renderPassResult = vkCreateRenderPass(device, &renderPassInfo, nullptr, &vk_renderPass);
	if (renderPassResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

/*******************************************************************************
* Function Argument 1: The Vulkan SDK device object is needed for the creation *
*					   of both the pipeline layout and the graphics pipeline   *
* Function Argument 2: The swapchain extent is needed for setting viewport and *
*					   scissor values										   *
*******************************************************************************/
void VulkanGraphicsPipelineHandle::CreateGraphicsPipeline(const VkDevice& device, 
	const VkExtent2D& swapchainExtent)
{
	//Reading and loading the shader code in memory
	std::vector<char> vertShaderCode;
	std::vector<char> fragShaderCode;
	ReadFile("Shaders/vert.spv", vertShaderCode);
	ReadFile("Shaders/frag.spv", fragShaderCode);

	//The code needs to be wrapped in a shader module before being passed to the graphics pipeline
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	CreateShaderModule(vertShaderCode, device, vertexShaderModule);
	CreateShaderModule(fragShaderCode, device, fragmentShaderModule);

	/* Create info struct for shader stage (specifies for what stage the shader will be used) */
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexShaderModule;
	vertShaderStageInfo.pName = "main";
	/* Create info struct complete */

	/* Doing the same as above for the fragment shader now */
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentShaderModule;
	fragShaderStageInfo.pName = "main";
	/* Create info struct complete */

	//Saving the shader stages to an array, so that they can be passed to the pipeline
	VkPipelineShaderStageCreateInfo shaderStageInfos[] = { vertShaderStageInfo, fragShaderStageInfo };

	//Setting which parts of the pipeline state can be changed at draw time
	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//Setting up the vertex data that will passed on to the vertex shader
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	//Setting up what kind of geometry will be drawn from the vertices and if primitive restart should be enabled
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//Setting up viewport values(this is unnecessary since it's specified as dynamic state)
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	//Setting up scissor values for the viewport(this is unnecessary since it's specified as dynamic state)
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;
	//Specifying the scissor and viewport count only since they are specified as dynamic states
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	//Configuring the rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	//Useful for shadow maps but needs a GPU feature enabled
	rasterizer.depthClampEnable = VK_FALSE;
	//rasterizerDiscardEnable would stop everything from being rendered as nothing would pass the rasterization stage
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	//Setting the type of face culling to use
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	//Depth can be used for shadow mapping sometimes
	rasterizer.depthBiasEnable = VK_FALSE;

	//Setting up multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//Setting up depth and stencil buffers

	//Configuring color blending for the framebuffer(s)
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	//Configuring global color blending
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; 
	colorBlending.blendConstants[1] = 0.0f; 
	colorBlending.blendConstants[2] = 0.0f; 
	colorBlending.blendConstants[3] = 0.0f; 
	
	//Setting up uniform variables layouts
	VkPipelineLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = 0;
	createInfo.pSetLayouts = nullptr;
	createInfo.pushConstantRangeCount = 0;
	createInfo.pPushConstantRanges = nullptr;

	//Creating the pipeline layout
	VkResult pipelineLayoutResult = vkCreatePipelineLayout(device, &createInfo, nullptr, &vk_pipelineLayout);
	if (pipelineLayoutResult != VK_SUCCESS)
	{
		__debugbreak();
	}

	/* Initializing Graphics pipeline create info struct */
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//Passing the shader stage array
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStageInfos;
	//Passing all the fixed function states
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	//Passing the pipeline layout object
	pipelineInfo.layout = vk_pipelineLayout;
	//Passing the render pass
	pipelineInfo.renderPass = vk_renderPass;
	//Passing the index of the subpass where the graphics pipeline will be used
	pipelineInfo.subpass = 0;

	//This is used if we want to have more than one pipeline
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
	pipelineInfo.basePipelineIndex = -1; 

	//Creating the graphics pipeline and checking if its creation was succesful
	VkResult graphicsPipelineResult = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, 
		nullptr, &vk_graphicsPipeline);
	if (graphicsPipelineResult != VK_SUCCESS)
	{
		__debugbreak();
	}

	//We don't need the shader module wrappers after the graphics pipeline has been created
	vkDestroyShaderModule(device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
}

void VulkanGraphicsPipelineHandle::ReadFile(const std::string& filename, 
	std::vector<char>& byteCode)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		__debugbreak();
	}

	size_t fileSize = file.tellg();
	byteCode.resize(fileSize);
	file.seekg(0);
	file.read(byteCode.data(), fileSize);

	file.close();
}

/************************************************************************************
*Function argument 1: The compiled code of the shader file that the shader module   *
*					   will wrap around                                             *
* Function argument 2: The device is required for the creation of the shader module *
* Function argument 3: The shader module that will be passed to the Vulkan SDK's    *
*					   create function, and wrap around the shader code             *
************************************************************************************/
void VulkanGraphicsPipelineHandle::CreateShaderModule(const std::vector<char>& code, 
	const VkDevice& device, VkShaderModule& shader)
{
	/* Initializing create info struct for shader module */
	VkShaderModuleCreateInfo createInfo{};
	createInfo.codeSize = code.size();
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	/* Create info struct complete */

	//Creating the shader module and checking if its creation was succesful
	VkResult shaderResult = vkCreateShaderModule(device, &createInfo, nullptr, &shader);
	if (shaderResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void VulkanGraphicsPipelineHandle::Cleanup(const VkDevice& device)
{
	vkDestroyPipeline(device, vk_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, vk_pipelineLayout, nullptr);
	vkDestroyRenderPass(device, vk_renderPass, nullptr);
}