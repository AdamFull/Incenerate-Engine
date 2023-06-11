#include "glmparse.h"

namespace glm
{
    /*--------------------GLM vectors bool--------------------*/
    //glm::bvec4
    void to_json(nlohmann::json& json, const bvec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != false);
        utl::serialize_from("y", json, type.y, type.y != false);
        utl::serialize_from("z", json, type.z, type.z != false);
        utl::serialize_from("w", json, type.w, type.w != false);
    }

    void from_json(const nlohmann::json& json, bvec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::bvec3
    void to_json(nlohmann::json& json, const bvec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != false);
        utl::serialize_from("y", json, type.y, type.y != false);
        utl::serialize_from("z", json, type.z, type.z != false);
    }

    void from_json(const nlohmann::json& json, bvec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::bvec2
    void to_json(nlohmann::json& json, const bvec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != false);
        utl::serialize_from("y", json, type.y, type.y != false);
    }

    void from_json(const nlohmann::json& json, bvec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::bvec1
    void to_json(nlohmann::json& json, const bvec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != false);
    }

    void from_json(const nlohmann::json& json, bvec1& type)
    {
        utl::parse_to("x", json, type.x);
    }

    /*--------------------GLM vectors int--------------------*/
    //glm::ivec4
    void to_json(nlohmann::json& json, const ivec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
        utl::serialize_from("z", json, type.z, type.z != 0);
        utl::serialize_from("w", json, type.w, type.w != 0);
    }

    void from_json(const nlohmann::json& json, ivec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::ivec3
    void to_json(nlohmann::json& json, const ivec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
        utl::serialize_from("z", json, type.z, type.z != 0);
    }

    void from_json(const nlohmann::json& json, ivec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::ivec2
    void to_json(nlohmann::json& json, const ivec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
    }

    void from_json(const nlohmann::json& json, ivec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::ivec1
    void to_json(nlohmann::json& json, const ivec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
    }

    void from_json(const nlohmann::json& json, ivec1& type)
    {
        utl::parse_to("x", json, type.x);
    }

    //glm::i8vec4
    void to_json(nlohmann::json& json, const i8vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
        utl::serialize_from("z", json, type.z, type.z != 0);
        utl::serialize_from("w", json, type.w, type.w != 0);
    }

    void from_json(const nlohmann::json& json, i8vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::i8vec3
    void to_json(nlohmann::json& json, const i8vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
        utl::serialize_from("z", json, type.z, type.z != 0);
    }

    void from_json(const nlohmann::json& json, i8vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::i8vec2
    void to_json(nlohmann::json& json, const i8vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
    }

    void from_json(const nlohmann::json& json, i8vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::i8vec1
    void to_json(nlohmann::json& json, const i8vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
    }

    void from_json(const nlohmann::json& json, i8vec1& type)
    {
        utl::parse_to("x", json, type.x);
    }


    //glm::i16vec4
    void to_json(nlohmann::json& json, const i16vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
        utl::serialize_from("z", json, type.z, type.z != 0);
        utl::serialize_from("w", json, type.w, type.w != 0);
    }

    void from_json(const nlohmann::json& json, i16vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::i16vec3
    void to_json(nlohmann::json& json, const i16vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
        utl::serialize_from("z", json, type.z, type.z != 0);
    }

    void from_json(const nlohmann::json& json, i16vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::i16vec2
    void to_json(nlohmann::json& json, const i16vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
        utl::serialize_from("y", json, type.y, type.y != 0);
    }

    void from_json(const nlohmann::json& json, i16vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::i16vec1
    void to_json(nlohmann::json& json, const i16vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0);
    }

    void from_json(const nlohmann::json& json, i16vec1& type)
    {
        utl::parse_to("x", json, type.x);
    }


    //glm::i64vec4
    void to_json(nlohmann::json& json, const i64vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ll);
        utl::serialize_from("y", json, type.y, type.y != 0ll);
        utl::serialize_from("z", json, type.z, type.z != 0ll);
        utl::serialize_from("w", json, type.w, type.w != 0ll);
    }

    void from_json(const nlohmann::json& json, i64vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::i64vec3
    void to_json(nlohmann::json& json, const i64vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ll);
        utl::serialize_from("y", json, type.y, type.y != 0ll);
        utl::serialize_from("z", json, type.z, type.z != 0ll);
    }

    void from_json(const nlohmann::json& json, i64vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::i64vec2
    void to_json(nlohmann::json& json, const i64vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ll);
        utl::serialize_from("y", json, type.y, type.y != 0ll);
    }

    void from_json(const nlohmann::json& json, i64vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::i64vec1
    void to_json(nlohmann::json& json, const i64vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ll);
    }

    void from_json(const nlohmann::json& json, i64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    /*--------------------GLM vectors uint--------------------*/
    //glm::uvec4
    void to_json(nlohmann::json& json, const uvec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
        utl::serialize_from("z", json, type.z, type.z != 0u);
        utl::serialize_from("w", json, type.w, type.w != 0u);
    }

    void from_json(const nlohmann::json& json, uvec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::uvec3
    void to_json(nlohmann::json& json, const uvec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
        utl::serialize_from("z", json, type.z, type.z != 0u);
    }

    void from_json(const nlohmann::json& json, uvec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::uvec2
    void to_json(nlohmann::json& json, const uvec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
    }

    void from_json(const nlohmann::json& json, uvec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::uvec1
    void to_json(nlohmann::json& json, const uvec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
    }

    void from_json(const nlohmann::json& json, uvec1& type)
    {
        utl::parse_to("x", json, type.x);
    }


    //glm::u8vec4
    void to_json(nlohmann::json& json, const u8vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
        utl::serialize_from("z", json, type.z, type.z != 0u);
        utl::serialize_from("w", json, type.w, type.w != 0u);
    }

    void from_json(const nlohmann::json& json, u8vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::u8vec3
    void to_json(nlohmann::json& json, const u8vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
        utl::serialize_from("z", json, type.z, type.z != 0u);
    }

    void from_json(const nlohmann::json& json, u8vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::u8vec2
    void to_json(nlohmann::json& json, const u8vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
    }

    void from_json(const nlohmann::json& json, u8vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::u8vec1
    void to_json(nlohmann::json& json, const u8vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
    }

    void from_json(const nlohmann::json& json, u8vec1& type)
    {
        utl::parse_to("x", json, type.x);
    }


    //glm::u16vec4
    void to_json(nlohmann::json& json, const u16vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
        utl::serialize_from("z", json, type.z, type.z != 0u);
        utl::serialize_from("w", json, type.w, type.w != 0u);
    }

    void from_json(const nlohmann::json& json, u16vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }

    //glm::u16vec3
    void to_json(nlohmann::json& json, const u16vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
        utl::serialize_from("z", json, type.z, type.z != 0u);
    }

    void from_json(const nlohmann::json& json, u16vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::u16vec2
    void to_json(nlohmann::json& json, const u16vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
        utl::serialize_from("y", json, type.y, type.y != 0u);
    }

    void from_json(const nlohmann::json& json, u16vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::u16vec1
    void to_json(nlohmann::json& json, const u16vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0u);
    }

    void from_json(const nlohmann::json& json, u16vec1& type)
    {
        utl::parse_to("x", json, type.x);
    }


    //glm::u64vec4
    void to_json(nlohmann::json& json, const u64vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ull);
        utl::serialize_from("y", json, type.y, type.y != 0ull);
        utl::serialize_from("z", json, type.z, type.z != 0ull);
        utl::serialize_from("w", json, type.w, type.w != 0ull);
    }

    void from_json(const nlohmann::json& json, u64vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }


    //glm::u64vec3
    void to_json(nlohmann::json& json, const u64vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ull);
        utl::serialize_from("y", json, type.y, type.y != 0ull);
        utl::serialize_from("z", json, type.z, type.z != 0ull);
    }

    void from_json(const nlohmann::json& json, u64vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }


    //glm::u64vec2
    void to_json(nlohmann::json& json, const u64vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ull);
        utl::serialize_from("y", json, type.y, type.y != 0ull);
    }

    void from_json(const nlohmann::json& json, u64vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }


    //glm::u64vec1
    void to_json(nlohmann::json& json, const u64vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0ull);
    }

    void from_json(const nlohmann::json& json, u64vec1& type)
    {
        utl::parse_to("x", json, type.x);
    }


    /*--------------------GLM vectors float--------------------*/
    //glm::vec4
    void to_json(nlohmann::json& json, const vec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.f);
        utl::serialize_from("y", json, type.y, type.y != 0.f);
        utl::serialize_from("z", json, type.z, type.z != 0.f);
        utl::serialize_from("w", json, type.w, type.w != 0.f);
    }

    void from_json(const nlohmann::json& json, vec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }

    //glm::vec3
    void to_json(nlohmann::json& json, const vec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.f);
        utl::serialize_from("y", json, type.y, type.y != 0.f);
        utl::serialize_from("z", json, type.z, type.z != 0.f);
    }

    void from_json(const nlohmann::json& json, vec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }

    //glm::vec2
    void to_json(nlohmann::json& json, const vec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.f);
        utl::serialize_from("y", json, type.y, type.y != 0.f);
    }

    void from_json(const nlohmann::json& json, vec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }

    //glm::vec1
    void to_json(nlohmann::json& json, const vec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.f);
    }

    void from_json(const nlohmann::json& json, vec1& type)
    {
        utl::parse_to("x", json, type.x);
    }

    //glm::dvec4
    void to_json(nlohmann::json& json, const dvec4& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.0);
        utl::serialize_from("y", json, type.y, type.y != 0.0);
        utl::serialize_from("z", json, type.z, type.z != 0.0);
        utl::serialize_from("w", json, type.w, type.w != 0.0);
    }

    void from_json(const nlohmann::json& json, dvec4& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }

    //glm::dvec3
    void to_json(nlohmann::json& json, const dvec3& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.0);
        utl::serialize_from("y", json, type.y, type.y != 0.0);
        utl::serialize_from("z", json, type.z, type.z != 0.0);
    }

    void from_json(const nlohmann::json& json, dvec3& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
    }

    //glm::dvec2
    void to_json(nlohmann::json& json, const dvec2& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.0);
        utl::serialize_from("y", json, type.y, type.y != 0.0);
    }

    void from_json(const nlohmann::json& json, dvec2& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
    }

    //glm::dvec1
    void to_json(nlohmann::json& json, const dvec1& type)
    {
        json = nlohmann::json();
        utl::serialize_from("x", json, type.x, type.x != 0.0);
    }

    void from_json(const nlohmann::json& json, dvec1& type)
    {
        utl::parse_to("x", json, type.x);
    }

    void to_json(nlohmann::json& json, const quat& type)
    {
        utl::serialize_from("x", json, type.x, type.x != 0.f);
        utl::serialize_from("y", json, type.y, type.y != 0.f);
        utl::serialize_from("z", json, type.z, type.z != 0.f);
        utl::serialize_from("w", json, type.w, type.w != 1.f);
    }

    void from_json(const nlohmann::json& json, quat& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }

    void to_json(nlohmann::json& json, const dquat& type)
    {
        utl::serialize_from("x", json, type.x, type.x != 0.0);
        utl::serialize_from("y", json, type.y, type.y != 0.0);
        utl::serialize_from("z", json, type.z, type.z != 0.0);
        utl::serialize_from("w", json, type.w, type.w != 1.0);
    }

    void from_json(const nlohmann::json& json, dquat& type)
    {
        utl::parse_to("x", json, type.x);
        utl::parse_to("y", json, type.y);
        utl::parse_to("z", json, type.z);
        utl::parse_to("w", json, type.w);
    }
}