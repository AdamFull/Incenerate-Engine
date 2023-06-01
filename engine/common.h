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

#include "shared/SharedInstances.h"

// Level of detail
#define MAX_LEVEL_OF_DETAIL 4u

// Directional light
#define MAX_DIRECTIONAL_LIGHT_COUNT 3u
#define MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT 1u
#define CASCADE_SHADOW_MAP_RESOLUTION 2048u
#define CASCADE_SHADOW_MAP_CASCADE_COUNT 8u

// Spot light
#define MAX_SPOT_LIGHT_COUNT 64u //65535
#define MAX_SPOT_LIGHT_SHADOW_COUNT 16u
#define SPOT_LIGHT_SHADOW_MAP_RESOLUTION 1024u

// Point light
#define MAX_POINT_LIGHT_COUNT 64u //65535
#define MAX_POINT_LIGHT_SHADOW_COUNT 16u
#define POINT_LIGHT_SHADOW_MAP_RESOLUTION 512u