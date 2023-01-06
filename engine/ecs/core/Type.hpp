#pragma once

#include <bitset>
#include <cstdint>

namespace engine
{
	namespace ecs
	{
		// Source: https://gist.github.com/Lee-R/3839813
		constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
		{
			return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
		}

		constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
		{
			return fnv1a_32(s, count);
		}

		using Entity = uint32_t;
		constexpr const Entity MAX_ENTITIES = 5000;
		using ComponentType = uint8_t;
		constexpr const ComponentType MAX_COMPONENTS = 32;
		using Signature = std::bitset<MAX_COMPONENTS>;

		// Events
		using EventId = uint32_t;
		using ParamId = uint32_t;

		namespace Events::Graphics
		{
			const ParamId ReCreate = "Events::Graphics::ReCreate"_hash;
		}

		namespace Events::Input 
		{
			const ParamId Key = "Events::Input::Key"_hash;
			const ParamId Mouse = "Events::Input::Mouse"_hash;
			const ParamId MouseX = "Events::Input::MouseX"_hash;
			const ParamId MouseY = "Events::Input::MouseY"_hash;
			const ParamId Axis = "Events::Input::Axis"_hash;
		}
	}
}