#include <optional>
#include <string_view>

#include "lmns/logger/level.hpp"
#include "lmns/macros.hpp"
#include "logger/buffer.hpp"
#include "logger/formatter.hpp"

namespace lmns::detail {
Formatter::Formatter(std::string_view pattern) : pattern(pattern) {
  if (parse())
    throw std::invalid_argument("invalid log format. parse failed.");
}

template <typename T> void append_integer(Log_buffer &output, T value) {
  std::array<char, 24> buffer{};
  const auto result =
      std::to_chars(buffer.data(), buffer.data() + buffer.size(),
                    value); // 基本上不会失败，转失败了也没关系
  ASSERT_RET_VOID2(result.ec == std::errc(),
                   "trans " << value << " to chars failed.");
  output.append(buffer.data(),
                static_cast<std::size_t>(result.ptr - buffer.data()));
}

void format(const Log_record_view &record, Log_buffer &output,
            const Formatter::Format_item &item) {
  switch (item.type) {
  case Formatter::Type::LEVEL:
    if (auto ret = Log_level::to_string(record.level); ret)
      output.append(*ret);
    else
      ASSERT_RET_VOID2(false, "failed to convert log level to string, level="
                                  << static_cast<int>(record.level));
    break;
  case Formatter::Type::LOGGER_NAME:
    output.append(record.logger_name);
    break;
  case Formatter::Type::MESSAGE:
    output.append(record.message);
    break;
  case Formatter::Type::TIMESTAMP: {
    // Warning: 同一个进程中的datetime format要一致，否则会串
    static thread_local time_t last_second = 0;
    static thread_local char cached_date_time[20] = {'\0'};

    const time_t current_second =
        static_cast<time_t>(std::chrono::duration_cast<std::chrono::seconds>(
                                record.timestamp.time_since_epoch())
                                .count());
    if (current_second != last_second) {
      std::tm buffer{};
#if defined(_WIN32)
      const errno_t result = localtime_s(&buffer, &current_second);
      ASSERT_RETNONE2(result == 0, "failed to convert log time to local time");
#else
      const std::tm *result = localtime_r(&current_second, &buffer);
      ASSERT_RET_VOID2(result != nullptr,
                       "failed to convert log time to local time");
#endif
      const std::size_t size =
          std::strftime(cached_date_time, sizeof(cached_date_time),
                        item.format.data(), &buffer);
      ASSERT_RET_VOID2(size != 0, "failed to format log time");
      last_second = current_second;
    }

    output.append(cached_date_time);
    break;
  }
  case Formatter::Type::ELAPSED:
    append_integer(
        output,
        std::chrono::duration_cast<std::chrono::milliseconds>(record.elapsed)
            .count());
    break;
  case Formatter::Type::THREAD_ID:
    append_integer(output, record.thread_id);
    break;
  case Formatter::Type::FIBER_ID:
    append_integer(output, record.fiber_id);
    break;
  case Formatter::Type::THREAD_NAME:
    output.append(record.thread_name);
    break;
  case Formatter::Type::FILE_NAME:
    output.append(record.file_name);
    break;
  case Formatter::Type::LINE:
    append_integer(output, record.line);
    break;
  case Formatter::Type::NEW_LINE:
    output.append('\n');
    break;
  case Formatter::Type::MICROSECONDS: {

      const auto total_microseconds =
          std::chrono::duration_cast<std::chrono::microseconds>(
              record.timestamp.time_since_epoch())
              .count();
      std::int64_t microseconds = total_microseconds % 1'000'000;
      ASSERT_RET_VOID2(microseconds >= 0,
                       "log timestamp has a negative microsecond remainder: "
                           << microseconds);

    // 秒内微秒必须补齐成固定六位，否则 12:00:00.1234
    // 这样的时间戳无法解析，也无法按字典序排序。
    constexpr i8 MICROSECONDS_WIDTH = 6;
    std::array<char, MICROSECONDS_WIDTH> digits{};
    auto remaining = static_cast<std::uint32_t>(microseconds);
    for (std::size_t index = digits.size(); index-- > 0;) {
      digits[index] = static_cast<char>('0' + remaining % 10);
      remaining /= 10;
    }
    output.append(digits.data(), digits.size());
    break;
  }
  case Formatter::Type::TAB:
    output.append('\t');
    break;
  case Formatter::Type::LITERAL:
    output.append(item.format);
    break;
  case Formatter::Type::NONE:
  default:
    ASSERT_RET_VOID2(false, "unknown directive");
    break;
  }
}

std::optional<Formatter::Type> form_string(char c) {
  if (c == 'p')
    return Formatter::Type::LEVEL;
  if (c == 'c')
    return Formatter::Type::LOGGER_NAME;
  if (c == 'm')
    return Formatter::Type::MESSAGE;
  if (c == 'd')
    return Formatter::Type::TIMESTAMP;
  if (c == 'r')
    return Formatter::Type::ELAPSED;
  if (c == 't')
    return Formatter::Type::THREAD_ID;
  if (c == 'F')
    return Formatter::Type::FIBER_ID;
  if (c == 'N')
    return Formatter::Type::THREAD_NAME;
  if (c == 'f')
    return Formatter::Type::FILE_NAME;
  if (c == 'l')
    return Formatter::Type::LINE;
  if (c == 'n')
    return Formatter::Type::NEW_LINE;
  if (c == 'u')
    return Formatter::Type::MICROSECONDS;
  if (c == 'T')
    return Formatter::Type::TAB;
  // 没有对应的格式化项
  // Formatter::Type::LITERAL
  // Formatter::Type::NONE

  return std::nullopt;
}

void Formatter::formatter(const Log_record_view &record,
                          Log_buffer &output) const {
  for (const auto &item : item)
    format(record, output, item);
}

void Formatter::add_literal(std::string &literal) {
  if (literal.empty())
    return;
  item.push_back(Formatter::Format_item{Type::LITERAL, std::move(literal)});
  literal.clear();
}
int Formatter::parse() {
  std::string literal;
  for (std::size_t index = 0; index < pattern.size(); ++index) {
    if (pattern[index] != '%') {
      literal.push_back(pattern[index]);
      continue;
    }
    ASSERT_RET_VAL2(index + 1 < pattern.size(), -1,
                    "logger format pattern ends with an incomplete directive");
    const char directive = pattern[++index];
    if (directive == '%') {
      literal.push_back('%');
      continue;
    }
    add_literal(literal);
    std::string_view item_format;
    if (index + 1 < pattern.size() && pattern[index + 1] == '{') {
      const std::size_t closing_brace = pattern.find('}', index + 2);
      ASSERT_RET_VAL2(closing_brace != std::string::npos &&
                          closing_brace != (index + 2),
                      -2, "missing a closing brace or empty");
      item_format = std::string_view(pattern).substr(index + 2,
                                                     closing_brace - index - 2);
      index = closing_brace;
    }
    if (auto ret = form_string(directive); ret)
      item.push_back(Format_item{*ret, std::string{item_format}});
    else
      ASSERT_RET_VAL2(false, -3, "unknown directive");
  }

  add_literal(literal);

    for (const auto &item : item)
      ASSERT_RET_VAL2(item.type != Formatter::Type::NONE, -3,
                      "logger formatter contains a null format item");

  return 0;
}

} // namespace lmns::detail