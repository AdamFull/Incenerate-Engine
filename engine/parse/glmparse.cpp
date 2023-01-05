#include "glmparse.h"

namespace glm
{
    /*--------------------GLM vectors bool--------------------*/
    //glm::lowp_bvec4
    void to_json(nlohmann::json& json, const lowp_bvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, lowp_bvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_bvec3
    void to_json(nlohmann::json& json, const lowp_bvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
        };
    }

    void from_json(const nlohmann::json& json, lowp_bvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_bvec2
    void to_json(nlohmann::json& json, const lowp_bvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
        };
    }

    void from_json(const nlohmann::json& json, lowp_bvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_bvec1
    void to_json(nlohmann::json& json, const lowp_bvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
        };
    }

    void from_json(const nlohmann::json& json, lowp_bvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_bvec4
    void to_json(nlohmann::json& json, const mediump_bvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, mediump_bvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_bvec3
    void to_json(nlohmann::json& json, const mediump_bvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
        };
    }

    void from_json(const nlohmann::json& json, mediump_bvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_bvec2
    void to_json(nlohmann::json& json, const mediump_bvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
        };
    }

    void from_json(const nlohmann::json& json, mediump_bvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_bvec1
    void to_json(nlohmann::json& json, const mediump_bvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_bvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::bvec4
    void to_json(nlohmann::json& json, const bvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, bvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::bvec3
    void to_json(nlohmann::json& json, const bvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, bvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::bvec2
    void to_json(nlohmann::json& json, const bvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, bvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::bvec1
    void to_json(nlohmann::json& json, const bvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, bvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }

    /*--------------------GLM vectors int--------------------*/
    //glm::lowp_ivec4
    void to_json(nlohmann::json& json, const lowp_ivec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_ivec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_ivec3
    void to_json(nlohmann::json& json, const lowp_ivec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_ivec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_ivec2
    void to_json(nlohmann::json& json, const lowp_ivec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_ivec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_ivec1
    void to_json(nlohmann::json& json, const lowp_ivec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_ivec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_ivec4
    void to_json(nlohmann::json& json, const mediump_ivec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_ivec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_ivec3
    void to_json(nlohmann::json& json, const mediump_ivec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_ivec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_ivec2
    void to_json(nlohmann::json& json, const mediump_ivec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_ivec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_ivec1
    void to_json(nlohmann::json& json, const mediump_ivec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_ivec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::ivec4
    void to_json(nlohmann::json& json, const ivec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, ivec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::ivec3
    void to_json(nlohmann::json& json, const ivec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, ivec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::ivec2
    void to_json(nlohmann::json& json, const ivec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, ivec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::ivec1
    void to_json(nlohmann::json& json, const ivec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, ivec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_i8vec4
    void to_json(nlohmann::json& json, const lowp_i8vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i8vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_i8vec3
    void to_json(nlohmann::json& json, const lowp_i8vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i8vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_i8vec2
    void to_json(nlohmann::json& json, const lowp_i8vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i8vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_i8vec1
    void to_json(nlohmann::json& json, const lowp_i8vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i8vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_i8vec4
    void to_json(nlohmann::json& json, const mediump_i8vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i8vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_i8vec3
    void to_json(nlohmann::json& json, const mediump_i8vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i8vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_i8vec2
    void to_json(nlohmann::json& json, const mediump_i8vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i8vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_i8vec1
    void to_json(nlohmann::json& json, const mediump_i8vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i8vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::i8vec4
    void to_json(nlohmann::json& json, const i8vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, i8vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::i8vec3
    void to_json(nlohmann::json& json, const i8vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, i8vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::i8vec2
    void to_json(nlohmann::json& json, const i8vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, i8vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::i8vec1
    void to_json(nlohmann::json& json, const i8vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, i8vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_i16vec4
    void to_json(nlohmann::json& json, const lowp_i16vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i16vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_i16vec3
    void to_json(nlohmann::json& json, const lowp_i16vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i16vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_i16vec2
    void to_json(nlohmann::json& json, const lowp_i16vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i16vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_i16vec1
    void to_json(nlohmann::json& json, const lowp_i16vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i16vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_i16vec4
    void to_json(nlohmann::json& json, const mediump_i16vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i16vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_i16vec3
    void to_json(nlohmann::json& json, const mediump_i16vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i16vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_i16vec2
    void to_json(nlohmann::json& json, const mediump_i16vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i16vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_i16vec1
    void to_json(nlohmann::json& json, const mediump_i16vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i16vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::i16vec4
    void to_json(nlohmann::json& json, const i16vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, i16vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::i16vec3
    void to_json(nlohmann::json& json, const i16vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, i16vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::i16vec2
    void to_json(nlohmann::json& json, const i16vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, i16vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::i16vec1
    void to_json(nlohmann::json& json, const i16vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, i16vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_i64vec4
    void to_json(nlohmann::json& json, const lowp_i64vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i64vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_i64vec3
    void to_json(nlohmann::json& json, const lowp_i64vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i64vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_i64vec2
    void to_json(nlohmann::json& json, const lowp_i64vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i64vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_i64vec1
    void to_json(nlohmann::json& json, const lowp_i64vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_i64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_i64vec4
    void to_json(nlohmann::json& json, const mediump_i64vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i64vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_i64vec3
    void to_json(nlohmann::json& json, const mediump_i64vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i64vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_i64vec2
    void to_json(nlohmann::json& json, const mediump_i64vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i64vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_i64vec1
    void to_json(nlohmann::json& json, const mediump_i64vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_i64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::i64vec4
    void to_json(nlohmann::json& json, const i64vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, i64vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::i64vec3
    void to_json(nlohmann::json& json, const i64vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, i64vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::i64vec2
    void to_json(nlohmann::json& json, const i64vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, i64vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::i64vec1
    void to_json(nlohmann::json& json, const i64vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, i64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    /*--------------------GLM vectors uint--------------------*/
    //glm::lowp_uvec4
    void to_json(nlohmann::json& json, const lowp_uvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_uvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_uvec3
    void to_json(nlohmann::json& json, const lowp_uvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_uvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_uvec2
    void to_json(nlohmann::json& json, const lowp_uvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_uvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_uvec1
    void to_json(nlohmann::json& json, const lowp_uvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_uvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_uvec4
    void to_json(nlohmann::json& json, const mediump_uvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_uvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_uvec3
    void to_json(nlohmann::json& json, const mediump_uvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_uvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_uvec2
    void to_json(nlohmann::json& json, const mediump_uvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_uvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_uvec1
    void to_json(nlohmann::json& json, const mediump_uvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_uvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::uvec4
    void to_json(nlohmann::json& json, const uvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, uvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::uvec3
    void to_json(nlohmann::json& json, const uvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, uvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::uvec2
    void to_json(nlohmann::json& json, const uvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, uvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::uvec1
    void to_json(nlohmann::json& json, const uvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, uvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_u8vec4
    void to_json(nlohmann::json& json, const lowp_u8vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u8vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_u8vec3
    void to_json(nlohmann::json& json, const lowp_u8vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u8vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_u8vec2
    void to_json(nlohmann::json& json, const lowp_u8vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u8vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_u8vec1
    void to_json(nlohmann::json& json, const lowp_u8vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u8vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_u8vec4
    void to_json(nlohmann::json& json, const mediump_u8vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u8vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_u8vec3
    void to_json(nlohmann::json& json, const mediump_u8vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u8vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_u8vec2
    void to_json(nlohmann::json& json, const mediump_u8vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u8vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_u8vec1
    void to_json(nlohmann::json& json, const mediump_u8vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u8vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::u8vec4
    void to_json(nlohmann::json& json, const u8vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, u8vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::u8vec3
    void to_json(nlohmann::json& json, const u8vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, u8vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::u8vec2
    void to_json(nlohmann::json& json, const u8vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, u8vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::u8vec1
    void to_json(nlohmann::json& json, const u8vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, u8vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_u16vec4
    void to_json(nlohmann::json& json, const lowp_u16vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u16vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_u16vec3
    void to_json(nlohmann::json& json, const lowp_u16vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u16vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_u16vec2
    void to_json(nlohmann::json& json, const lowp_u16vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u16vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_u16vec1
    void to_json(nlohmann::json& json, const lowp_u16vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u16vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_u16vec4
    void to_json(nlohmann::json& json, const mediump_u16vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u16vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_u16vec3
    void to_json(nlohmann::json& json, const mediump_u16vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u16vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_u16vec2
    void to_json(nlohmann::json& json, const mediump_u16vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u16vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_u16vec1
    void to_json(nlohmann::json& json, const mediump_u16vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u16vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::u16vec4
    void to_json(nlohmann::json& json, const u16vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, u16vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::u16vec3
    void to_json(nlohmann::json& json, const u16vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, u16vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::u16vec2
    void to_json(nlohmann::json& json, const u16vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, u16vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::u16vec1
    void to_json(nlohmann::json& json, const u16vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, u16vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_u64vec4
    void to_json(nlohmann::json& json, const lowp_u64vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u64vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::lowp_u64vec3
    void to_json(nlohmann::json& json, const lowp_u64vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u64vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::lowp_u64vec2
    void to_json(nlohmann::json& json, const lowp_u64vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u64vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::lowp_u64vec1
    void to_json(nlohmann::json& json, const lowp_u64vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_u64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_u64vec4
    void to_json(nlohmann::json& json, const mediump_u64vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u64vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::mediump_u64vec3
    void to_json(nlohmann::json& json, const mediump_u64vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u64vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::mediump_u64vec2
    void to_json(nlohmann::json& json, const mediump_u64vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u64vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::mediump_u64vec1
    void to_json(nlohmann::json& json, const mediump_u64vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_u64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::u64vec4
    void to_json(nlohmann::json& json, const u64vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, u64vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }


    //glm::u64vec3
    void to_json(nlohmann::json& json, const u64vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, u64vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }


    //glm::u64vec2
    void to_json(nlohmann::json& json, const u64vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, u64vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }


    //glm::u64vec1
    void to_json(nlohmann::json& json, const u64vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, u64vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    /*--------------------GLM vectors float--------------------*/
    //glm::lowp_vec4
    void to_json(nlohmann::json& json, const lowp_vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w}
        };
    }

    void from_json(const nlohmann::json& json, lowp_vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::lowp_vec3
    void to_json(nlohmann::json& json, const lowp_vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }

    //glm::lowp_vec2
    void to_json(nlohmann::json& json, const lowp_vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }

    //glm::lowp_vec1
    void to_json(nlohmann::json& json, const lowp_vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_vec4
    void to_json(nlohmann::json& json, const mediump_vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, mediump_vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::mediump_vec3
    void to_json(nlohmann::json& json, const mediump_vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }

    //glm::mediump_vec2
    void to_json(nlohmann::json& json, const mediump_vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }

    //glm::mediump_vec1
    void to_json(nlohmann::json& json, const mediump_vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::vec4
    void to_json(nlohmann::json& json, const vec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, vec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::vec3
    void to_json(nlohmann::json& json, const vec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, vec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }

    //glm::vec2
    void to_json(nlohmann::json& json, const vec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, vec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }

    //glm::vec1
    void to_json(nlohmann::json& json, const vec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, vec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::lowp_dvec4
    void to_json(nlohmann::json& json, const lowp_dvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, lowp_dvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::lowp_dvec3
    void to_json(nlohmann::json& json, const lowp_dvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, lowp_dvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }

    //glm::lowp_dvec2
    void to_json(nlohmann::json& json, const lowp_dvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, lowp_dvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }

    //glm::lowp_dvec1
    void to_json(nlohmann::json& json, const lowp_dvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, lowp_dvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::mediump_dvec4
    void to_json(nlohmann::json& json, const mediump_dvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, mediump_dvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::mediump_dvec3
    void to_json(nlohmann::json& json, const mediump_dvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, mediump_dvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }

    //glm::mediump_dvec2
    void to_json(nlohmann::json& json, const mediump_dvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, mediump_dvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }

    //glm::mediump_dvec1
    void to_json(nlohmann::json& json, const mediump_dvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, mediump_dvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }


    //glm::dvec4
    void to_json(nlohmann::json& json, const dvec4& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z},
            {"w", type.w},
        };
    }

    void from_json(const nlohmann::json& json, dvec4& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
        if (json.find("w") != json.end())
            json.at("w").get_to(type.w);
        else
            type.w = 1.0;
    }

    //glm::dvec3
    void to_json(nlohmann::json& json, const dvec3& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"z", type.z}
        };
    }

    void from_json(const nlohmann::json& json, dvec3& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
        if (json.find("z") != json.end())
            json.at("z").get_to(type.z);
        else
            type.z = 1.0;
    }

    //glm::dvec2
    void to_json(nlohmann::json& json, const dvec2& type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y}
        };
    }

    void from_json(const nlohmann::json& json, dvec2& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
        if (json.find("y") != json.end())
            json.at("y").get_to(type.y);
        else
            type.y = 1.0;
    }

    //glm::dvec1
    void to_json(nlohmann::json& json, const dvec1& type)
    {
        json = nlohmann::json
        {
            {"x", type.x}
        };
    }

    void from_json(const nlohmann::json& json, dvec1& type)
    {
        if (json.find("x") != json.end())
            json.at("x").get_to(type.x);
        else
            type.x = 1.0;
    }

    /*--------------------GLM matrices--------------------*/
    //glm::lowp_mat4
    void to_json(nlohmann::json& json, const lowp_mat4& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]},
            {"v3", type[3]}
        };
    }

    void from_json(const nlohmann::json& json, lowp_mat4& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
        if (json.find("v3") != json.end())
            json.at("v3").get_to(type[3]);
    }


    //glm::lowp_mat3
    void to_json(nlohmann::json& json, const lowp_mat3& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]}
        };
    }

    void from_json(const nlohmann::json& json, lowp_mat3& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
    }


    //glm::lowp_mat2
    void to_json(nlohmann::json& json, const lowp_mat2& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]}
        };
    }

    void from_json(const nlohmann::json& json, lowp_mat2& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
    }


    //glm::mediump_mat4
    void to_json(nlohmann::json& json, const mediump_mat4& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]},
            {"v3", type[3]}
        };
    }

    void from_json(const nlohmann::json& json, mediump_mat4& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
        if (json.find("v3") != json.end())
            json.at("v3").get_to(type[3]);
    }


    //glm::mediump_mat3
    void to_json(nlohmann::json& json, const mediump_mat3& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]}
        };
    }

    void from_json(const nlohmann::json& json, mediump_mat3& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
    }


    //glm::mediump_mat2
    void to_json(nlohmann::json& json, const mediump_mat2& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]}
        };
    }

    void from_json(const nlohmann::json& json, mediump_mat2& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
    }


    //glm::mat4
    void to_json(nlohmann::json& json, const mat4& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]},
            {"v3", type[3]},
        };
    }

    void from_json(const nlohmann::json& json, mat4& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
        if (json.find("v3") != json.end())
            json.at("v3").get_to(type[3]);
    }


    //glm::mat3
    void to_json(nlohmann::json& json, const mat3& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]}
        };
    }

    void from_json(const nlohmann::json& json, mat3& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
    }


    //glm::mat2
    void to_json(nlohmann::json& json, const mat2& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]}
        };
    }

    void from_json(const nlohmann::json& json, mat2& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
    }


    //glm::lowp_dmat4
    void to_json(nlohmann::json& json, const lowp_dmat4& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]},
            {"v3", type[3]}
        };
    }

    void from_json(const nlohmann::json& json, lowp_dmat4& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
        if (json.find("v3") != json.end())
            json.at("v3").get_to(type[3]);
    }


    //glm::lowp_dmat3
    void to_json(nlohmann::json& json, const lowp_dmat3& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]}
        };
    }

    void from_json(const nlohmann::json& json, lowp_dmat3& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
    }


    //glm::lowp_dmat2
    void to_json(nlohmann::json& json, const lowp_dmat2& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]}
        };
    }

    void from_json(const nlohmann::json& json, lowp_dmat2& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
    }



    //glm::mediump_dmat4
    void to_json(nlohmann::json& json, const mediump_dmat4& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]},
            {"v3", type[3]}
        };
    }

    void from_json(const nlohmann::json& json, mediump_dmat4& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
        if (json.find("v3") != json.end())
            json.at("v3").get_to(type[3]);
    }


    //glm::mediump_dmat3
    void to_json(nlohmann::json& json, const mediump_dmat3& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]}
        };
    }

    void from_json(const nlohmann::json& json, mediump_dmat3& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
    }


    //glm::mediump_dmat2
    void to_json(nlohmann::json& json, const mediump_dmat2& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]}
        };
    }

    void from_json(const nlohmann::json& json, mediump_dmat2& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
    }


    //glm::dmat4
    void to_json(nlohmann::json& json, const dmat4& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]},
            {"v3", type[3]}
        };
    }

    void from_json(const nlohmann::json& json, dmat4& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
        if (json.find("v3") != json.end())
            json.at("v3").get_to(type[3]);
    }


    //glm::dmat3
    void to_json(nlohmann::json& json, const dmat3& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]},
            {"v2", type[2]}
        };
    }

    void from_json(const nlohmann::json& json, dmat3& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
        if (json.find("v2") != json.end())
            json.at("v2").get_to(type[2]);
    }


    //glm::dmat2
    void to_json(nlohmann::json& json, const dmat2& type)
    {
        json = nlohmann::json
        {
            {"v0", type[0]},
            {"v1", type[1]}
        };
    }

    void from_json(const nlohmann::json& json, dmat2& type)
    {
        if (json.find("v0") != json.end())
            json.at("v0").get_to(type[0]);
        if (json.find("v1") != json.end())
            json.at("v1").get_to(type[1]);
    }
}