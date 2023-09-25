#include "APIStructures.h"

namespace vk
{
	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		ShaderStageFlagBits,
		{
			{ShaderStageFlagBits::eVertex, "Vertex"},
			{ShaderStageFlagBits::eTessellationControl, "TessellationControl"},
			{ShaderStageFlagBits::eTessellationEvaluation, "TessellationEvaluation"},
			{ShaderStageFlagBits::eGeometry, "Geometry"},
			{ShaderStageFlagBits::eFragment, "Fragment"},
			{ShaderStageFlagBits::eCompute, "Compute"},
			{ShaderStageFlagBits::eAllGraphics, "AllGraphics"},
			{ShaderStageFlagBits::eAll, "All"},
			{ShaderStageFlagBits::eRaygenKHR, "RaygenKHR"},
			{ShaderStageFlagBits::eAnyHitKHR, "AnyHitKHR"},
			{ShaderStageFlagBits::eClosestHitKHR, "ClosestHitKHR"},
			{ShaderStageFlagBits::eMissKHR, "MissKHR"},
			{ShaderStageFlagBits::eIntersectionKHR, "IntersectionKHR"},
			{ShaderStageFlagBits::eCallableKHR, "CallableKHR"},
			{ShaderStageFlagBits::eTaskNV, "TaskNV"},
			{ShaderStageFlagBits::eMeshNV, "MeshNV"},
			{ShaderStageFlagBits::eAnyHitNV, "AnyHitNV"},
			{ShaderStageFlagBits::eCallableNV, "CallableNV"},
			{ShaderStageFlagBits::eClosestHitNV, "ClosestHitNV"},
			{ShaderStageFlagBits::eIntersectionNV, "IntersectionNV"},
			{ShaderStageFlagBits::eMissNV, "MissNV"},
			{ShaderStageFlagBits::eRaygenNV, "RaygenNV"},
		}
	);

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		PrimitiveTopology,
		{
			{PrimitiveTopology::ePointList, "PointList"},
			{PrimitiveTopology::eLineList, "LineList"},
			{PrimitiveTopology::eLineStrip, "LineStrip"},
			{PrimitiveTopology::eTriangleList, "TriangleList"},
			{PrimitiveTopology::eTriangleStrip, "TriangleStrip"},
			{PrimitiveTopology::eTriangleFan, "TriangleFan"},
			{PrimitiveTopology::eLineListWithAdjacency, "LineListWithAdjacency"},
			{PrimitiveTopology::eLineStripWithAdjacency, "LineStripWithAdjacency"},
			{PrimitiveTopology::eTriangleListWithAdjacency, "TriangleListWithAdjacency"},
			{PrimitiveTopology::eTriangleStripWithAdjacency, "TriangleStripWithAdjacency"},
			{PrimitiveTopology::ePatchList, "PatchList"},
		}
	);

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		PipelineBindPoint,
		{
			{PipelineBindPoint::eGraphics, "Graphics"},
			{PipelineBindPoint::eCompute, "Compute"},
			{PipelineBindPoint::eRayTracingKHR, "RayTracingKHR"},
			{PipelineBindPoint::eRayTracingNV, "RayTracingNV"},
		}
	);

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		CullModeFlagBits,
		{
			{CullModeFlagBits::eNone, "None"},
			{CullModeFlagBits::eFront, "Front"},
			{CullModeFlagBits::eBack, "Back"},
			{CullModeFlagBits::eFrontAndBack, "FrontAndBack"}
		}
	);

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		FrontFace,
		{
			{FrontFace::eCounterClockwise, "ccw"},
			{FrontFace::eClockwise, "cw"}
		}
	);

