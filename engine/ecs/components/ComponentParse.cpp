#include "AudioComponent.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "SkyboxComponent.h"
#include "ScriptComponent.h"

#include "parse/glmparse.h"

namespace engine
{
	namespace ecs
	{
		void to_json(nlohmann::json& json, const FAudioComponent& type)
		{
			json = nlohmann::json
			{
				{"source", type.source},
				{"gain", type.gain},
				{"pitch", type.pitch},
				{"velocity", type.velocity},
				{"loop", type.loop}
			};
		}

		void from_json(const nlohmann::json& json, FAudioComponent& type)
		{
			utl::parse_to("source", json, type.source);
			utl::parse_to("gain", json, type.gain);
			utl::parse_to("pitch", json, type.pitch);
			utl::parse_to("velocity", json, type.velocity);
			utl::parse_to("loop", json, type.loop);
		}


		void to_json(nlohmann::json& json, const FCameraComponent& type)
		{
			json = nlohmann::json
			{
				{"fov", type.fieldOfView},
				{"near", type.nearPlane},
				{"far", type.farPlane},
				{"active", type.active}
			};
		}

		void from_json(const nlohmann::json& json, FCameraComponent& type)
		{
			utl::parse_to("fov", json, type.fieldOfView);
			utl::parse_to("near", json, type.nearPlane);
			utl::parse_to("far", json, type.farPlane);
			utl::parse_to("active", json, type.active);
		}


		void to_json(nlohmann::json& json, const FMeshComponent& type)
		{
			json = nlohmann::json
			{
				{"source", type.source},
				{"isSkybox", type.isSkybox},
			};
		}

		void from_json(const nlohmann::json& json, FMeshComponent& type)
		{
			utl::parse_to("source", json, type.source);
			utl::parse_to("isSkybox", json, type.isSkybox);
		}


		void to_json(nlohmann::json& json, const FTransformComponent& type)
		{
			json = nlohmann::json
			{
				{"pos", type.position},
				{"rot", type.rotation},
				{"scale", type.scale}
			};
		}
		
		void from_json(const nlohmann::json& json, FTransformComponent& type)
		{
			utl::parse_to("pos", json, type.position);
			utl::parse_to("rot", json, type.rotation);
			utl::parse_to("scale", json, type.scale);
		}


		void to_json(nlohmann::json& json, const FDirectionalLightComponent& type)
		{
			json = nlohmann::json
			{
				{"color", type.color},
				{"intencity", type.intencity},
				{"castShadows", type.castShadows}
			};
		}

		void from_json(const nlohmann::json& json, FDirectionalLightComponent& type)
		{
			utl::parse_to("color", json, type.color);
			utl::parse_to("intencity", json, type.intencity);
			utl::parse_to("castShadows", json, type.castShadows);
		}


		void to_json(nlohmann::json& json, const FPointLightComponent& type)
		{
			json = nlohmann::json
			{
				{"color", type.color},
				{"intencity", type.intencity},
				{"radius", type.radius},
				{"castShadows", type.castShadows}
			};
		}

		void from_json(const nlohmann::json& json, FPointLightComponent& type)
		{
			utl::parse_to("color", json, type.color);
			utl::parse_to("intencity", json, type.intencity);
			utl::parse_to("radius", json, type.radius);
			utl::parse_to("castShadows", json, type.castShadows);
		}


		void to_json(nlohmann::json& json, const FSpotLightComponent& type)
		{
			json = nlohmann::json
			{
				{"color", type.color},
				{"intencity", type.intencity},
				{"innerAngle", type.innerAngle},
				{"outerAngle", type.outerAngle},
				{"toTarget", type.toTarget},
				{"castShadows", type.castShadows}
			};
		}

		void from_json(const nlohmann::json& json, FSpotLightComponent& type)
		{
			utl::parse_to("color", json, type.color);
			utl::parse_to("intencity", json, type.intencity);
			utl::parse_to("innerAngle", json, type.innerAngle);
			utl::parse_to("outerAngle", json, type.outerAngle);
			utl::parse_to("toTarget", json, type.toTarget);
			utl::parse_to("castShadows", json, type.castShadows);
		}


		void to_json(nlohmann::json& json, const FSpriteComponent& type)
		{
			json = nlohmann::json
			{
				{"source", type.source}
			};
		}

		void from_json(const nlohmann::json& json, FSpriteComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}


		void to_json(nlohmann::json& json, const FScriptComponent& type)
		{
			json = nlohmann::json
			{
				{"source", type.source}
			};
		}

		void from_json(const nlohmann::json& json, FScriptComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}


		void to_json(nlohmann::json& json, const FSkyboxComponent& type)
		{
			json = nlohmann::json
			{
				{"source", type.source}
			};
		}

		void from_json(const nlohmann::json& json, FSkyboxComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}
	}
}