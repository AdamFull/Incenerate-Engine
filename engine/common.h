#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <filesystem>
#include <string>
#include <memory>
#include <optional>
#include <thread>
#include <unordered_map>
#include <map>
#include <set>
#include <list>
#include <vector>

#include <entt/fwd.hpp>

#include "fdecl.h"

constexpr const size_t invalid_index{ -1ull };

#define MAX_DIRECTIONAL_LIGHT_COUNT 1
#define MAX_SPOT_LIGHT_COUNT 15
#define MAX_POINT_LIGHT_COUNT 16
#define SHADOW_MAP_RESOLUTION 512