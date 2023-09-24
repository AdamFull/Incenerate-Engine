#pragma once

#include <uparse.hpp>

#include "compiler_structures.h"

NLOHMANN_JSON_SERIALIZE_ENUM(EShaderType, 
    {
        { EShaderType::eUnknown, nullptr },
        { EShaderType::eVertex, "vertex" },
        { EShaderType::eControl, "control" },
        { EShaderType::eEvaluation, "evaluation" },
        { EShaderType::eGeometry, "geometry" },
        { EShaderType::eFragment, "fragment" },
        { EShaderType::eCompute, "compute" },
        { EShaderType::eRayGen, "raygen" },
        { EShaderType::eIntersection, "intersection" },
        { EShaderType::eAnyHit, "anyhit" },
        { EShaderType::eClosestHit, "closesthit" },
        { EShaderType::eMiss, "miss" },
        { EShaderType::eCallable, "callable" },
        { EShaderType::eTask, "task" },
        { EShaderType::eMesh, "mesh" },
    })

struct FShaderModule
{
    EShaderType type{ EShaderType::eUnknown };
    std::string link;
};

struct FCompiledLibrary
{
    /*
    * shaders:
    *   vertex:
    *       "shader_hash": "code"
    * 
    * modules:
    *   module_unique_name: [
    *       { type, shader_hash }
    * ]
    */
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<uint32_t>>> shaders;
    std::unordered_map<std::string, std::vector<FShaderModule>> modules;
};

inline void to_json(nlohmann::json& json, const FShaderModule& type)
{
    json = nlohmann::json();
    utl::serialize_from("type", json, type.type, true);
    utl::serialize_from("link", json, type.link, true);
}

inline void from_json(const nlohmann::json& json, FShaderModule& type)
{
    utl::parse_to("type", json, type.type);
    utl::parse_to("link", json, type.link);
}


inline void to_json(nlohmann::json& json, const FCompiledLibrary& type)
{
    json = nlohmann::json();
    utl::serialize_from("shaders", json, type.shaders, true);
    utl::serialize_from("modules", json, type.modules, true);
}

inline void from_json(const nlohmann::json& json, FCompiledLibrary& type)
{
    utl::parse_to("shaders", json, type.shaders);
    utl::parse_to("modules", json, type.modules);
}