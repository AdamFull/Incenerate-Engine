#include "AnimationSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::ecs;

glm::vec3 linear(const std::vector<glm::vec4>& positions, uint32_t index, const float interp, bool quat = false)
{
	auto& prev = positions[index];
	auto& next = positions[index + 1];

	if (quat)
	{
		glm::quat prevq;
		prevq.x = prev.x;
		prevq.y = prev.y;
		prevq.z = prev.z;
		prevq.w = prev.w;

		glm::quat nextq;
		nextq.x = next.x;
		nextq.y = next.y;
		nextq.z = next.z;
		nextq.w = next.w;

		return glm::eulerAngles(glm::normalize(glm::slerp(prevq, nextq, interp)));
	}
	else 
		return glm::mix(prev, next, interp);
}

glm::vec3 step(const std::vector<glm::vec4>& positions, uint32_t index, const float interp, bool quat = false)
{
	auto& pos = positions[index + 1];

	if (quat)
	{
		glm::quat q;
		q.x = pos.x;
		q.y = pos.y;
		q.z = pos.z;
		q.w = pos.w;

		return glm::vec3(glm::eulerAngles(glm::normalize(q)));
	}
	else
		return pos;
}

glm::vec4 cubicspline(const std::vector<glm::vec4>& positions, uint32_t index, const float interp)
{
	auto t = interp;
	auto t2 = t * t;
	auto t3 = t2 * t;

	auto prevIndex = index * 3;
	auto nextIndex = (index + 1) * 3;

	auto& prev = positions[prevIndex + 1];
	auto prevTangent = positions[prevIndex + 2];
	auto& next = positions[nextIndex + 1];
	auto nextTangent = positions[nextIndex];

	return (2 * t3 - 3 * t2 + 1) * prev + (t3 - 2 * t2 + t) * prevTangent + (-2 * t3 + 3 * t2) * next + (t3 - t2) * nextTangent;
}

glm::vec3 v_interpolate(EInterpolationType interpolation, const std::vector<glm::vec4>& positions, uint32_t index, const float interp)
{
	if (interpolation == EInterpolationType::eLinear)
		return linear(positions, index, interp);
	else if (interpolation == EInterpolationType::eStep)
		return step(positions, index, interp);
	else if (interpolation == EInterpolationType::eCubicSpline)
		return cubicspline(positions, index, interp);
}

glm::vec3 q_interpolate(EInterpolationType interpolation, const std::vector<glm::vec4>& positions, uint32_t index, const float interp)
{
	if (interpolation == EInterpolationType::eLinear)
		return linear(positions, index, interp, true);
	else if (interpolation == EInterpolationType::eStep)
		return step(positions, index, interp, true);
	else if (interpolation == EInterpolationType::eCubicSpline)
	{
		auto tmp = cubicspline(positions, index, interp);

		glm::quat q;
		q.x = tmp.x;
		q.y = tmp.y;
		q.z = tmp.z;
		q.w = tmp.w;
		return glm::eulerAngles(glm::normalize(q));
	}
}

void playAnimation(entt::registry* registry, const FMeshAnimation& animation, float time)
{
	for (auto& channel : animation.channels)
	{
		auto& sampler = animation.samplers[channel.samplerIndex];

		for (auto i = 0; i < sampler.inputs.size() - 1; i++)
		{
			if ((time >= sampler.inputs[i]) && (time <= sampler.inputs[i + 1]))
			{
				float u = std::max(0.0f, time - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);

				if (u <= 1.0f)
				{
					auto& transform = registry->get<FTransformComponent>(channel.node);
					switch (channel.path)
					{
					case EPathType::eTranslation: {
						transform.position = v_interpolate(sampler.interpolation, sampler.outputsVec4, i, u);
					} break;
					case EPathType::eScale: {
						transform.scale = v_interpolate(sampler.interpolation, sampler.outputsVec4, i, u);
					} break;
					case EPathType::eRotation: {
						transform.rotation = q_interpolate(sampler.interpolation, sampler.outputsVec4, i, u);
					} break;
					}
				}
			}
		}
	}
}

void CAnimationSystem::__create()
{

}

void CAnimationSystem::__update(float fDt)
{
	if (!EGEngine->isEditorEditing());
	{
		auto view = registry->view<FSceneComponent>();
		for (auto [entity, scene] : view.each())
		{
			uint32_t index{ 0 };
			for (auto& animation : scene.animations)
			{
				scene.timers[index] += fDt;
				if (scene.timers[index] > animation.end)
					scene.timers[index] -= animation.end;

				playAnimation(registry, animation, scene.timers[index]);

				index++;
			}
		}
	}
}