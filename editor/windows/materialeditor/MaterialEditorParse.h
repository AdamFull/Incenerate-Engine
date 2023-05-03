#pragma once

namespace engine
{
	namespace editor
	{
		enum class EMaterialEditorValueType
		{
			eNone,
			eBool,
			eFloat,
			eVec2,
			eVec3,
			eVec4,
			eMat2,
			eMat3,
			eMat4,
			eDouble,
			eDvec2,
			eDvec3,
			eDvec4,
			eDmat2,
			eDmat3,
			eDmat4,
			eInt,
			eIvec2,
			eIvec3,
			eIvec4,
			eUInt,
			eUvec2,
			eUvec3,
			eUvec4
		};

		enum class EMaterialEditorNodeType
		{
			eConstValue,
			eUniformValue,
			eSampler,
			eMath,
			eGeometric,
			eMatrix,
			eDerivative,
			eOperation
		};

		enum class EMaterialEditorPinKind
		{
			eInput,
			eOutput
		};

		struct FMaterialEditorAttributeCreateInfo
		{
			std::string name;
			size_t defaultType{ 0 };
			std::vector<EMaterialEditorValueType> types;
		};

		void to_json(nlohmann::json& json, const FMaterialEditorAttributeCreateInfo& type);
		void from_json(const nlohmann::json& json, FMaterialEditorAttributeCreateInfo& type);

		struct FMaterialEditorNodeCreateInfo
		{
			std::string name{ "" };
			EMaterialEditorNodeType type;
			std::vector<FMaterialEditorAttributeCreateInfo> inputAttributes;
			std::vector<FMaterialEditorAttributeCreateInfo> outputAttributes;
		};

		void to_json(nlohmann::json& json, const FMaterialEditorNodeCreateInfo& type);
		void from_json(const nlohmann::json& json, FMaterialEditorNodeCreateInfo& type);

		struct FMaterialEditorGroupCreateInfo
		{
			std::string name{ "" };
			std::vector<FMaterialEditorNodeCreateInfo> nodes;
			std::vector<FMaterialEditorGroupCreateInfo> subgroups;
		};

		void to_json(nlohmann::json& json, const FMaterialEditorGroupCreateInfo& type);
		void from_json(const nlohmann::json& json, FMaterialEditorGroupCreateInfo& type);
	}
}