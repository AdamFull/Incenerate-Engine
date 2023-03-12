#pragma once
#include "MaterialEditorParse.h"

#include <queue>

namespace engine
{
	namespace editor
	{
		struct FMaterialEditorNode
		{
			std::string name{ "" };
			EMaterialEditorNodeType type;
			std::vector<int32_t> vInputAttributes;
			std::vector<int32_t> vOutputAttributes;
		};

		struct FMaterialEditorAttribute
		{
			std::string name;
			std::vector<EMaterialEditorValueType> types;
			EMaterialEditorValueType selectedType;
			size_t defautlType{ 0 };
			int32_t parent_id{ 0 };
			bool linked{ false };
		};

		class CNodeAttribContainer
		{
		public:
			int32_t createNode(const FMaterialEditorNodeCreateInfo& node_ci);
			void removeNode(int32_t id);

			void createLink(int32_t from, int32_t to);
			void removeLink(int32_t id);

			void render();
		private:
			void renderAttribute(int32_t attr_id, bool isInput, bool hasInputs);
			void nextId();
		private:
			int32_t next_id{ 0 };
			std::queue<int32_t> qFreedIds;

			std::map<int32_t, FMaterialEditorNode> mNodes;
			std::map<int32_t, FMaterialEditorAttribute> mAttributes;

			std::map<int32_t, std::pair<int32_t, int32_t>> mLinks;
		};
	}
}