#pragma once

#include "Handler.h"

namespace engine
{
	namespace graphics
	{
        class CUniformHandler : public CHandler
        {
        public:
            CUniformHandler() = default;
            CUniformHandler(CDevice* device);
            virtual ~CUniformHandler() = default;

            void create(const CUniformBlock& uniformBlock) override;
        };
	}
}