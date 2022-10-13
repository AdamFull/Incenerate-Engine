#pragma once

#include <source_location>
#include <string>
#include <format>
#include <chrono>
#include <string_view>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace utl
{
    enum class level
    {
        eVerbose,
        eInfo,
        eWarning,
        eError
    };

    namespace colorize
    {
        inline std::string black(const std::string& input) { return "\x1B[30m" + input + "\033[0m\t\t"; }
        inline std::string red(const std::string& input) { return "\x1B[31m" + input + "\033[0m\t\t"; }
        inline std::string green(const std::string& input) { return "\x1B[32m" + input + "\033[0m\t\t"; }
        inline std::string yellow(const std::string& input) { return "\x1B[33m" + input + "\033[0m\t\t"; }
        inline std::string blue(const std::string& input) { return "\x1B[34m" + input + "\033[0m\t\t"; }
        inline std::string magenta(const std::string& input) { return "\x1B[35m" + input + "\033[0m\t\t"; }
        inline std::string cyan(const std::string& input) { return "\x1B[36m" + input + "\033[0m\t\t"; }
        inline std::string white(const std::string& input) { return "\x1B[37m" + input + "\033[0m\t\t"; }

        inline std::string br_black(const std::string& input) { return "\x1B[90m" + input + "\033[0m\t\t"; }
        inline std::string br_red(const std::string& input) { return "\x1B[91m" + input + "\033[0m\t\t"; }
        inline std::string br_green(const std::string& input) { return "\x1B[92m" + input + "\033[0m\t\t"; }
        inline std::string br_yellow(const std::string& input) { return "\x1B[93m" + input + "\033[0m\t\t"; }
        inline std::string br_blue(const std::string& input) { return "\x1B[94m" + input + "\033[0m\t\t"; }
        inline std::string br_magenta(const std::string& input) { return "\x1B[95m" + input + "\033[0m\t\t"; }
        inline std::string br_cyan(const std::string& input) { return "\x1B[96m" + input + "\033[0m\t\t"; }
        inline std::string br_white(const std::string& input) { return "\x1B[97m" + input + "\033[0m\t\t"; }
    }

    template<level _level, class... _Args>
    struct log
    {
        constexpr log(const std::string_view fmt, _Args&&... args, std::source_location&& loc = std::source_location::current())
        {
            auto _time = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
            auto _formatted_time = std::format("{:%F %T %Z}", _time);

            auto _base = std::format("[{}] {} {}:{}:{}", get_level<_level>(), _formatted_time, 
            loc.file_name(), loc.function_name(), loc.line());

            auto _user_log = std::vformat(fmt, std::make_format_args(std::forward<_Args>(args)...));

            auto _log = std::format("{} -> {}", _base, _user_log);
            _log = colorize<_level>(_log);

            std::cout << _log << std::endl;

#ifdef _WIN32
            OutputDebugString(_log.c_str());
#endif
        }

    private:
        template<level _level>
        constexpr std::string colorize(const std::string& input)
        {
            switch (_level)
            {
            case level::eVerbose: return colorize::white(input);
            case level::eInfo: return colorize::green(input);
            case level::eWarning: return colorize::yellow(input);
            case level::eError: return colorize::red(input);
            }

            return input;
        }

        template<level _level>
        constexpr const char* get_level()
        {
            switch (_level)
            {
            case level::eVerbose: return "verbose";
            case level::eInfo: return "info";
            case level::eWarning: return "warning";
            case level::eError: return "error";
            }
            return "none";
        }
    };

    template<level _level = level::eVerbose, class... _Args>
    log(std::string_view const, _Args&&...) -> log<_level, _Args...>;
}