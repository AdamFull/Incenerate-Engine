#pragma once

#include "Handler.h"

namespace engine
{
    namespace graphics
    {
        class CStorageHandler : public CHandler
        {
        public:
            CStorageHandler() = default;
            CStorageHandler(CDevice* device);
            virtual ~CStorageHandler() = default;

            void create(const CUniformBlock& uniformBlock) override;
        };
    }
}