#include "lua_bindings.h"

#include <glm/glm.hpp>

template<class _Ty, class _Kty>
auto make_vec_add()
{
    return sol::overload(
        [](const _Ty& v1, const _Ty& v2) -> _Ty { return v1 + v2; },
        [](const _Ty& v1, _Kty f) -> _Ty { return v1 + f; },
        [](_Kty f, const _Ty& v1) -> _Ty { return f + v1; }
    );
}

template<class _Ty, class _Kty>
auto make_vec_sub()
{
    return sol::overload(
        [](const _Ty& v1, const _Ty& v2) -> _Ty { return v1 - v2; },
        [](const _Ty& v1, _Kty f) -> _Ty { return v1 - f; },
        [](_Kty f, const _Ty& v1) -> _Ty { return f - v1; }
    );
}

template<class _Ty, class _Kty>
auto make_vec_mult()
{
    return sol::overload(
        [](const _Ty& v1, const _Ty& v2) -> _Ty { return v1 * v2; },
        [](const _Ty& v1, _Kty f) -> _Ty { return v1 * f; },
        [](_Kty f, const _Ty& v1) -> _Ty { return f * v1; }
    );
}

template<class _Ty, class _Kty>
auto make_vec_div()
{
    return sol::overload(
        [](const _Ty& v1, const _Ty& v2) -> _Ty { return v1 / v2; },
        [](const _Ty& v1, _Kty f) -> _Ty { return v1 / f; },
        [](_Kty f, const _Ty& v1) -> _Ty { return f / v1; }
    );
}

template<class _TargetType, class _NumType>
void register_vec1(const std::string& name, sol::state& lua)
{
   lua.new_usertype<_TargetType>(
       name.c_str(),
       sol::constructors<
       _TargetType(), _TargetType(_NumType),
       _TargetType(_TargetType)
       >(),
       sol::meta_function::addition, make_vec_add<_TargetType, _NumType>(),
       sol::meta_function::subtraction, make_vec_sub<_TargetType, _NumType>(),
       sol::meta_function::multiplication, make_vec_mult<_TargetType, _NumType>(),
       sol::meta_function::division, make_vec_div<_TargetType, _NumType>(),
       "x", &_TargetType::x
       );
}

template<class _TargetType, class _Vec1Type, class _NumType>
void register_vec2(const std::string& name, sol::state& lua)
{
    lua.new_usertype<_TargetType>(
        name.c_str(),
        sol::constructors<
        _TargetType(), _TargetType(_NumType), _TargetType(_NumType, _NumType),
        _TargetType(_TargetType), _TargetType(_Vec1Type), _TargetType(_Vec1Type, _Vec1Type),
        _TargetType(_Vec1Type, _NumType), _TargetType(_NumType, _Vec1Type)
        >(),
        sol::meta_function::addition, make_vec_add<_TargetType, _NumType>(),
        sol::meta_function::subtraction, make_vec_sub<_TargetType, _NumType>(),
        sol::meta_function::multiplication, make_vec_mult<_TargetType, _NumType>(),
        sol::meta_function::division, make_vec_div<_TargetType, _NumType>(),
        "x", &_TargetType::x,
        "y", &_TargetType::y
        );
}

template<class _TargetType, class _Vec1Type, class _Vec2Type, class _NumType>
void register_vec3(const std::string& name, sol::state& lua)
{
    lua.new_usertype<_TargetType>(
        name.c_str(),
        sol::constructors<
        _TargetType(), _TargetType(_NumType), _TargetType(_NumType, _NumType, _NumType),
        _TargetType(_TargetType), _TargetType(_Vec1Type), _TargetType(_Vec1Type, _Vec1Type, _Vec1Type),
        _TargetType(_Vec2Type, _NumType), _TargetType(_NumType, _Vec2Type)
        >(),
        sol::meta_function::addition, make_vec_add<_TargetType, _NumType>(),
        sol::meta_function::subtraction, make_vec_sub<_TargetType, _NumType>(),
        sol::meta_function::multiplication, make_vec_mult<_TargetType, _NumType>(),
        sol::meta_function::division, make_vec_div<_TargetType, _NumType>(),
        "x", &_TargetType::x,
        "y", &_TargetType::y,
        "z", &_TargetType::z
        );
}

