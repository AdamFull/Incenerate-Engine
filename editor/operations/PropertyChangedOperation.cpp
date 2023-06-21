#include "PropertyChangedOperation.h"

using namespace engine::editor;

namespace property_type
{
	constexpr const uint32_t float_h = utl::type_hash<float>();
	constexpr const uint32_t int_h = utl::type_hash<int>();
	constexpr const uint32_t bool_h = utl::type_hash<bool>();
	constexpr const uint32_t vec2_h = utl::type_hash<glm::vec2>();
	constexpr const uint32_t vec3_h = utl::type_hash<glm::vec3>();
	constexpr const uint32_t vec4_h = utl::type_hash<glm::vec4>();
	constexpr const uint32_t quat_h = utl::type_hash<glm::quat>();
	constexpr const uint32_t string_h = utl::type_hash<std::string>();
}

CPropertyChangedOperation::CPropertyChangedOperation(std::any vold, std::any vnew, size_t property_type)
{
	oldValue = vold;
	newValue = vnew;
	prop_type = property_type;
}

void CPropertyChangedOperation::undo()
{
	switch (prop_type)
	{
	case property_type::float_h:
	{
		auto vold = std::any_cast<float>(oldValue);
		auto* vnew = std::any_cast<float*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	case property_type::int_h:
	{
		auto vold = std::any_cast<int>(oldValue);
		auto* vnew = std::any_cast<int*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	case property_type::bool_h:
	{
		auto vold = std::any_cast<bool>(oldValue);
		auto* vnew = std::any_cast<bool*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	case property_type::vec2_h:
	{
		auto vold = std::any_cast<glm::vec2>(oldValue);
		auto* vnew = std::any_cast<glm::vec2*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	case property_type::vec3_h:
	{
		auto vold = std::any_cast<glm::vec3>(oldValue);
		auto* vnew = std::any_cast<glm::vec3*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	case property_type::vec4_h:
	{
		auto vold = std::any_cast<glm::vec4>(oldValue);
		auto* vnew = std::any_cast<glm::vec4*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	case property_type::quat_h:
	{
		auto vold = std::any_cast<glm::quat>(oldValue);
		auto* vnew = std::any_cast<glm::quat*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;
	
	case property_type::string_h:
	{
		auto vold = std::any_cast<std::string>(oldValue);
		auto* vnew = std::any_cast<std::string*>(newValue);
		newValueCpy = *vnew;
		*vnew = vold;
	} break;

	default:
		break;
	}
}

void CPropertyChangedOperation::redo()
{
	switch (prop_type)
	{
	case property_type::float_h:
	{
		auto vnewcpy = std::any_cast<float>(newValueCpy);
		auto* vnew = std::any_cast<float*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::int_h:
	{
		auto vnewcpy = std::any_cast<int>(newValueCpy);
		auto* vnew = std::any_cast<int*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::bool_h:
	{
		auto vnewcpy = std::any_cast<bool>(newValueCpy);
		auto* vnew = std::any_cast<bool*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::vec2_h:
	{
		auto vnewcpy = std::any_cast<glm::vec2>(newValueCpy);
		auto* vnew = std::any_cast<glm::vec2*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::vec3_h:
	{
		auto vnewcpy = std::any_cast<glm::vec3>(newValueCpy);
		auto* vnew = std::any_cast<glm::vec3*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::vec4_h:
	{
		auto vnewcpy = std::any_cast<glm::vec4>(newValueCpy);
		auto* vnew = std::any_cast<glm::vec4*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::quat_h:
	{
		auto vnewcpy = std::any_cast<glm::quat>(newValueCpy);
		auto* vnew = std::any_cast<glm::quat*>(newValue);
		*vnew = vnewcpy;
	} break;

	case property_type::string_h:
	{
		auto vnewcpy = std::any_cast<std::string>(newValueCpy);
		auto* vnew = std::any_cast<std::string*>(newValue);
		*vnew = vnewcpy;
	} break;

	default:
		break;
	}
}