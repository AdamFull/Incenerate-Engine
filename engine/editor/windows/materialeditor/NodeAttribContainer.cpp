#include "NodeAttribContainer.h"

#include <imgui/imgui.h>
#include <imgui/imnodes.h>
#include "editor/CustomControls.h"

using namespace engine::editor;

int32_t CNodeAttribContainer::createNode(const FMaterialEditorNodeCreateInfo& node_ci)
{
	auto node_id = next_id;
	nextId();

	FMaterialEditorNode node;
	node.name = node_ci.name;
	node.type = node_ci.type;
	
	for (auto& attrib : node_ci.inputAttributes)
	{
		FMaterialEditorAttribute attribute;
		attribute.name = attrib.name;
		attribute.types = attrib.types;
		attribute.defautlType = attrib.defaultType;
		attribute.selectedType = attrib.types[attrib.defaultType];
		attribute.parent_id = node_id;

		mAttributes.emplace(next_id, attribute);
		node.vInputAttributes.emplace_back(next_id);
		nextId();
	}

	for (auto& attrib : node_ci.outputAttributes)
	{
		FMaterialEditorAttribute attribute;
		attribute.name = attrib.name;
		attribute.types = attrib.types;
		attribute.defautlType = attrib.defaultType;
		attribute.selectedType = attrib.types[attrib.defaultType];
		attribute.parent_id = node_id;

		mAttributes.emplace(next_id, attribute);
		node.vOutputAttributes.emplace_back(next_id);
		nextId();
	}
	
	mNodes.emplace(node_id, node);
	return node_id;
}

void CNodeAttribContainer::removeNode(int32_t id)
{
	auto& node = mNodes[id];
	
	for (auto& aid : node.vInputAttributes)
	{
		qFreedIds.push(aid);
		mAttributes.erase(aid);
	}

	for (auto& aid : node.vOutputAttributes)
	{
		qFreedIds.push(aid);
		mAttributes.erase(aid);
	}

	qFreedIds.push(id);
	mNodes.erase(id);
}

void CNodeAttribContainer::createLink(int32_t from, int32_t to)
{
	auto lhs = mAttributes.find(from);
	if (lhs == mAttributes.end())
		log_error("Attribute with id {} was not found.", from);

	auto rhs = mAttributes.find(to);
	if (rhs == mAttributes.end())
		log_error("Attribute with id {} was not found.", to);

	// Looking for avaliable types
	auto found = std::find(rhs->second.types.begin(), rhs->second.types.end(), lhs->second.selectedType);
	if ((lhs->second.selectedType == rhs->second.selectedType || found != rhs->second.types.end() || rhs->second.selectedType == EMaterialEditorValueType::eNone) && !rhs->second.linked)
	{
		auto node = mNodes.find(rhs->second.parent_id);
		if (node != mNodes.end())
		{
			for (auto& attrib_id : node->second.vInputAttributes)
			{
				auto attrib = mAttributes.find(attrib_id);
				if (attrib == mAttributes.end())
					continue;

				auto target_type = std::find(attrib->second.types.begin(), attrib->second.types.end(), lhs->second.selectedType);

				if (attrib->second.types.size() == 1)
					attrib->second.selectedType = attrib->second.types.front();
				else
				{
					if (target_type != attrib->second.types.end())
						attrib->second.selectedType = *target_type;
					else
						attrib->second.selectedType = attrib->second.types.at(attrib->second.defautlType);
				}
			}

			for (auto& attrib_id : node->second.vOutputAttributes)
			{
				auto attrib = mAttributes.find(attrib_id);
				if (attrib == mAttributes.end())
					continue;

				auto target_type = std::find(attrib->second.types.begin(), attrib->second.types.end(), lhs->second.selectedType);

				if (attrib->second.types.size() == 1)
					attrib->second.selectedType = attrib->second.types.front();
				else
				{
					if (target_type != attrib->second.types.end())
						attrib->second.selectedType = *target_type;
					else
						attrib->second.selectedType = attrib->second.types.at(attrib->second.defautlType);
				}
			}
		}

		lhs->second.linked = rhs->second.linked = true;

		mLinks.emplace(next_id, std::make_pair(from, to));
		nextId();
	}
}

