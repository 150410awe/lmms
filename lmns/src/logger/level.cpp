
#include <algorithm>
#include <array>
#include <cctype>

#include <lmns/logger/level.hpp>
namespace lmns {

std::optional<std::string_view> Log_level::to_string(Level value) noexcept {
	switch (value) {
	case Level::DEBUG:
		return "DEBUG";
	case Level::INFO:
		return "INFO";
	case Level::WARN:
		return "WARN";
	case Level::ERROR:
		return "ERROR";
	case Level::FATAL:
		return "FATAL";
	case Level::OFF:
		return "OFF";
	default:
		return std::nullopt;
	}
}
std::optional<Log_level::Level>
Log_level::from_string(std::string_view value) noexcept {
	if (value.size() > 6)
		return std::nullopt;
	std::array<char, 6> normalized {};
	std::transform(value.begin(), value.end(), normalized.begin(),
		       [](unsigned char character) {
			       return static_cast<char>(std::toupper(character));
		       });
	const std::string_view upper_value {normalized.data(), value.size()};
	if (upper_value == "DEBUG")
		return Level::DEBUG;
	if (upper_value == "INFO")
		return Level::INFO;
	if (upper_value == "WARN")
		return Level::WARN;
	if (upper_value == "ERROR")
		return Level::ERROR;
	if (upper_value == "FATAL")
		return Level::FATAL;
	if (upper_value == "OFF")
		return Level::OFF;
	return std::nullopt;
}
} // namespace lmns
