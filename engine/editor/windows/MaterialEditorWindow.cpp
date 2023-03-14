#include "MaterialEditorWindow.h"

#include "Engine.h"
#include "system/filesystem/filesystem.h"

#include <imgui/imgui_node_editor_internal.h>
#include "materialeditor/utilities/builders.h"
#include "materialeditor/utilities/widgets.h"

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace engine::system;
using namespace engine::editor;
using namespace engine::graphics;

ImColor getAttributeColor(EMaterialEditorValueType type)
{
	uint32_t color{ 0xffffffff };

	switch (type)
	{
	case engine::editor::EMaterialEditorValueType::eBool: color = IM_COL32(220, 48, 48, 255); break;
	case engine::editor::EMaterialEditorValueType::eFloat: color = IM_COL32(147, 226, 74, 255); break;
	case engine::editor::EMaterialEditorValueType::eVec2: color = IM_COL32(228, 228, 75, 255); break;
	case engine::editor::EMaterialEditorValueType::eVec3: color = IM_COL32(228, 171, 75, 255); break;
	case engine::editor::EMaterialEditorValueType::eVec4: color = IM_COL32(228, 105, 75, 255); break;
	case engine::editor::EMaterialEditorValueType::eMat2: color = IM_COL32(228, 228, 128, 255); break;
	case engine::editor::EMaterialEditorValueType::eMat3: color = IM_COL32(228, 171, 128, 255); break;
	case engine::editor::EMaterialEditorValueType::eMat4: color = IM_COL32(228, 105, 128, 255); break;
	case engine::editor::EMaterialEditorValueType::eDouble: color = IM_COL32(228, 75, 166, 255); break;
	case engine::editor::EMaterialEditorValueType::eDvec2: color = IM_COL32(228, 75, 207, 255); break;
	case engine::editor::EMaterialEditorValueType::eDvec3: color = IM_COL32(228, 75, 228, 255); break;
	case engine::editor::EMaterialEditorValueType::eDvec4: color = IM_COL32(195, 75, 228, 255); break;
	case engine::editor::EMaterialEditorValueType::eDmat2: color = IM_COL32(228, 128, 207, 255); break;
	case engine::editor::EMaterialEditorValueType::eDmat3: color = IM_COL32(228, 128, 228, 255); break;
	case engine::editor::EMaterialEditorValueType::eDmat4: color = IM_COL32(195, 128, 228, 255); break;
	case engine::editor::EMaterialEditorValueType::eInt: color = IM_COL32(68, 201, 156, 255); break;
	case engine::editor::EMaterialEditorValueType::eIvec2: color = IM_COL32(68, 202, 180, 255); break;
	case engine::editor::EMaterialEditorValueType::eIvec3: color = IM_COL32(68, 198, 202, 255); break;
	case engine::editor::EMaterialEditorValueType::eIvec4: color = IM_COL32(68, 175, 202, 255); break;
	case engine::editor::EMaterialEditorValueType::eUInt: color = IM_COL32(128, 201, 156, 255); break;
	case engine::editor::EMaterialEditorValueType::eUvec2: color = IM_COL32(128, 202, 180, 255); break;
	case engine::editor::EMaterialEditorValueType::eUvec3: color = IM_COL32(128, 198, 202, 255); break;
	case engine::editor::EMaterialEditorValueType::eUvec4: color = IM_COL32(128, 175, 202, 255); break;
	}

	return color;
}

ImColor getNodeHeaderColor(EMaterialEditorNodeType type)
{
	uint32_t color{ 0xffffffff };

	switch (type)
	{
	case engine::editor::EMaterialEditorNodeType::eConstValue: color = IM_COL32(55, 137, 219, 255); break;
	case engine::editor::EMaterialEditorNodeType::eUniformValue: color = IM_COL32(225, 57, 57, 255); break;
	case engine::editor::EMaterialEditorNodeType::eSampler: color = IM_COL32(108, 108, 108, 255); break;

	case engine::editor::EMaterialEditorNodeType::eMath: 
	case engine::editor::EMaterialEditorNodeType::eGeometric:
	case engine::editor::EMaterialEditorNodeType::eMatrix:
	case engine::editor::EMaterialEditorNodeType::eDerivative:
	case engine::editor::EMaterialEditorNodeType::eOperation: color = IM_COL32(66, 191, 78, 255); break;
	}

	return color;
}

