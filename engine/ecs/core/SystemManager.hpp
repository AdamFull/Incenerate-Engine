#pragma once

#include "System.hpp"
#include <unordered_map>
#include <memory>

namespace engine
{
	namespace ecs
	{
		class CSystemManager
		{
		public:
			template<class _Ty>
			_Ty* registrate()
			{
				const char* typeName = typeid(_Ty).name();

				assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

				mSystems.insert({ typeName, std::make_unique<_Ty>() });
				return static_cast<_Ty*>(mSystems[typeName].get());
			}

			template<class _Ty>
			void setSignature(Signature signature)
			{
				const char* typeName = typeid(_Ty).name();

				assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

				mSignatures.insert({ typeName, signature });
			}

			void entityDestroyed(Entity entity);
			void entitySignatureChanged(Entity entity, Signature entitySignature);

		private:
			std::unordered_map<const char*, Signature> mSignatures{};
			std::unordered_map<const char*, std::unique_ptr<ISystem>> mSystems{};
		};
	}
}