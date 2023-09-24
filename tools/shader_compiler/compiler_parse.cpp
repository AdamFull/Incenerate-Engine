#include "compiler_parse.h"

EShaderType parse_shader_type(const std::string& shader_type_str)
{
	if (shader_type_str == "vertex")
		return EShaderType::eVertex;
	else if (shader_type_str == "control")
		return EShaderType::eControl;
	else if (shader_type_str == "evaluation")
		return EShaderType::eEvaluation;
	else if (shader_type_str == "geometry")
		return EShaderType::eGeometry;
	else if (shader_type_str == "fragment")
		return EShaderType::eFragment;
	else if (shader_type_str == "compute")
		return EShaderType::eCompute;
	else if (shader_type_str == "raygen")
		return EShaderType::eRayGen;
	else if (shader_type_str == "intersection")
		return EShaderType::eIntersection;
	else if (shader_type_str == "anyhit")
		return EShaderType::eAnyHit;
	else if (shader_type_str == "closesthit")
		return EShaderType::eClosestHit;
	else if (shader_type_str == "miss")
		return EShaderType::eMiss;
	else if (shader_type_str == "callable")
		return EShaderType::eCallable;
	else if (shader_type_str == "task")
		return EShaderType::eTask;
	else if (shader_type_str == "mesh")
		return EShaderType::eMesh;

	return EShaderType::eUnknown;
}

void deserialize(const sjson::sjson_object& sjson, FShaderObject& value)
{
	sjson.try_parse_to("version", value.version);

	sjson.try_parse_to("enable_extensions", value.enable_extensions);
	sjson.try_parse_to("require_extensions", value.require_extensions);
	sjson.try_parse_to("definitions", value.definitions);
	sjson.try_parse_to("includes", value.includes);

	sjson.try_parse_to("code", value.code, true);
}


void deserialize(const sjson::sjson_object& sjson, FShaderSource& value)
{
	std::string type{};
	sjson.try_parse_to("type", type, true);
	value.type = parse_shader_type(type);
	sjson.try_parse_to("common", value.common);
	sjson.try_parse_to("shader_name", value.shader_name, true);
}


void deserialize(const sjson::sjson_object& sjson, FShaderDescription& value)
{
	sjson.try_parse_to("name", value.name, true);
	sjson.try_parse_to("sources", value.sources, true);
}


void deserialize(const sjson::sjson_object& sjson, FShaderPermutation& value)
{
	sjson.try_parse_to("definitions", value.definitions);
}

void deserialize(const sjson::sjson_object& sjson, FShaderPermutationGenerator& value)
{
	sjson.try_parse_to("definitions", value.definitions);
}

void deserialize(const sjson::sjson_object& sjson, FShaderCompile& value)
{
	sjson.try_parse_to("shader_name", value.name, true);
	sjson.try_parse_to("permutations", value.permutations);
	sjson.try_parse_to("permutation_generator", value.permutation_gen);
}


void deserialize(const sjson::sjson_object& sjson, FShaderLibrary& value)
{
	sjson.try_parse_to("name", value.name, true);

	std::vector<FShaderDescription> descriptions;
	sjson.try_parse_to("shaders", descriptions);

	for (auto& description : descriptions)
		value.shaders.emplace(description.name, description);

	sjson.try_parse_to("compile", value.compiles);
}