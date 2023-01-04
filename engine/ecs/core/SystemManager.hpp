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
			std::unique_ptr<_Ty>& registrate()
			{
				const char* typeName = typeid(_Ty).name();

				assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

				mSystems.insert({ typeName, std::make_shared<_Ty>() });
				return mSystems[typeName];
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
			std::unordered_map<const char*, std::shared_ptr<ISystem>> mSystems{};
		};
	}
}