#ifndef __ANDROID__
	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		DynamicState,
		{
			{DynamicState::eViewport, "Viewport"},
			{DynamicState::eScissor, "Scissor"},
			{DynamicState::eLineWidth, "LineWidth"},
			{DynamicState::eDepthBias, "DepthBias"},
			{DynamicState::eBlendConstants, "BlendConstants"},
			{DynamicState::eDepthBounds, "DepthBounds"},
			{DynamicState::eStencilCompareMask, "StencilCompareMask"},
			{DynamicState::eStencilWriteMask, "StencilWriteMask"},
			{DynamicState::eStencilReference, "StencilReference"},
			{DynamicState::eCullMode, "CullMode"},
			{DynamicState::eFrontFace, "FrontFace"},
			{DynamicState::ePrimitiveTopology, "PrimitiveTopology"},
			{DynamicState::eViewportWithCount, "ViewportWithCount"},
			{DynamicState::eScissorWithCount, "ScissorWithCount"},
			{DynamicState::eVertexInputBindingStride, "VertexInputBindingStride"},
			{DynamicState::eDepthTestEnable, "DepthTestEnable"},
			{DynamicState::eDepthWriteEnable, "DepthWriteEnable"},
			{DynamicState::eDepthCompareOp, "DepthCompareOp"},
			{DynamicState::eDepthBoundsTestEnable, "DepthBoundsTestEnable"},
			{DynamicState::eStencilTestEnable, "StencilTestEnable"},
			{DynamicState::eStencilOp, "StencilOp"},
			{DynamicState::eRasterizerDiscardEnable, "RasterizerDiscardEnable"},
			{DynamicState::eDepthBiasEnable, "DepthBiasEnable"},
			{DynamicState::ePrimitiveRestartEnable, "PrimitiveRestartEnable"},
			{DynamicState::eViewportWScalingNV, "ViewportWScalingNV"},
			{DynamicState::eDiscardRectangleEXT, "DiscardRectangleEXT"},
			{DynamicState::eSampleLocationsEXT, "SampleLocationsEXT"},
			{DynamicState::eRayTracingPipelineStackSizeKHR, "RayTracingPipelineStackSizeKHR"},
			{DynamicState::eViewportShadingRatePaletteNV, "ViewportShadingRatePaletteNV"},
			{DynamicState::eViewportCoarseSampleOrderNV, "ViewportCoarseSampleOrderNV"},
			{DynamicState::eExclusiveScissorNV, "ExclusiveScissorNV"},
			{DynamicState::eFragmentShadingRateKHR, "FragmentShadingRateKHR"},
			{DynamicState::eLineStippleEXT, "LineStippleEXT"},
			{DynamicState::eVertexInputEXT, "VertexInputEXT"},
			{DynamicState::ePatchControlPointsEXT, "PatchControlPointsEXT"},
			{DynamicState::eLogicOpEXT, "LogicOpEXT"},
			{DynamicState::eColorWriteEnableEXT, "ColorWriteEnableEXT"},
			{DynamicState::eCullModeEXT, "CullModeEXT"},
			{DynamicState::eDepthBiasEnableEXT, "DepthBiasEnableEXT"},
			{DynamicState::eDepthBoundsTestEnableEXT, "DepthBoundsTestEnableEXT"},
			{DynamicState::eDepthCompareOpEXT, "DepthCompareOpEXT"},
			{DynamicState::eDepthTestEnableEXT, "DepthTestEnableEXT"},
			{DynamicState::eDepthWriteEnableEXT, "DepthWriteEnableEXT"},
			{DynamicState::eFrontFaceEXT, "FrontFaceEXT"},
			{DynamicState::ePrimitiveRestartEnableEXT, "PrimitiveRestartEnableEXT"},
			{DynamicState::ePrimitiveTopologyEXT, "PrimitiveTopologyEXT"},
			{DynamicState::eRasterizerDiscardEnableEXT, "RasterizerDiscardEnableEXT"},
			{DynamicState::eScissorWithCountEXT, "ScissorWithCountEXT"},
			{DynamicState::eStencilOpEXT, "StencilOpEXT"},
			{DynamicState::eStencilTestEnableEXT, "StencilTestEnableEXT"},
			{DynamicState::eVertexInputBindingStrideEXT, "VertexInputBindingStrideEXT"},
			{DynamicState::eViewportWithCountEXT, "ViewportWithCountEXT"},
		}
	);
