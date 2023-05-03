#pragma once

namespace engine
{
	namespace editor
	{
		class IOperation
		{
		public:
			virtual ~IOperation() = default;

			virtual void undo() = 0;
			virtual void redo() = 0;
		};
	}
}