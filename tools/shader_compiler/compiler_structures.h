#pragma once

#include <string>
#include <vector>
#include <unordered_map>

enum class ETargetAPIType
{
	eVK10,
	eVK11,
	eVK12,
	eVK13,
	eGL
};

enum class ETargetBuildType
{
	eRelease,
	eDebug
};

enum class EShaderType
{
	eUnknown,
	eVertex,
	eControl,
	eEvaluation,
	eGeometry,
	eFragment,
	eCompute,
	eRayGen,
	eIntersection,
	eAnyHit,
	eClosestHit,
	eMiss,
	eCallable,
	eTask,
	eMesh
};

struct FShaderObject
{
	uint32_t version;

	std::vector<std::string> enable_extensions;
	std::vector<std::string> require_extensions;
	std::vector<std::string> definitions;
	std::vector<std::string> includes;

	std::string code;
};

struct FShaderSource
{
	EShaderType type{ EShaderType::eUnknown };
	bool common{ false };
	std::string shader_name;
};

struct FShaderDescription
{
	std::string name;
	std::vector<FShaderSource> sources;
};

struct FShaderPermutation
{
	std::vector<std::string> definitions;
};

struct FShaderPermutationGenerator
{
	std::vector<std::string> definitions;
};

struct FShaderCompile
{
	std::string name;
	FShaderPermutationGenerator permutation_gen;
	std::vector<FShaderPermutation> permutations;
};

struct FShaderLibrary
{
	std::string name;
	std::unordered_map<std::string, FShaderDescription> shaders;
	std::vector<FShaderCompile> compiles;
};