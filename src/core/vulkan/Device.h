#pragma once

#include "../API.h"

#include <vulkan/vulkan.h>

namespace Euler
{
    namespace Vulkan
    {
        class EULER_API Device
        {
        private:

        public:
            void Init();
            void Cleanup();
        };
    };
};
