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
			{ShaderStageFlagBits::eSubpassShadingHUAWEI, "SubpassShadingHUAWEI"},
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
		PipelineBindPoint,
		{
			{PipelineBindPoint::eGraphics, "Graphics"},
			{PipelineBindPoint::eCompute, "Compute"},
			{PipelineBindPoint::eRayTracingKHR, "RayTracingKHR"},
			{PipelineBindPoint::eSubpassShadingHUAWEI, "SubpassShadingHUAWEI"},
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
}

namespace engine
{
	namespace graphics
	{
		void to_json(nlohmann::json& json, const FProgramCreateInfo& type)
		{
			json = nlohmann::json
			{
				{"bind_point", type.bindPoint},
				{"cull_mode", type.cullMode},
				{"front_face", type.frontFace},
				{"depth_test", type.depthTest},
				{"dynamic_states", type.dynamicStates},
				{"tesselation", type.tesselation},
				{"stages", type.stages},
				{"defines", type.defines},
			};
		}

		void from_json(const nlohmann::json& json, FProgramCreateInfo& type)
		{
			utl::parse_to("bind_point", json, type.bindPoint);
			utl::parse_to("cull_mode", json, type.cullMode);
			utl::parse_to("front_face", json, type.frontFace);
			utl::parse_to("depth_test", json, type.depthTest);
			utl::parse_to("dynamic_states", json, type.dynamicStates);
			utl::parse_to("tesselation", json, type.tesselation);
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