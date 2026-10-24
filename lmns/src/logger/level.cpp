#include <lmns/logger/level.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <string>

namespace lmns
{

    std::optional<std::string_view> log_level::to_string(level value) noexcept
    {
        switch (value)
        {
        case level::DEBUG:
            return "DEBUG";
        case level::INFO:
            return "INFO";
        case level::WARN:
            return "WARN";
        case level::ERROR:
            return "ERROR";
        case level::FATAL:
            return "FATAL";
        case level::OFF:
            return "OFF";
        default:
            return std::nullopt;
        }
    }
    std::optional<log_level::level> log_level::from_string(std::string_view value) noexcept
    {
        if (value.size() > 6)
            return std::nullopt;
        std::array<char, 6> normalized{};
        std::transform(value.begin(), value.end(), normalized.begin(), [](unsigned char character)
                       { return static_cast<char>(std::toupper(character)); });
        const std::string_view upper_value{normalized.data(), value.size()};
        if (upper_value == "DEBUG")
            return level::DEBUG;
        if (upper_value == "INFO")
            return level::INFO;
        if (upper_value == "WARN")
            return level::WARN;
        if (upper_value == "ERROR")
            return level::ERROR;
        if (upper_value == "FATAL")
            return level::FATAL;
        if (upper_value == "OFF")
            return level::OFF;
        return std::nullopt;
    }
}
