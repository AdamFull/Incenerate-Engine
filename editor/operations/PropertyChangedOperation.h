#pragma once

#include "Operation.h"
#include <any>

namespace engine
{
	namespace editor
	{
		class CPropertyChangedOperation : public IOperation
		{
		public:
			virtual ~CPropertyChangedOperation() override = default;
			CPropertyChangedOperation(std::any vold, std::any vnew, size_t property_type);

			void undo() override;
			void redo() override;

		private:
			size_t prop_type{ invalid_index };
			std::any oldValue;
			std::any newValue;
			std::any newValueCpy;
		};
	}
}