#else
	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		DynamicState,
		{
			{DynamicState::eViewport, "Viewport"},
			{DynamicState::eScissor, "Scissor"},
			{DynamicState::eLineWidth, "LineWidth"},
			{DynamicState::eDepthBias, "DepthBias"},
			{DynamicState::eBlendConstants, "BlendConstants"},
			{DynamicState::eDepthBounds, "DepthBounds"},
			{DynamicState::eStencilCompareMask, "StencilCompareMask"},
			{DynamicState::eStencilWriteMask, "StencilWriteMask"},
			{DynamicState::eStencilReference, "StencilReference"},
			{DynamicState::eScissorWithCount, "ScissorWithCount"},
			{DynamicState::eVertexInputBindingStride, "VertexInputBindingStride"},
			{DynamicState::eDepthTestEnable, "DepthTestEnable"},
			{DynamicState::eDepthWriteEnable, "DepthWriteEnable"},
			{DynamicState::eDepthCompareOp, "DepthCompareOp"},
			{DynamicState::eDepthBoundsTestEnable, "DepthBoundsTestEnable"},
			{DynamicState::eStencilTestEnable, "StencilTestEnable"},
			{DynamicState::eStencilOp, "StencilOp"},
			{DynamicState::eRasterizerDiscardEnable, "RasterizerDiscardEnable"},
			{DynamicState::eDepthBiasEnable, "DepthBiasEnable"},
			{DynamicState::ePrimitiveRestartEnable, "PrimitiveRestartEnable"},
			{DynamicState::eViewportWScalingNV, "ViewportWScalingNV"},
			{DynamicState::eDiscardRectangleEXT, "DiscardRectangleEXT"},
			{DynamicState::eSampleLocationsEXT, "SampleLocationsEXT"},
			{DynamicState::eRayTracingPipelineStackSizeKHR, "RayTracingPipelineStackSizeKHR"},
			{DynamicState::eViewportShadingRatePaletteNV, "ViewportShadingRatePaletteNV"},
			{DynamicState::eViewportCoarseSampleOrderNV, "ViewportCoarseSampleOrderNV"},
			{DynamicState::eExclusiveScissorNV, "ExclusiveScissorNV"},
			{DynamicState::eFragmentShadingRateKHR, "FragmentShadingRateKHR"},
			{DynamicState::eLineStippleEXT, "LineStippleEXT"},
			{DynamicState::eVertexInputEXT, "VertexInputEXT"},
			{DynamicState::ePatchControlPointsEXT, "PatchControlPointsEXT"},
			{DynamicState::eLogicOpEXT, "LogicOpEXT"},
			{DynamicState::eColorWriteEnableEXT, "ColorWriteEnableEXT"},
			{DynamicState::eCullModeEXT, "CullMode"},
			{DynamicState::eDepthBiasEnableEXT, "DepthBiasEnableEXT"},
			{DynamicState::eDepthBoundsTestEnableEXT, "DepthBoundsTestEnableEXT"},
			{DynamicState::eDepthCompareOpEXT, "DepthCompareOpEXT"},
			{DynamicState::eDepthTestEnableEXT, "DepthTestEnableEXT"},
			{DynamicState::eDepthWriteEnableEXT, "DepthWriteEnableEXT"},
			{DynamicState::eFrontFaceEXT, "FrontFace"},
			{DynamicState::ePrimitiveRestartEnableEXT, "PrimitiveRestartEnableEXT"},
			{DynamicState::ePrimitiveTopologyEXT, "PrimitiveTopology"},
			{DynamicState::eRasterizerDiscardEnableEXT, "RasterizerDiscardEnableEXT"},
			{DynamicState::eScissorWithCountEXT, "ScissorWithCountEXT"},
			{DynamicState::eStencilOpEXT, "StencilOpEXT"},
			{DynamicState::eStencilTestEnableEXT, "StencilTestEnableEXT"},
			{DynamicState::eVertexInputBindingStrideEXT, "VertexInputBindingStrideEXT"},
			{DynamicState::eViewportWithCountEXT, "ViewportWithCount"},
		}
	);
#endif
}

namespace engine
{
	namespace graphics
	{
		NLOHMANN_JSON_SERIALIZE_ENUM
		(
			EVertexType,
			{
				{EVertexType::eNone, nullptr},
				{EVertexType::eDefault, "default"},
				{EVertexType::eSmall, "small"}
			}
		);
	}
}