#ifndef LMNS_SRC_LOGGER_FORMATTER_HPP
#define LMNS_SRC_LOGGER_FORMATTER_HPP

#include <chrono>
#include <string_view>

#include <lmns/logger/level.hpp>
#include <lmns/macros.hpp>

#include "logger/buffer.hpp"

namespace lmns::detail {

struct Log_record_view final {
  Log_level::Level level = Log_level::Level::DEBUG;
  std::string_view logger_name;                    // 日志器的名字
  std::string_view message;                        // 日志内容
  std::chrono::system_clock::time_point timestamp; // 时间戳
  std::chrono::steady_clock::duration elapsed;     // 起服已过时间
  u64 thread_id = 0;                               // 线程ID
  u64 fiber_id = 0;                                // 协程ID
  std::string_view thread_name;                    // 线程名
  std::string_view file_name;                      // 文件名
  std::uint32_t line = 0;                          // 行号
};

class Formatter final {
public:
  enum class Type {
    LEVEL = 0,
    LOGGER_NAME = 1,
    MESSAGE = 2,
    TIMESTAMP = 3,
    ELAPSED = 4,
    THREAD_ID = 5,
    FIBER_ID = 6,
    THREAD_NAME = 7,
    FILE_NAME = 8,
    LINE = 9,
    NEW_LINE = 10,
    MICROSECONDS = 11,
    TAB = 12,
    LITERAL = 13,
    NONE = 14,
  };

  struct Format_item {
    Type type = Type::NONE;
    std::string format;
  };

  explicit Formatter(std::string_view pattern);
  void formatter(const Log_record_view &record, Log_buffer &output) const;
  [[nodiscard]] const std::string &get_pattern() const noexcept { return pattern; }

private:
  int parse();
  void add_literal(std::string &literal);

  std::string pattern;
  std::vector<Format_item> item;
};
} // namespace lmns::detail

#endif