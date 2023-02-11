#pragma once

namespace engine
{
	namespace ecs
	{
		enum class EPathType
		{
			eTranslation,
			eRotation,
			eScale
		};

		struct FAnimationChannel
		{
			EPathType path;
			uint32_t samplerIndex;
			entt::entity node;
		};

		enum class EInterpolationType
		{
			eLinear,
			eStep,
			eCubicSpline
		};

		struct FAnimationSampler
		{
			EInterpolationType interpolation;
			std::vector<float> inputs;
			std::vector<glm::vec4> outputsVec4;
		};

		struct FMeshAnimation
		{
			std::string name;
			std::vector<FAnimationSampler> samplers;
			std::vector<FAnimationChannel> channels;
			float start = std::numeric_limits<float>::max();
			float end = std::numeric_limits<float>::min();
		};

		struct FMeshSkin
		{
			std::string name;
			entt::entity root{ entt::null };
			std::vector<glm::mat4> inverseBindMatrices;
			std::vector<entt::entity> joints;
		};

		struct FSceneComponent
		{
			std::string source;
			bool loaded{ false };
			bool castShadows{ true };
			std::vector<FMeshAnimation> animations;
			std::vector<FMeshSkin> skins;
			std::map<uint32_t, float> timers;
		};

		void to_json(nlohmann::json& json, const FSceneComponent& type);
		void from_json(const nlohmann::json& json, FSceneComponent& type);
	}
}