#ifndef LMNS_INCLUDE_LMNS_LOGGER_LOGGER_HPP
#define LMNS_INCLUDE_LMNS_LOGGER_LOGGER_HPP

#include <memory>
#include <string_view>

#include <lmns/export.hpp>
#include <lmns/logger/appender.hpp>
#include <lmns/logger/level.hpp>

namespace lmns {

namespace detail {
class Logger_access;
}

class LMNS_API Logger final {
public:
  class Impl;
  explicit Logger(std::string_view name = "root");
  ~Logger();

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  // Check if the logger should log the given level.
  [[nodiscard]] bool should_log(Log_level::Level level) const noexcept;
  void set_level(Log_level::Level level) noexcept;
  [[nodiscard]] Log_level::Level get_level() const noexcept;
  [[nodiscard]] std::string_view get_name() const noexcept;

  void set_formatter(std::string_view pattern);
  [[nodiscard]] std::string_view get_formatter_pattern() const noexcept;

  void add_appender(Appender_ptr appender);
  void remove_appender(const Appender_ptr& appender);
  void clear_appenders();

  void flush();
  void sync();

private:
  std::unique_ptr<Impl> impl;
  friend class detail::Logger_access;
};

using Logger_ptr = std::shared_ptr<Logger>;
[[nodiscard]] LMNS_API Logger_ptr get_root_logger();
[[nodiscard]] LMNS_API Logger_ptr get_logger(std::string_view name);

} // namespace lmns

#endif