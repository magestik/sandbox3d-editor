#pragma once

namespace RHI
{

#if HAVE_OPENGL
typedef GLuint ShaderModule;
#endif // HAVE_OPENGL

#if HAVE_VULKAN
typedef VkShaderModule ShaderModule;
#endif // HAVE_VULKAN

struct PipelineInputAssemblyStateCreateInfo
{
	PipelineInputAssemblyStateCreateInfo(void)
	{
		topology = PRIMITIVE_POINT_LIST;
		enableRestartPrimitive = false;
	}

	PrimitiveType topology;
	bool enableRestartPrimitive;
};

struct PipelineRasterizationStateCreateInfo
{
	PipelineRasterizationStateCreateInfo(void)
	{
		enableDepthClamp = false;
		enableRasterizerDiscard = false;
		polygonMode = POLYGON_MODE_FILL;
		cullMode = CULL_MODE_NONE;
		frontFace = FRONT_FACE_COUNTER_CLOCKWISE;
		enableDepthBias = false;
		depthBiasConstantFactor = 0.0f;
		depthBiasClamp = 0.0f;
		depthBiasSlopeFactor = 0.0f;
		lineWidth = 1.0f;
	}

	bool			enableDepthClamp;
	bool			enableRasterizerDiscard;
	PolygonMode		polygonMode;
	CullMode		cullMode;
	FrontFace		frontFace;
	bool			enableDepthBias;
	float			depthBiasConstantFactor;
	float			depthBiasClamp;
	float			depthBiasSlopeFactor;
	float			lineWidth;
};

struct PipelineDepthStencilStateCreateInfo
{
	PipelineDepthStencilStateCreateInfo(void)
	{
		enableDepth = false;
		enableStencil = false;
	}

	struct DepthState
	{
		DepthState(void)
		{
			enableWrite = true;
			compareOp = COMPARE_OP_LESS;
		}

		bool		enableWrite;
		CompareOp	compareOp;
	};

	struct StencilState
	{
		StencilState(void)
		{
			failOp = STENCIL_OP_ZERO;
			passOp = STENCIL_OP_ZERO;
			depthFailOp = STENCIL_OP_ZERO;
			compareOp = COMPARE_OP_ALWAYS;
			compareMask = UINT32_MAX;
			writeMask = 0;
			reference = 0;
		}

		StencilOp		failOp;
		StencilOp		passOp;
		StencilOp		depthFailOp;
		CompareOp		compareOp;
		unsigned int	compareMask;
		unsigned		writeMask;
		unsigned		reference;
	};

	bool			enableDepth;
	DepthState		depthState;

	bool			enableStencil;
	StencilState	frontStencilState;
	StencilState	backStencilState;
};

struct PipelineBlendStateCreateInfo
{
	PipelineBlendStateCreateInfo(void)
	{
		enable = false;

		srcColorFactor	= srcAlphaFactor = BLEND_FACTOR_ONE;
		dstColorFactor	= dstAlphaFactor = BLEND_FACTOR_ZERO;
		colorEquation	= alphaEquation  = BLEND_OP_ADD;

		writeMask = COLOR_MASK_R | COLOR_MASK_G | COLOR_MASK_B | COLOR_MASK_A;
	}

	bool enable;

	BlendFactor srcColorFactor;
	BlendFactor dstColorFactor;
	BlendOp colorEquation;

	BlendFactor srcAlphaFactor;
	BlendFactor dstAlphaFactor;
	BlendOp alphaEquation;

	unsigned int writeMask;
};

struct PipelineShaderStageCreateInfo
{
	PipelineShaderStageCreateInfo(void)
	{
		stage = ShaderStageFlag(0);
		module = 0;
	}

	ShaderStageFlag stage;
	ShaderModule module;
};

struct SamplerCreateInfo
{
	SamplerCreateInfo(void)
	{
		magFilter = FILTER_NEAREST;
		minFilter = FILTER_NEAREST;
		//mipmapMode = ;
		addressModeU = SAMPLER_ADDRESS_MODE_REPEAT;
		addressModeV = SAMPLER_ADDRESS_MODE_REPEAT;
		addressModeW = SAMPLER_ADDRESS_MODE_REPEAT;
		mipLodBias = 0.0f;
		anisotropyEnable = false;
		maxAnisotropy = 0.0f;
		compareEnable = false;
		compareOp = COMPARE_OP_NEVER;
		minLod = 0.0f;
		maxLod = 0.0f;
		//borderColor = ;
		unnormalizedCoordinates = false;
	}

	Filter magFilter;
	Filter minFilter;
	//SamplerMipmapMode mipmapMode;
	SamplerAddressMode addressModeU;
	SamplerAddressMode addressModeV;
	SamplerAddressMode addressModeW;
	float mipLodBias;
	bool anisotropyEnable;
	float maxAnisotropy;
	bool compareEnable;
	CompareOp compareOp;
	float minLod;
	float maxLod;
	//BorderColor borderColor;
	bool unnormalizedCoordinates;
};

}