void showLabel(const char* label, ImColor color)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
	auto size = ImGui::CalcTextSize(label);

	auto padding = ImGui::GetStyle().FramePadding;
	auto spacing = ImGui::GetStyle().ItemSpacing;

	ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

	auto rectMin = ImGui::GetCursorScreenPos() - padding;
	auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
	ImGui::TextUnformatted(label);
}

CMaterialEditorWindow::~CMaterialEditorWindow()
{
	if (!pBuilder)
	{
		delete pBuilder;
		pBuilder = nullptr;
	}

	if (!pEditor)
	{
		ax::NodeEditor::DestroyEditor(pEditor);
		pEditor = nullptr;
	}
}

void CMaterialEditorWindow::create()
{
	auto& graphics = EGEngine->getGraphics();
	auto& device = graphics->getDevice();

	auto bpBack = std::make_unique<CImage>(device.get());
	bpBack->create("icon/editor/blueprint_back.png", vk::Format::eR8G8B8A8Unorm);
	auto extent = bpBack->getExtent();

	pBuilder = new util::BlueprintNodeBuilder(graphics->addImage("editor_blueprint_img_noise", std::move(bpBack)), extent.width, extent.height);

	//bIsOpen = false;
	EGEngine->addEventListener(Events::Editor::SelectMaterial, this, &CMaterialEditorWindow::onSetMaterial);

	fs::read_json("materialeditor.json", vGroups, true);

	ed::Config config;
	config.SettingsFile = "materialEditorSettings.json";
	config.UserPointer = this;

	//config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
	//{
	//    auto self = static_cast<Example*>(userPointer);
	//
	//    auto node = self->FindNode(nodeId);
	//    if (!node)
	//        return 0;
	//
	//    if (data != nullptr)
	//        memcpy(data, node->State.data(), node->State.size());
	//    return node->State.size();
	//};
	//
	//config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
	//{
	//    auto self = static_cast<Example*>(userPointer);
	//
	//    auto node = self->FindNode(nodeId);
	//    if (!node)
	//        return false;
	//
	//    node->State.assign(data, size);
	//
	//    self->TouchNode(nodeId);
	//
	//    return true;
	//};

	pEditor = ed::CreateEditor(&config);
	ed::SetCurrentEditor(pEditor);

	ed::NavigateToContent();
}

