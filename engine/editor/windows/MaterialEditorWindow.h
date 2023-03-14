#pragma once

#include "WindowBase.h"
#include "materialeditor/MaterialEditorParse.h"
#include "event/Event.hpp"

#include <imgui/imgui_node_editor.h>

#include <queue>

namespace ax 
{
	namespace NodeEditor 
	{
		namespace Utilities 
		{
			struct BlueprintNodeBuilder;
		}
	}
}

namespace engine
{
	namespace editor
	{
		struct FNodeIdLess
		{
			bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
			{
				return lhs.AsPointer() < rhs.AsPointer();
			}
		};

		struct FMaterialEditorAttribute
		{
			ax::NodeEditor::PinId id;
			ax::NodeEditor::NodeId parent;

			std::string srName;
			EMaterialEditorPinKind ePinKind;
			std::vector<EMaterialEditorValueType> vTypes;
			EMaterialEditorValueType eSelectedType;
			size_t defautlType{ 0 };
			bool linked{ false };
		};

		struct FMaterialEditorNode
		{
			ax::NodeEditor::NodeId id;
			std::string srName{ "" };
			EMaterialEditorNodeType eNodeType;

			std::string srState;
			std::string srSavedState;

			std::vector<ax::NodeEditor::PinId> vInputAttributes;
			std::vector<ax::NodeEditor::PinId> vOutputAttributes;
		};

		struct FMaterialEditorLink
		{
			ax::NodeEditor::LinkId id;

			ax::NodeEditor::PinId startPin;
			ax::NodeEditor::PinId endPin;
		};

		class CMaterialEditorWindow : public IEditorWindow
		{
		public:
			CMaterialEditorWindow(const std::string& sname) { name = sname; }
			virtual ~CMaterialEditorWindow() override;

			void create() override;
			void __draw(float fDt) override;
		private:
			uint32_t nextId();

			std::unique_ptr<FMaterialEditorNode>& findNode(ax::NodeEditor::NodeId id);
			std::unique_ptr<FMaterialEditorAttribute>& findAttribute(ax::NodeEditor::PinId id);
			std::unique_ptr<FMaterialEditorLink>& findLink(ax::NodeEditor::LinkId id);

			ax::NodeEditor::NodeId createNode(const FMaterialEditorNodeCreateInfo& ci);
			void destroyNode(ax::NodeEditor::NodeId id);

			void createLink(ax::NodeEditor::PinId startPin, ax::NodeEditor::PinId endPin);
			void destroyLink(ax::NodeEditor::LinkId id);

			bool canCreateLink(const std::unique_ptr<FMaterialEditorAttribute>& lhs, const std::unique_ptr<FMaterialEditorAttribute>& rhs);

			void onSetMaterial(CEvent& event);
			void makePopupMenuContent(const std::vector<FMaterialEditorGroupCreateInfo>& groups);
		private:
			size_t selected_material{ invalid_index };
			int32_t hovered_id{ 0 };

			const char* pNextPopup{ nullptr };
			bool createNewNode{ false };

			std::queue<int32_t> qFreedIds;

			ax::NodeEditor::EditorContext* pEditor{ nullptr };
			ax::NodeEditor::Utilities::BlueprintNodeBuilder* pBuilder{ nullptr };

			std::vector<FMaterialEditorGroupCreateInfo> vGroups;

			std::map<ax::NodeEditor::NodeId, float, FNodeIdLess> mNodeTouchTime;
			std::vector<std::unique_ptr<FMaterialEditorNode>> vNodes;
			std::vector<std::unique_ptr<FMaterialEditorAttribute>> vAttributes;
			std::vector<std::unique_ptr<FMaterialEditorLink>> vLinks;
		};
	}
}