template<class _TargetType, class _Vec1Type, class _Vec2Type, class _Vec3Type, class _NumType>
void register_vec4(const std::string& name, sol::state& lua)
{
    lua.new_usertype<_TargetType>(
        name.c_str(),
        sol::constructors<
        _TargetType(), _TargetType(_NumType), _TargetType(_NumType, _NumType, _NumType, _NumType),
        _TargetType(_TargetType), _TargetType(_Vec1Type), _TargetType(_Vec1Type, _Vec1Type, _Vec1Type, _Vec1Type),
        _TargetType(_Vec2Type, _NumType, _NumType), _TargetType(_NumType, _Vec2Type, _NumType), _TargetType(_NumType, _NumType, _Vec2Type), _TargetType(_Vec2Type, _Vec2Type),
        _TargetType(_Vec3Type, _NumType), _TargetType(_NumType, _Vec3Type)
        >(),
        sol::meta_function::addition, make_vec_add<_TargetType, _NumType>(),
        sol::meta_function::subtraction, make_vec_sub<_TargetType, _NumType>(),
        sol::meta_function::multiplication, make_vec_mult<_TargetType, _NumType>(),
        sol::meta_function::division, make_vec_div<_TargetType, _NumType>(),
        "x", &_TargetType::x,
        "y", &_TargetType::y,
        "z", &_TargetType::z,
        "w", &_TargetType::w
        );
}

namespace engine
{
    namespace scripting
    {
        void bind_glm_lib(sol::state& lua)
        {
            // Bool 
            
            // Integer signed
            //register_vec1<glm::ivec1, int32_t>("ivec1", lua);
            //register_vec2<glm::ivec2, glm::ivec1, int32_t>("ivec2", lua);
            //register_vec3<glm::ivec3, glm::ivec1, glm::ivec2, int32_t>("ivec3", lua);
            //register_vec4<glm::ivec4, glm::ivec1, glm::ivec2, glm::ivec3, int32_t>("ivec4", lua);
            //
            //// Integer unsigned
            //register_vec1<glm::uvec1, uint32_t>("uvec1", lua);
            //register_vec2<glm::uvec2, glm::uvec1, uint32_t>("uvec2", lua);
            //register_vec3<glm::uvec3, glm::uvec1, glm::uvec2, uint32_t>("uvec3", lua);
            //register_vec4<glm::uvec4, glm::uvec1, glm::uvec2, glm::uvec3, uint32_t>("uvec4", lua);
            //
            //// Floating point
            //register_vec1<glm::vec1, float>("vec1", lua);
            //register_vec2<glm::vec2, glm::vec1, float>("vec2", lua);
            //register_vec3<glm::vec3, glm::vec1, glm::vec2, float>("vec3", lua);
            //register_vec4<glm::vec4, glm::vec1, glm::vec2, glm::vec3, float>("vec4", lua);
            //
            //// Double
            //register_vec1<glm::dvec1, double>("dvec1", lua);
            //register_vec2<glm::dvec2, glm::dvec1, double>("dvec2", lua);
            //register_vec3<glm::dvec3, glm::dvec1, glm::dvec2, double>("dvec3", lua);
            //register_vec4<glm::dvec4, glm::dvec1, glm::dvec2, glm::dvec3, double>("dvec4", lua);
            //
            //// Matrices
            //lua.new_usertype<glm::mat2>(
            //    "mat2",
            //    sol::constructors<
            //    glm::mat2(), glm::mat2(float), glm::mat2(float, float, float, float),
            //    glm::mat2(glm::mat2),
            //    //glm::mat2(glm::vec1, glm::vec1, glm::vec1, glm::vec1),
            //    glm::mat2(glm::vec2, glm::vec2)
            //    >(),
            //    sol::meta_function::addition, make_vec_add<glm::mat2, float>(),
            //    sol::meta_function::subtraction, make_vec_sub<glm::mat2, float>(),
            //    sol::meta_function::multiplication, make_vec_mult<glm::mat2, float>(),
            //    sol::meta_function::division, make_vec_div<glm::mat2, float>()
            //    );
            //
            //lua.new_usertype<glm::mat3>(
            //    "mat3",
            //    sol::constructors<
            //    glm::mat3(), glm::mat3(float), glm::mat3(float, float, float, float, float, float, float, float, float),
            //    glm::mat3(glm::mat3),
            //    glm::mat3(glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1),
            //    glm::mat3(glm::vec3, glm::vec3, glm::vec3)
            //    >(),
            //    sol::meta_function::addition, make_vec_add<glm::mat3, float>(),
            //    sol::meta_function::subtraction, make_vec_sub<glm::mat3, float>(),
            //    sol::meta_function::multiplication, make_vec_mult<glm::mat3, float>(),
            //    sol::meta_function::division, make_vec_div<glm::mat3, float>()
            //    );
            //
            //lua.new_usertype<glm::mat4>(
            //    "mat4",
            //    sol::constructors<
            //    glm::mat4(), glm::mat4(float), glm::mat4(float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float),
            //    glm::mat4(glm::mat4),
            //    //glm::mat4(glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1, glm::vec1),
            //    glm::mat4(glm::vec4, glm::vec4, glm::vec4, glm::vec4)
            //    >(),
            //    sol::meta_function::addition, make_vec_add<glm::mat4, float>(),
            //    sol::meta_function::subtraction, make_vec_sub<glm::mat4, float>(),
            //    sol::meta_function::multiplication, make_vec_mult<glm::mat4, float>(),
            //    sol::meta_function::division, make_vec_div<glm::mat4, float>()
            //    );
        }
    }
}