void CNodeAttribContainer::removeLink(int32_t id)
{
	auto link = mLinks.find(id);
	if(link == mLinks.end())
		log_error("Link with id {} was not found.", id);

	auto lhs = mAttributes.find(link->second.first);
	if(lhs == mAttributes.end())
		log_error("Attribute with id {} was not found.", link->second.first);

	auto rhs = mAttributes.find(link->second.second);
	if (rhs == mAttributes.end())
		log_error("Attribute with id {} was not found.", link->second.second);

	lhs->second.linked = rhs->second.linked = false;

	auto node = mNodes.find(rhs->second.parent_id);
	if (node != mNodes.end())
	{
		for (auto& attrib_id : node->second.vInputAttributes)
		{
			auto attrib = mAttributes.find(attrib_id);
			if (attrib == mAttributes.end())
				continue;

			if (attrib->second.types.size() == 1)
				attrib->second.selectedType = attrib->second.types.front();
			else
				attrib->second.selectedType = attrib->second.types.at(attrib->second.defautlType);
		}

		for (auto& attrib_id : node->second.vOutputAttributes)
		{
			auto attrib = mAttributes.find(attrib_id);
			if (attrib == mAttributes.end())
				continue;

			if (attrib->second.types.size() == 1)
				attrib->second.selectedType = attrib->second.types.front();
			else
				attrib->second.selectedType = attrib->second.types.at(attrib->second.defautlType);
		}
	}

	qFreedIds.push(id);
	mLinks.erase(id);
}

void CNodeAttribContainer::render()
{
	for (auto& [node_id, node] : mNodes)
	{
		bool hasInputs = !node.vInputAttributes.empty();

		ImNodes::BeginNode(node_id);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(node.name.c_str());
		ImNodes::EndNodeTitleBar();

		for (auto& attr_id : node.vInputAttributes)
			renderAttribute(attr_id, true, hasInputs);

		for (auto& attr_id : node.vOutputAttributes)
			renderAttribute(attr_id, false, hasInputs);

		ImNodes::EndNode();
	}

	for (auto& [id, link] : mLinks)
	{
		ImNodes::Link(id, link.first, link.second);
	}
}

