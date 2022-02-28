#include <array>
#include "display/vulkan/graphicspipeline.h"
#include "display/vulkan/shaderstagemodules.h"

#include "core/logger.h"

static Boiler::Logger logger("Vulkan Graphics Pipeline");;

using namespace Boiler::Vulkan;

const std::string GraphicsPipeline::SHADER_ENTRY = "main";

GraphicsPipeline::GraphicsPipeline() : GraphicsPipeline(VK_NULL_HANDLE)
{
}

GraphicsPipeline::GraphicsPipeline(VkPipeline pipeline)
{
	this->pipeline = pipeline;
}

GraphicsPipeline GraphicsPipeline::create(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkExtent2D swapChainExtent,
										  const VkVertexInputBindingDescription *inputBind, const std::vector<VkVertexInputAttributeDescription> *attrDescs,
										  const int attachmentCount, const ShaderStageModules &shaderModules, int subpassIndex,
										  VkCullModeFlags cullMode, bool enableDepth, VkCompareOp depthCompareOp, bool flipViewport, bool useTexture)
{
	VkPipelineVertexInputStateCreateInfo vertInputCreateInfo = {};
	vertInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInputCreateInfo.vertexBindingDescriptionCount = (inputBind != nullptr) ? 1 : 0;
	vertInputCreateInfo.pVertexBindingDescriptions = inputBind;
	vertInputCreateInfo.vertexAttributeDescriptionCount = (attrDescs != nullptr) ? attrDescs->size() : 0;
	vertInputCreateInfo.pVertexAttributeDescriptions = (attrDescs != nullptr) ? attrDescs->data() : nullptr;

	// Input assembly stage
	VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {};
	assemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	// Viewport and scissor setup
	VkViewport viewport = {};
	if (flipViewport)
	{
		viewport.x = 0;
		viewport.y = static_cast<float>(swapChainExtent.height);
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = -static_cast<float>(swapChainExtent.height);
	}
	else
	{
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
	}
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;

	// Rasterizer configuration
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.cullMode = cullMode;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizerCreateInfo.depthBiasClamp = 0.0f;
	rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multiSampCreateInfo = {};
	multiSampCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSampCreateInfo.sampleShadingEnable = VK_FALSE;
	multiSampCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multiSampCreateInfo.minSampleShading = 1.0f;
	multiSampCreateInfo.pSampleMask = nullptr;
	multiSampCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multiSampCreateInfo.alphaToOneEnable = VK_FALSE;

	// Colour blending configuration
	std::vector<VkPipelineColorBlendAttachmentState> blendStates{};
	for (int i = 0; i < attachmentCount; ++i)
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachState = {};
		colorBlendAttachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachState.blendEnable = VK_TRUE;
		colorBlendAttachState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachState.alphaBlendOp = VK_BLEND_OP_ADD;

		blendStates.push_back(colorBlendAttachState);
	}
	
	
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendCreateInfo.attachmentCount = blendStates.size();
	colorBlendCreateInfo.pAttachments = blendStates.data();
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	// depth/stencil buffer setup
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = enableDepth ? VK_TRUE : VK_FALSE;
	depthStencilInfo.depthWriteEnable = enableDepth ? VK_TRUE : VK_FALSE;
	depthStencilInfo.depthCompareOp = depthCompareOp;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.stencilTestEnable = VK_FALSE;

	std::array<VkSpecializationMapEntry, 1> mapEntries
	{
		VkSpecializationMapEntry
		{
			.constantID = 0,
			.offset = 0,
			.size = sizeof(VkBool32),
		}
	};

	size_t dataSize = 0;
	for (const auto &entry : mapEntries)
	{
		dataSize += entry.size;
	}

	VkBool32 vkUseTexture = useTexture;
	VkSpecializationInfo specializationCreateInfo = {};
	specializationCreateInfo.dataSize = dataSize;
	specializationCreateInfo.mapEntryCount = mapEntries.size();
	specializationCreateInfo.pMapEntries = mapEntries.data();
	specializationCreateInfo.pData = &vkUseTexture;

	auto createStageInfo = [](VkShaderModule module, VkShaderStageFlagBits stage, VkSpecializationInfo *specializationInfo)
	{
		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = stage;
		stageInfo.module = module;
		stageInfo.pName = SHADER_ENTRY.c_str();
		stageInfo.pSpecializationInfo = specializationInfo;
		return stageInfo;
	};

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {
		createStageInfo(shaderModules.vertex, VK_SHADER_STAGE_VERTEX_BIT, nullptr),
		createStageInfo(shaderModules.fragment, VK_SHADER_STAGE_FRAGMENT_BIT, &specializationCreateInfo)
	};

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertInputCreateInfo;
	pipelineInfo.pInputAssemblyState = &assemblyCreateInfo;
	pipelineInfo.pViewportState = &viewportCreateInfo;
	pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineInfo.pMultisampleState = &multiSampCreateInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pColorBlendState = &colorBlendCreateInfo;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = subpassIndex;

	// not deriving a pipeline
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	VkPipeline pipeline = VK_NULL_HANDLE;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating graphics pipeline");
	}
	logger.log("Created new pipeline");

	return GraphicsPipeline(pipeline);
}

void GraphicsPipeline::destroy(VkDevice device, const GraphicsPipeline &pipeline)
{
	assert(pipeline.vulkanPipeline() != VK_NULL_HANDLE);

	vkDestroyPipeline(device, pipeline.vulkanPipeline(), nullptr);
	logger.log("Destroyed pipeline");
}
