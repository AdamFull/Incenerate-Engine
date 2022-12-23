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

#include "fdecl.h"

constexpr const size_t invalid_index{ -1ull };