void CNodeAttribContainer::renderAttribute(int32_t attr_id, bool isInput, bool hasInputs)
{
	const float node_width = 100.f;

	auto found = mAttributes.find(attr_id);
	if (found == mAttributes.end())
		log_error("Attribute with id {} was not found.", attr_id);

	auto& attribute = found->second;
	switch (attribute.selectedType)
	{
	case EMaterialEditorValueType::eBool: 
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(128, 0, 0, 255)); 
		break;
	case EMaterialEditorValueType::eInt:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(0, 255, 0, 255)); 
		break;
	case EMaterialEditorValueType::eFloat:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(255, 0, 255, 255)); 
		break;
	case EMaterialEditorValueType::eVec2:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(128, 0, 128, 255));
		break;
	case EMaterialEditorValueType::eVec3:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(160, 0, 160, 255));
		break;
	case EMaterialEditorValueType::eVec4:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(192, 0, 192, 255));
		break;
	case EMaterialEditorValueType::eIvec2:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(0, 128, 0, 255));
		break;
	case EMaterialEditorValueType::eIvec3:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(0, 160, 0, 255));
		break;
	case EMaterialEditorValueType::eIvec4:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(0, 192, 0, 255));
		break;
	case EMaterialEditorValueType::eMat2:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(128, 128, 0, 255));
		break;
	case EMaterialEditorValueType::eMat3:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(192, 192, 0, 255));
		break;
	case EMaterialEditorValueType::eMat4:
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(255, 255, 0, 255));
		break;
	}

	if(isInput)
		ImNodes::BeginInputAttribute(attr_id);
	else
		ImNodes::BeginOutputAttribute(attr_id);

	const float label_width = ImGui::CalcTextSize(attribute.name.c_str()).x;
	ImGui::PushItemWidth(node_width - label_width);

	if ((attribute.linked && hasInputs) || (!isInput && hasInputs))
	{
		if(!isInput)
			ImGui::Indent(node_width - label_width);

		ImGui::Text(attribute.name.c_str());
	}
	else
	{
		switch (attribute.selectedType)
		{
		case EMaterialEditorValueType::eBool: {
			static bool value{ false };
			ImGui::Checkbox(attribute.name.c_str(), &value);
		} break;
		case EMaterialEditorValueType::eInt: {
			static int value{ 0 };
			ImGui::DragInt(attribute.name.c_str(), &value);
		} break;
		case EMaterialEditorValueType::eFloat: {
			static float value{ 0.f };
			ImGui::DragFloat(attribute.name.c_str(), &value);
		} break;
		case EMaterialEditorValueType::eVec2: {
			static glm::vec2 value{ 0.f };
			ImGui::DragFloat2(attribute.name.c_str(), glm::value_ptr(value));
		}break;
		case EMaterialEditorValueType::eVec3: {
			static glm::vec3 value{ 0.f };
			ImGui::DragFloat3(attribute.name.c_str(), glm::value_ptr(value));
		}break;
		case EMaterialEditorValueType::eVec4: {
			static glm::vec4 value{ 0.f };
			ImGui::DragFloat4(attribute.name.c_str(), glm::value_ptr(value));
		}break;
		case EMaterialEditorValueType::eIvec2: {
			static glm::ivec2 value{ 0 };
			ImGui::DragInt2(attribute.name.c_str(), glm::value_ptr(value));
		}break;
		case EMaterialEditorValueType::eIvec3: {
			static glm::ivec3 value{ 0 };
			ImGui::DragInt3(attribute.name.c_str(), glm::value_ptr(value));
		}break;
		case EMaterialEditorValueType::eIvec4: {
			static glm::ivec4 value{ 0 };
			ImGui::DragInt4(attribute.name.c_str(), glm::value_ptr(value));
		}break;
		case EMaterialEditorValueType::eMat2: {
			static glm::mat2 value{ 0.f };

			ImGui::DragFloat2(attribute.name.c_str(), glm::value_ptr(value[0]));
			ImGui::DragFloat2(attribute.name.c_str(), glm::value_ptr(value[1]));
		}break;
		case EMaterialEditorValueType::eMat3: {
			static glm::mat3 value{ 0.f };
			ImGui::DragFloat3(attribute.name.c_str(), glm::value_ptr(value[0]));
			ImGui::DragFloat3(attribute.name.c_str(), glm::value_ptr(value[1]));
			ImGui::DragFloat3(attribute.name.c_str(), glm::value_ptr(value[2]));
		}break;
		case EMaterialEditorValueType::eMat4: {
			static glm::mat4 value{ 0.f };
			ImGui::DragFloat4(attribute.name.c_str(), glm::value_ptr(value[0]));
			ImGui::DragFloat4(attribute.name.c_str(), glm::value_ptr(value[1]));
			ImGui::DragFloat4(attribute.name.c_str(), glm::value_ptr(value[2]));
			ImGui::DragFloat4(attribute.name.c_str(), glm::value_ptr(value[3]));
		}break;
		}
	}

	ImGui::PopItemWidth();

	if(isInput)
		ImNodes::EndInputAttribute();
	else
		ImNodes::EndOutputAttribute();

	ImNodes::PopColorStyle();
}

void CNodeAttribContainer::nextId()
{
	static std::atomic<int32_t> uid{ 0 };

	if (!qFreedIds.empty())
	{
		next_id = qFreedIds.front();
		qFreedIds.pop();
	}
	else
	{
		uid.fetch_add(1, std::memory_order_relaxed);

		if (uid == invalid_index)
			uid.fetch_add(1, std::memory_order_relaxed);

		next_id = uid;
	}
}