void CMaterialEditorWindow::__draw(float fDt)
{
	static ed::NodeId contextNodeId = 0;
	static ed::LinkId contextLinkId = 0;
	static ed::PinId  contextPinId = 0;
	static ed::PinId newNodeLinkPin = 0;
	static ed::PinId newLinkPin = 0;

	const bool open_popup =
		ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
		ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	if (open_popup)
		pNextPopup = "add node";

	for (auto& [id, time] : mNodeTouchTime)
	{
		if (time > 0.0f)
			time -= fDt;
	}

	ed::SetCurrentEditor(pEditor);
	ed::Begin("Material editor");

	for (auto& node : vNodes)
	{
		pBuilder->Begin(node->id);

		pBuilder->Header(getNodeHeaderColor(node->eNodeType));
		ImGui::Spring(0);
		ImGui::TextUnformatted(node->srName.c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		ImGui::Spring(0);
		pBuilder->EndHeader();

		for (auto& iid : node->vInputAttributes)
		{
			auto& attribute = findAttribute(iid);
			auto alpha = ImGui::GetStyle().Alpha;
			pBuilder->Input(iid);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			ax::Widgets::Icon(ImVec2(24.f, 24.f), ax::Drawing::IconType::Circle, attribute->linked, getAttributeColor(attribute->eSelectedType), ImColor(32, 32, 32, (int)(alpha * 255)));
			ImGui::Spring(0);
			ImGui::TextUnformatted(attribute->srName.c_str());
			ImGui::Spring(0);
			ImGui::PopStyleVar();
			pBuilder->EndInput();
		}

		for (auto& oid : node->vOutputAttributes)
		{
			auto& attribute = findAttribute(oid);
			auto alpha = ImGui::GetStyle().Alpha;
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			pBuilder->Output(oid);
			ImGui::Spring(0);
			ImGui::TextUnformatted(attribute->srName.c_str());
			ImGui::Spring(0);
			ax::Widgets::Icon(ImVec2(24.f, 24.f), ax::Drawing::IconType::Circle, attribute->linked, getAttributeColor(attribute->eSelectedType), ImColor(32, 32, 32, (int)(alpha * 255)));
			ImGui::PopStyleVar();
			pBuilder->EndOutput();
		}

		pBuilder->End();
	}

	// Process links
	for (auto& link : vLinks)
	{
		auto& attribute = findAttribute(link->startPin);
		ed::Link(link->id, link->startPin, link->endPin, getAttributeColor(attribute->eSelectedType), 2.0f);
	}

	if (!createNewNode)
	{
		if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
		{
			// Creating new link
			ed::PinId startPinId = 0, endPinId = 0;
			if (ed::QueryNewLink(&startPinId, &endPinId))
			{
				auto& startPin = findAttribute(startPinId);
				auto& endPin = findAttribute(endPinId);

				newLinkPin = startPin ? startPin->id : endPin->id;

				if (startPin->ePinKind == EMaterialEditorPinKind::eInput)
				{
					std::swap(startPin, endPin);
					std::swap(startPinId, endPinId);
				}

				if (startPin && endPin)
				{
					if (endPin == startPin)
					{
						ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					}
					else if (endPin->ePinKind == startPin->ePinKind)
					{
						showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
						ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					}
					else if (!canCreateLink(startPin, endPin))
					{
						showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
						ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
					}
					else
					{
						showLabel("+ Create Link", ImColor(32, 45, 32, 180));
						if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
							createLink(startPinId, endPinId);
					}
				}
			}

			ed::PinId pinId = 0;
			if (ed::QueryNewNode(&pinId))
			{
				newLinkPin = findAttribute(pinId)->id;
				if (newLinkPin)
					showLabel("+ Create Node", ImColor(32, 45, 32, 180));

				if (ed::AcceptNewItem())
				{
					createNewNode = true;
					newNodeLinkPin = findAttribute(pinId)->id;
					newLinkPin = 0;
					ed::Suspend();
					pNextPopup = "add node";
					ed::Resume();
				}
			}
		}
		else
			newLinkPin = 0;

		ed::EndCreate();

		if (ed::BeginDelete())
		{
			ed::LinkId linkId = 0;
			while (ed::QueryDeletedLink(&linkId))
			{
				if (ed::AcceptDeletedItem())
					destroyLink(linkId);
			}

			ed::NodeId nodeId = 0;
			while (ed::QueryDeletedNode(&nodeId))
			{
				if (ed::AcceptDeletedItem())
					destroyNode(nodeId);
			}
		}
		ed::EndDelete();
	}

	ed::End();

	if (pNextPopup)
	{
		ImGui::OpenPopup(pNextPopup);
		pNextPopup = nullptr;
	}

	if (ImGui::BeginPopup("add node"))
	{
		makePopupMenuContent(vGroups);
		createNewNode = false;
		ImGui::EndPopup();
	}
}

uint32_t CMaterialEditorWindow::nextId()
{
	static uint32_t out_id{ 1 };
	static std::atomic<uint32_t> uid{ 1 };

	if (!qFreedIds.empty())
	{
		out_id = qFreedIds.front();
		qFreedIds.pop();
	}
	else
	{
		out_id = uid;

		uid.fetch_add(1, std::memory_order_relaxed);

		if (uid == invalid_index)
			uid.fetch_add(1, std::memory_order_relaxed);
	}

	return out_id;
}

std::unique_ptr<FMaterialEditorNode>& CMaterialEditorWindow::findNode(ax::NodeEditor::NodeId id)
{
	static std::unique_ptr<FMaterialEditorNode> pNotFound;

	auto found = std::find_if(vNodes.begin(), vNodes.end(), [id = id](const std::unique_ptr<FMaterialEditorNode>& pNode) { return pNode->id == id; });
	if (found != vNodes.end())
		return *found;

	return pNotFound;
}

std::unique_ptr<FMaterialEditorAttribute>& CMaterialEditorWindow::findAttribute(ax::NodeEditor::PinId id)
{
	static std::unique_ptr<FMaterialEditorAttribute> pNotFound;

	auto found = std::find_if(vAttributes.begin(), vAttributes.end(), [id = id](const std::unique_ptr<FMaterialEditorAttribute>& pNode) { return pNode->id == id; });
	if (found != vAttributes.end())
		return *found;

	return pNotFound;
}

std::unique_ptr<FMaterialEditorLink>& CMaterialEditorWindow::findLink(ax::NodeEditor::LinkId id)
{
	static std::unique_ptr<FMaterialEditorLink> pNotFound;

	auto found = std::find_if(vLinks.begin(), vLinks.end(), [id = id](const std::unique_ptr<FMaterialEditorLink>& pNode) { return pNode->id == id; });
	if (found != vLinks.end())
		return *found;

	return pNotFound;
}

ax::NodeEditor::NodeId CMaterialEditorWindow::createNode(const FMaterialEditorNodeCreateInfo& ci)
{
	auto id = nextId();
	auto pNode = std::make_unique<FMaterialEditorNode>();
	pNode->id = id;
	pNode->srName = ci.name;
	pNode->eNodeType = ci.type;

	for (auto& attrib : ci.inputAttributes)
	{
		auto pAttribute = std::make_unique<FMaterialEditorAttribute>();
		pAttribute->id = nextId();
		pAttribute->parent = pNode->id;
		pAttribute->srName = attrib.name;
		pAttribute->ePinKind = EMaterialEditorPinKind::eInput;
		pAttribute->vTypes = attrib.types;
		pAttribute->defautlType = attrib.defaultType;
		pAttribute->eSelectedType = pAttribute->vTypes[pAttribute->defautlType];

		pNode->vInputAttributes.emplace_back(pAttribute->id);
		vAttributes.emplace_back(std::move(pAttribute));
	}

	for (auto& attrib : ci.outputAttributes)
	{
		auto pAttribute = std::make_unique<FMaterialEditorAttribute>();
		pAttribute->id = nextId();
		pAttribute->parent = pNode->id;
		pAttribute->srName = attrib.name;
		pAttribute->ePinKind = EMaterialEditorPinKind::eOutput;
		pAttribute->vTypes = attrib.types;
		pAttribute->defautlType = attrib.defaultType;
		pAttribute->eSelectedType = pAttribute->vTypes[pAttribute->defautlType];

		pNode->vOutputAttributes.emplace_back(pAttribute->id);
		vAttributes.emplace_back(std::move(pAttribute));
	}

	vNodes.emplace_back(std::move(pNode));
	return id;
}

void CMaterialEditorWindow::destroyNode(ax::NodeEditor::NodeId id)
{
	auto& pNode = findNode(id);

	if (!pNode)
	{
		log_error("Failed to find node with id {}.", id.Get());
		return;
	}

	// Making single vector with all node attributes
	std::vector<ed::PinId> nodeAttributes;
	nodeAttributes.insert(nodeAttributes.end(), pNode->vInputAttributes.begin(), pNode->vInputAttributes.end());
	nodeAttributes.insert(nodeAttributes.end(), pNode->vOutputAttributes.begin(), pNode->vOutputAttributes.end());

	// Releasing attributes and saving id for reuse
	for (auto& aid : nodeAttributes)
	{
		qFreedIds.push(aid.Get());
		auto to_remove = std::remove_if(vAttributes.begin(), vAttributes.end(), [id = aid](const std::unique_ptr<FMaterialEditorAttribute>& attrib) { return attrib->id == id; });
		vAttributes.erase(to_remove);
	}

	qFreedIds.push(id.Get());
	auto to_remove = std::remove_if(vNodes.begin(), vNodes.end(), [id = id](const std::unique_ptr<FMaterialEditorNode>& node) { return node->id == id; });
	vNodes.erase(to_remove);
}

void CMaterialEditorWindow::createLink(ax::NodeEditor::PinId startPin, ax::NodeEditor::PinId endPin)
{
	auto& lhs = findAttribute(startPin);
	auto& rhs = findAttribute(endPin);

	auto foundType = std::find(rhs->vTypes.begin(), rhs->vTypes.end(), lhs->eSelectedType);
	if ((lhs->eSelectedType == rhs->eSelectedType || foundType != rhs->vTypes.end() || rhs->eSelectedType == EMaterialEditorValueType::eNone) && !rhs->linked)
	{
		auto& pNode = findNode(rhs->parent);
		if (pNode)
		{
			std::vector<ed::PinId> nodeAttributes;
			nodeAttributes.insert(nodeAttributes.end(), pNode->vInputAttributes.begin(), pNode->vInputAttributes.end());
			nodeAttributes.insert(nodeAttributes.end(), pNode->vOutputAttributes.begin(), pNode->vOutputAttributes.end());

			for (auto& aid : nodeAttributes)
			{
				auto& attrib = findAttribute(aid);
				if (!attrib)
					continue;

				auto target_type = std::find(attrib->vTypes.begin(), attrib->vTypes.end(), lhs->eSelectedType);

				if (attrib->vTypes.size() == 1)
					attrib->eSelectedType = attrib->vTypes.front();
				else
				{
					if (target_type != attrib->vTypes.end())
						attrib->eSelectedType = *target_type;
					else
						attrib->eSelectedType = attrib->vTypes.at(attrib->defautlType);
				}
			}
		}

		lhs->linked = rhs->linked = true;

		auto pLink = std::make_unique<FMaterialEditorLink>();
		pLink->id = nextId();
		pLink->startPin = startPin;
		pLink->endPin = endPin;
		vLinks.emplace_back(std::move(pLink));
	}
}

void CMaterialEditorWindow::destroyLink(ax::NodeEditor::LinkId id)
{
	auto& link = findLink(id);
	if (!link)
		return;

	auto& lhs = findAttribute(link->startPin);
	auto& rhs = findAttribute(link->endPin);

	lhs->linked = rhs->linked = false;

	auto& pNode = findNode(rhs->parent);
	if (pNode)
	{
		std::vector<ed::PinId> nodeAttributes;
		nodeAttributes.insert(nodeAttributes.end(), pNode->vInputAttributes.begin(), pNode->vInputAttributes.end());
		nodeAttributes.insert(nodeAttributes.end(), pNode->vOutputAttributes.begin(), pNode->vOutputAttributes.end());

		for (auto& aid : nodeAttributes)
		{
			auto& attrib = findAttribute(aid);
			if (!attrib)
				continue;

			if (attrib->vTypes.size() == 1)
				attrib->eSelectedType = attrib->vTypes.front();
			else
				attrib->eSelectedType = attrib->vTypes.at(attrib->defautlType);
		}
	}

	// TODO: remove attributes linked
	qFreedIds.push(id.Get());
	auto to_remove = std::remove_if(vLinks.begin(), vLinks.end(), [id = id](const std::unique_ptr<FMaterialEditorLink>& link) { return link->id == id; });
	vLinks.erase(to_remove);
}

bool CMaterialEditorWindow::canCreateLink(const std::unique_ptr<FMaterialEditorAttribute>& lhs, const std::unique_ptr<FMaterialEditorAttribute>& rhs)
{
	if (lhs == rhs)
		return false;

	if (lhs->ePinKind == rhs->ePinKind || lhs->parent == rhs->parent)
		return false;

	// Check all avaliable types
	auto targetType = std::find(rhs->vTypes.begin(), rhs->vTypes.end(), lhs->eSelectedType);
	if (targetType == rhs->vTypes.end())
		return false;

	// Check is some target pins already linked, and type is selected
	bool isTypeSelected{ false };
	auto& pNode = findNode(rhs->parent);
	for (auto& id : pNode->vInputAttributes)
	{
		auto& attr = findAttribute(id);
		if (attr->linked && lhs->eSelectedType != attr->eSelectedType)
			return false;
	}

	return true;
}

void CMaterialEditorWindow::onSetMaterial(CEvent& event)
{
	bIsOpen = true;
	selected_material = event.getParam<size_t>(Events::Editor::SelectMaterial);
}

void CMaterialEditorWindow::makePopupMenuContent(const std::vector<FMaterialEditorGroupCreateInfo>& groups)
{
	const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
	for (auto& group : groups)
	{
		if (!group.subgroups.empty())
		{
			if (ImGui::BeginMenu(group.name.c_str()))
			{
				makePopupMenuContent(group.subgroups);
				ImGui::EndMenu();
			}
		}
		else
		{
			if (ImGui::BeginMenu(group.name.c_str()))
			{
				for (auto& node : group.nodes)
				{
					if (ImGui::MenuItem(node.name.c_str()))
					{
						auto id = createNode(node);
						ed::SetNodePosition(id, click_pos);
					}
				}
				ImGui::EndMenu();
			}
		}
	}
}