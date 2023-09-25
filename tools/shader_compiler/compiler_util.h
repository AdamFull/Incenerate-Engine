#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <udetail.hpp>
#include "compiler_structures.h"

constexpr const uint64_t MOD = static_cast<uint64_t>(-1);

inline std::string make_shader_hashname(const std::vector<std::string>& tokens)
{
	uint64_t result_hash{ 0ull };
	for (auto& token : tokens)
		result_hash = (result_hash + utl::fnv1a_64_hash(token.c_str())) % MOD;

	std::stringstream ss;
	ss << std::hex << result_hash;
	return ss.str();
}

inline std::string get_shader_type(EShaderType type)
{
	switch (type)
	{
	case EShaderType::eUnknown: return ""; break;
	case EShaderType::eVertex: return "vertex"; break;
	case EShaderType::eControl: return "control"; break;
	case EShaderType::eEvaluation: return "evaluation"; break;
	case EShaderType::eGeometry: return "geometry"; break;
	case EShaderType::eFragment: return "fragment"; break;
	case EShaderType::eCompute: return "compute"; break;
	case EShaderType::eRayGen: return "raygen"; break;
	case EShaderType::eIntersection: return "intersection"; break;
	case EShaderType::eAnyHit: return "anyhit"; break;
	case EShaderType::eClosestHit: return "closesthit"; break;
	case EShaderType::eMiss: return "miss"; break;
	case EShaderType::eCallable: return "callable"; break;
	case EShaderType::eTask: return "task"; break;
	case EShaderType::eMesh: return "mesh"; break;
	}

	return "";
}

inline std::string make_module_unique_name(const std::string& shader_name, const std::vector<std::string>& definitions)
{
	std::vector<std::string> shader_name_tokens{ shader_name };
	shader_name_tokens.insert(shader_name_tokens.end(), definitions.begin(), definitions.end());
	return make_shader_hashname(shader_name_tokens);
}

inline std::string make_shader_unique_name(size_t value)
{
	std::stringstream stream;
	stream << std::hex << value;
	return stream.str();
}