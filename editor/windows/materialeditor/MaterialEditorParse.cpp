#include "MaterialEditorParse.h"

namespace engine
{
	namespace editor
	{
		NLOHMANN_JSON_SERIALIZE_ENUM
		(
			EMaterialEditorValueType,
			{
				{EMaterialEditorValueType::eBool, "bool"},
				{EMaterialEditorValueType::eFloat, "float"},
				{EMaterialEditorValueType::eVec2, "vec2"},
				{EMaterialEditorValueType::eVec3, "vec3"},
				{EMaterialEditorValueType::eVec4, "vec4"},
				{EMaterialEditorValueType::eMat2, "mat2"},
				{EMaterialEditorValueType::eMat3, "mat3"},
				{EMaterialEditorValueType::eMat4, "mat4"},
				{EMaterialEditorValueType::eDouble, "double"},
				{EMaterialEditorValueType::eDvec2, "dvec2"},
				{EMaterialEditorValueType::eDvec3, "dvec3"},
				{EMaterialEditorValueType::eDvec4, "dvec4"},
				{EMaterialEditorValueType::eDmat2, "dmat2"},
				{EMaterialEditorValueType::eDmat3, "dmat3"},
				{EMaterialEditorValueType::eDmat4, "dmat4"},
				{EMaterialEditorValueType::eInt, "int"},
				{EMaterialEditorValueType::eIvec2, "ivec2"},
				{EMaterialEditorValueType::eIvec3, "ivec3"},
				{EMaterialEditorValueType::eIvec4, "ivec4"},
				{EMaterialEditorValueType::eUInt, "uint"},
				{EMaterialEditorValueType::eUvec2, "uvec2"},
				{EMaterialEditorValueType::eUvec3, "uvec3"},
				{EMaterialEditorValueType::eUvec4, "uvec4"}
			}
		);

		NLOHMANN_JSON_SERIALIZE_ENUM
		(
			EMaterialEditorNodeType,
			{
				{EMaterialEditorNodeType::eConstValue, "const_value"},
				{EMaterialEditorNodeType::eUniformValue, "uniform_value"},
				{EMaterialEditorNodeType::eSampler, "sampler"},
				{EMaterialEditorNodeType::eMath, "math"},
				{EMaterialEditorNodeType::eGeometric, "geometric"},
				{EMaterialEditorNodeType::eMatrix, "matrix"},
				{EMaterialEditorNodeType::eDerivative, "derivative"},
				{EMaterialEditorNodeType::eOperation, "operation"}
			}
		);

		void to_json(nlohmann::json& json, const FMaterialEditorAttributeCreateInfo& type) {}
		void from_json(const nlohmann::json& json, FMaterialEditorAttributeCreateInfo& type)
		{
			utl::parse_to("name", json, type.name);
			utl::parse_to("default", json, type.defaultType);
			utl::parse_to("types", json, type.types);
		}

		void to_json(nlohmann::json& json, const FMaterialEditorNodeCreateInfo& type) {}
		void from_json(const nlohmann::json& json, FMaterialEditorNodeCreateInfo& type)
		{
			utl::parse_to("name", json, type.name);
			utl::parse_to("type", json, type.type);
			utl::parse_to("input_attributes", json, type.inputAttributes);
			utl::parse_to("output_attributes", json, type.outputAttributes);
		}

		void to_json(nlohmann::json& json, const FMaterialEditorGroupCreateInfo& type) {}
		void from_json(const nlohmann::json& json, FMaterialEditorGroupCreateInfo& type)
		{
			utl::parse_to("name", json, type.name);
			utl::parse_to("nodes", json, type.nodes);
			utl::parse_to("subgroups", json, type.subgroups);
		}
	}
}