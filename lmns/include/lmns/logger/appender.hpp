#ifndef LMNS_INCLUDE_LMNS_LOGGER_APPENDER_HPP
#define LMNS_INCLUDE_LMNS_LOGGER_APPENDER_HPP

#include <filesystem>
#include <lmns/export.hpp>
#include <lmns/logger/level.hpp>
#include <memory>


namespace lmns {
namespace detail {
class Appender_access;
}

class LMNS_API Appender {
public:
  class Impl;
  ~Appender() = default;

  Appender(const Appender &) = delete;
  Appender(Appender &&) = delete;
  Appender &operator=(const Appender &) = delete;
  Appender &operator=(Appender &&) = delete;

  void set_level(const Log_level::Level level) noexcept;
  [[nodiscard]] Log_level::Level get_level() const noexcept;

  void flush();
  void sync();

private:
  explicit Appender(std::unique_ptr<Impl> impl) noexcept;
  std::unique_ptr<Impl> impl;
  friend class detail::Appender_access;
};
using Appender_ptr = std::shared_ptr<Appender>;

[[nodiscard]] LMNS_API Appender_ptr make_stdout_appender();
[[nodiscard]] LMNS_API Appender_ptr
make_file_appender(std::filesystem::path path);

} // namespace lmns

#endif