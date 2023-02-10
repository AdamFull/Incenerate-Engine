#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/epsilon.hpp>

#include <utility/logger/logger.h>
#include <utility/uparse.hpp>
#include <utility/upattern.hpp>
#include <utility/uflag.hpp>
#include <utility/ufunction.hpp>

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
#include <any>

#include <entt/entt.hpp>

#include "fdecl.h"

constexpr inline const size_t invalid_index{ -1ull };

#define MAX_DIRECTIONAL_LIGHT_COUNT 1
#define MAX_SPOT_LIGHT_COUNT 15
#define MAX_POINT_LIGHT_COUNT 16
#define SHADOW_MAP_RESOLUTION 512
#define SHADOW_MAP_CASCADE_COUNT 5