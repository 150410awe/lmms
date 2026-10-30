#ifndef LMNS_SRC_LOGGER_APPENDER_IMPL_HPP
#define LMNS_SRC_LOGGER_APPENDER_IMPL_HPP

#include <atomic>
#include <filesystem>
#include <mutex>
#include <string_view>

#include <lmns/logger/appender.hpp>
#include <lmns/logger/level.hpp>

namespace lmns {

class Appender::Impl {
public:
  virtual ~Impl() = default;
  void append(Log_level::Level level,
              std::string_view formatted_record) noexcept;
  void set_level(Log_level::Level level) noexcept;
  [[nodiscard]] Log_level::Level get_level() const noexcept;
  void flush();
  void sync();

protected:
  virtual void write_unlocked(std::string_view formatted_record) noexcept = 0;
  virtual void flush_unlocked() = 0;
  virtual void sync_unlocked() = 0;

  std::mutex mutex;

private:
  std::atomic<Log_level::Level> level{Log_level::Level::DEBUG};
};

namespace detail {
class Appender_access final {
public:
  [[nodiscard]] static Appender_ptr make_stdout_appender();
  [[nodiscard]] static Appender_ptr
  make_file_appender(std::filesystem::path path);
  static void append(const Appender_ptr &appender, Log_level::Level level,
                     std::string_view formatted_record) noexcept;
};
} // namespace detail

} // namespace lmns

#endif