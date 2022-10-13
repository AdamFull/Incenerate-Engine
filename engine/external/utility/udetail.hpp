#pragma once

#include <string_view>

namespace utl
{
    template <typename T> constexpr std::string_view type_name();

    template <>
    constexpr std::string_view type_name<void>() { return "void"; }

    namespace detail 
    {
        using type_name_prober = void;

        template <typename T>
        constexpr std::string_view wrapped_type_name() 
        {
        #ifdef __clang__
            return __PRETTY_FUNCTION__;
        #elif defined(__GNUC__)
            return __PRETTY_FUNCTION__;
        #elif defined(_MSC_VER)
            return __FUNCSIG__;
        #else
        #error "Unsupported compiler"
        #endif
        }

        constexpr std::size_t wrapped_type_name_prefix_length() { 
            return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>()); 
        }

        constexpr std::size_t wrapped_type_name_suffix_length() { 
            return wrapped_type_name<type_name_prober>().length() 
                - wrapped_type_name_prefix_length() 
                - type_name<type_name_prober>().length();
        }

    } // namespace detail

    template <class _Ty>
    constexpr std::string_view type_name() 
    {
        constexpr auto wrapped_name = detail::wrapped_type_name<_Ty>();
        constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
        constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
        constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
        return wrapped_name.substr(prefix_length, type_name_length);
    }
}