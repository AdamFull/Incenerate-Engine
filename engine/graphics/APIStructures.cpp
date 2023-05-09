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
				{EVertexType::eDefault, "default"},
				{EVertexType::eSmall, "small"}
			}
		);

		void to_json(nlohmann::json& json, const FProgramCreateInfo& type)
		{
			json = nlohmann::json
			{
				{"stage", type.srStage},
				{"vertex", type.vertexType},
				{"vertex_free", type.srStage},
				{"bind_point", type.srStage},
				{"topology", type.srStage},
				{"cull_mode", type.srStage},
				{"front_face", type.srStage},
				{"depth_test", type.srStage},
				{"dynamic_states", type.srStage},
				{"tesselation", type.tesselation},
				{"uses_bindless_textures", type.usesBindlessTextures},
				{"stages", type.srStage},
				{"defines", type.srStage}
			};
		}

		void from_json(const nlohmann::json& json, FProgramCreateInfo& type)
		{
			utl::parse_to("stage", json, type.srStage);
			utl::parse_to("vertex", json, type.vertexType);
			utl::parse_to("vertex_free", json, type.vertexfree);
			utl::parse_to("bind_point", json, type.bindPoint);
			utl::parse_to("topology", json, type.topology);
			utl::parse_to("cull_mode", json, type.cullMode);
			utl::parse_to("front_face", json, type.frontFace);
			utl::parse_to("depth_test", json, type.depthTest);
			utl::parse_to("dynamic_states", json, type.dynamicStates);
			utl::parse_to("tesselation", json, type.tesselation);
			utl::parse_to("uses_bindless_textures", json, type.usesBindlessTextures);
			utl::parse_to("stages", json, type.stages);
			utl::parse_to("defines", json, type.defines);
		}


		void to_json(nlohmann::json& json, const FCachedShader& type)
		{
			json = nlohmann::json
			{
				{"stage", type.shaderStage},
				{"code", type.shaderCode},
				{"hash", type.hash}
			};
		}

		void from_json(const nlohmann::json& json, FCachedShader& type)
		{
			utl::parse_to("stage", json, type.shaderStage);
			utl::parse_to("code", json, type.shaderCode);
			utl::parse_to("hash", json, type.hash);
		}
	}
}