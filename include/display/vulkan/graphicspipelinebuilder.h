#ifndef GRAPHICSPIPELINEBUILDER_H
#define GRAPHICSPIPELINEBUILDER_H

#include <vector>
#include <array>
#include "display/vulkan/graphicspipeline.h"
#include "display/vulkan/shaderstagemodules.h"
#include "core/logger.h"

namespace Boiler { namespace Vulkan {

struct AttachmentBlendInfo
{
	bool enabled;
};

const std::string SHADER_ENTRY = "main";

template<unsigned short NumAttachments>
class GraphicsPipelineBuilder
{
	Logger logger;
	VkDevice device;
	VkPipelineLayout layout;
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	VkPipelineVertexInputStateCreateInfo vertInputCreateInfo = {};
	VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {};
	VkViewport pipelineViewport = {};
	VkRect2D scissor = {};
	VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	VkPipelineMultisampleStateCreateInfo multiSampCreateInfo = {};
	std::array<VkPipelineColorBlendAttachmentState, NumAttachments> blendStates{};
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
	VkPipelineDynamicStateCreateInfo dynaInfo{};
	std::array<VkSpecializationMapEntry, 1> mapEntries;
	VkSpecializationInfo specializationCreateInfo = {};
	VkBool32 vkUseTexture;

public:
	GraphicsPipelineBuilder(VkDevice device, VkPipelineLayout layout) : logger("Graphics Pipeline Builder")
	{
		this->device = device;
		this->layout = layout;
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = layout;
		// not deriving a pipeline
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.pDynamicState = nullptr;
	}

	GraphicsPipelineBuilder &assembly(VkVertexInputBindingDescription *inputBind, std::vector<VkVertexInputAttributeDescription> *attrDescs, VkPrimitiveTopology topology)
	{
		vertInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputCreateInfo.vertexBindingDescriptionCount = (inputBind != nullptr) ? 1 : 0;
		vertInputCreateInfo.pVertexBindingDescriptions = inputBind;
		vertInputCreateInfo.vertexAttributeDescriptionCount = (attrDescs != nullptr) ? attrDescs->size() : 0;
		vertInputCreateInfo.pVertexAttributeDescriptions = (attrDescs != nullptr) ? attrDescs->data() : nullptr;

		assemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyCreateInfo.topology = topology;
		assemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		pipelineInfo.pVertexInputState = &vertInputCreateInfo;
		pipelineInfo.pInputAssemblyState = &assemblyCreateInfo;

		return *this;
	}

	GraphicsPipelineBuilder &viewport(bool flipViewport, VkExtent2D swapChainExtent)
	{
		// Viewport and scissor setup
		if (flipViewport)
		{
			pipelineViewport.x = 0;
			pipelineViewport.y = static_cast<float>(swapChainExtent.height);
			pipelineViewport.width = static_cast<float>(swapChainExtent.width);
			pipelineViewport.height = -static_cast<float>(swapChainExtent.height);
		}
		else
		{
			pipelineViewport.x = 0;
			pipelineViewport.y = 0;
			pipelineViewport.width = static_cast<float>(swapChainExtent.width);
			pipelineViewport.height = static_cast<float>(swapChainExtent.height);
		}
		pipelineViewport.minDepth = 0;
		pipelineViewport.maxDepth = 1;

		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;

		viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportCreateInfo.viewportCount = 1;
		viewportCreateInfo.pViewports = &pipelineViewport;
		viewportCreateInfo.scissorCount = 1;
		viewportCreateInfo.pScissors = &scissor;

		pipelineInfo.pViewportState = &viewportCreateInfo;

		return *this;
	}

	GraphicsPipelineBuilder &rasterizer(VkCullModeFlags cullMode)
	{
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

		pipelineInfo.pRasterizationState = &rasterizerCreateInfo;

		return *this;
	}

	GraphicsPipelineBuilder &multisampling()
	{
		multiSampCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSampCreateInfo.sampleShadingEnable = VK_FALSE;
		multiSampCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multiSampCreateInfo.minSampleShading = 1.0f;
		multiSampCreateInfo.pSampleMask = nullptr;
		multiSampCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multiSampCreateInfo.alphaToOneEnable = VK_FALSE;

		pipelineInfo.pMultisampleState = &multiSampCreateInfo;

		return *this;
	}

	GraphicsPipelineBuilder &blending(const std::array<AttachmentBlendInfo, NumAttachments> &blendingInfos)
	{
		for (int i = 0; i < NumAttachments; ++i)
		{
			VkPipelineColorBlendAttachmentState colorBlendAttachState = {};
			colorBlendAttachState.blendEnable = VK_TRUE;
			colorBlendAttachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachState.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachState.alphaBlendOp = VK_BLEND_OP_ADD;
			blendStates[i] = colorBlendAttachState;
		}

		colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendCreateInfo.attachmentCount = blendStates.size();
		colorBlendCreateInfo.pAttachments = blendStates.data();

		pipelineInfo.pColorBlendState = &colorBlendCreateInfo;

		return *this;
	}

	GraphicsPipelineBuilder &depth(bool enableDepth, VkCompareOp depthCompareOp)
	{
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = enableDepth ? VK_TRUE : VK_FALSE;
		depthStencilInfo.depthWriteEnable = enableDepth ? VK_TRUE : VK_FALSE;
		depthStencilInfo.depthCompareOp = depthCompareOp;
		depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilInfo.stencilTestEnable = VK_FALSE;

		pipelineInfo.pDepthStencilState = &depthStencilInfo;

		return *this;
	}

	GraphicsPipelineBuilder &shaderModules(const ShaderStageModules &shaderModules, bool useTexture)
	{
		mapEntries = 
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

		this->vkUseTexture = useTexture;
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

		shaderStages[0] = createStageInfo(shaderModules.vertex, VK_SHADER_STAGE_VERTEX_BIT, nullptr);
		shaderStages[1] = createStageInfo(shaderModules.fragment, VK_SHADER_STAGE_FRAGMENT_BIT, &specializationCreateInfo);

		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		return *this;
	}

	GraphicsPipelineBuilder &renderPass(VkRenderPass renderPass, uint32_t subpassIndex)
	{
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subpassIndex;

		return *this;
	}

	template<size_t Size>
	GraphicsPipelineBuilder &dynamicState(std::array<VkDynamicState, Size> *states)
	{
		dynaInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynaInfo.pDynamicStates = states->data();
		dynaInfo.dynamicStateCount = states->size();
		pipelineInfo.pDynamicState = &dynaInfo;

		return *this;
	}

	GraphicsPipeline build()
	{
		logger.log("Creating graphics pipeline");

		VkPipeline pipeline = VK_NULL_HANDLE;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Error creating graphics pipeline");
		}
		logger.log("Created new pipeline");

		return GraphicsPipeline(pipeline, layout);
	}
};

}};

#endif /* GRAPHICSPIPELINEBUILDER_H */
