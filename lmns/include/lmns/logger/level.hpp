#ifndef LMNS_INCLUDE_LMNS_LOGGER_LEVEL_HPP
#define LMNS_INCLUDE_LMNS_LOGGER_LEVEL_HPP

#include <string_view>
#include <cstdint>
#include <optional>

#include <lmns/export.hpp>

namespace lmns {
    class LMNS_API log_level final {
    public:
        enum class level : std::int8_t {
            DEBUG = 0,
            INFO = 1,
            WARN = 2,
            ERROR = 3,
            FATAL = 4,
            OFF = 5,
        };
        [[nodiscard]] static std::optional<std::string_view> to_string(level value) noexcept;
        [[nodiscard]] static std::optional<level> from_string(std::string_view value) noexcept;
    };
}

#endif

