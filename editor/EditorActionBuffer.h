#pragma once

#include <stack>
#include <memory>

#include "operations/Operation.h"

namespace engine
{
	namespace editor
	{
		class CEditorActionBuffer
		{
		public:
			void addOperation(std::unique_ptr<IOperation>&& operation);

			void undo();
			void redo();

			bool canUndo();
			bool canRedo();

		private:
			std::stack<std::unique_ptr<IOperation>> undo_stack_;
			std::stack<std::unique_ptr<IOperation>> redo_stack_;
		};
	}
}