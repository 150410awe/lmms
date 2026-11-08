#ifndef LMNS_SRC_LOGGER_LOGGER_IMPL_HPP
#define LMNS_SRC_LOGGER_LOGGER_IMPL_HPP

#include <memory>
#include <shared_mutex>
#include <string_view>

#include <lmns/logger/appender.hpp>
#include <lmns/logger/level.hpp>
#include <lmns/logger/logger.hpp>

#include "logger/formatter.hpp"

namespace lmns {

class Logger::Impl {
public:
  explicit Impl(std::string_view name);
  ~Impl() = default;

  [[nodiscard]] bool should_log(Log_level::Level level) const noexcept;
  void submit(const detail::Log_record_view &record) noexcept;

  void set_level(Log_level::Level level) noexcept;
  [[nodiscard]] Log_level::Level get_level() const noexcept;

  [[nodiscard]] std::string_view get_name() const noexcept;

  void set_formatter(std::shared_ptr<const detail::Formatter> formatter);
  [[nodiscard]] std::shared_ptr<const detail::Formatter>
  get_formatter() const noexcept;

  void add_appender(Appender_ptr appender);
  void remove_appender(const Appender_ptr &appender);
  void clear_appenders();

  void flush();
  void sync();

  void set_root(Logger_ptr root);
private:
  std::string name;
  std::atomic<Log_level::Level> level{Log_level::Level::DEBUG};
  mutable std::shared_mutex mutex;
  std::shared_ptr<const detail::Formatter> formatter;
  std::vector<Appender_ptr> appenders;
  Logger_ptr root;
};

namespace detail {

class Logger_access final {
public:
  static void submit(Logger &logger, const Log_record_view &record) noexcept;
  static void set_root(Logger &logger, Logger_ptr root);
};

} // namespace detail

} // namespace lmns

#endif