

#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

#include <lmns/logger/appender.hpp>
#include <lmns/logger/level.hpp>
#include <lmns/logger/logger.hpp>
#include <lmns/macros.hpp>

#include "logger/appender_impl.hpp"
#include "logger/formatter.hpp"
#include "logger/logger_impl.hpp"

namespace lmns {
constexpr std::string_view DEFAULT_LOG_PATTERN = "%d{%Y-%m-%d %H:%M:%S}.%u%Tthread=%t%Tfiber=%F%T[%p]%T%f:%l%T%m%n";

Logger::Logger(std::string_view name) : impl(std::make_unique<Impl>(name)) {}

bool Logger::should_log(Log_level::Level level) const noexcept { return impl->should_log(level); }

void Logger::set_level(Log_level::Level level) noexcept { impl->set_level(level); }

Log_level::Level Logger::get_level() const noexcept { return impl->get_level(); }

std::string_view Logger::get_name() const noexcept { return impl->get_name(); }

void Logger::set_formatter(std::string_view pattern) { impl->set_formatter(std::make_shared<const detail::Formatter>(pattern)); }

std::string_view Logger::get_formatter_pattern() const noexcept {
	const auto formatter = impl->get_formatter();
	return formatter == nullptr ? std::string_view {} : formatter->get_pattern();
}

void Logger::add_appender(Appender_ptr appender) { impl->add_appender(appender); }

void Logger::remove_appender(const Appender_ptr &appender) { impl->remove_appender(appender); }

void Logger::clear_appenders() { impl->clear_appenders(); }

void Logger::flush() { impl->flush(); }

void Logger::sync() { impl->sync(); }

Logger::Impl::Impl(std::string_view name) : name(name) {
	if (name.empty())
		throw std::invalid_argument("logger name cannot be empty");

	formatter = std::make_shared<const detail::Formatter>(DEFAULT_LOG_PATTERN);
}

bool Logger::Impl::should_log(Log_level::Level level) const noexcept {
	if (static_cast<std::uint8_t>(level) < static_cast<std::uint8_t>(get_level()))
		return false;
	{
		std::shared_lock _ {mutex};
		if (!appenders.empty())
			return true;
	}
	return root != nullptr && root->should_log(level);
}

void Logger::Impl::submit(const detail::Log_record_view &record) noexcept {
	if (!should_log(record.level))
		return;

	std::shared_lock<std::shared_mutex> lock {mutex};
	if (appenders.empty()) {
		if (root != nullptr)
			root->impl->submit(record);
		return;
	}

	try {
		detail::Log_buffer formatted_record;
		formatter->format(record, formatted_record);
		for (const auto &appender : appenders)
			detail::Appender_access::append(appender, record.level, formatted_record.view());
	} catch (...) {
		constexpr std::string_view message = "lmns logger: record formating failed\n";
		std::fwrite(message.data(), 1, message.size(), stderr);
	}
}

void Logger::Impl::set_level(Log_level::Level level) noexcept { this->level.store(level, std::memory_order_release); }

Log_level::Level Logger::Impl::get_level() const noexcept { return level.load(std::memory_order_acquire); }

std::string_view Logger::Impl::get_name() const noexcept { return name; }

void Logger::Impl::set_formatter(std::shared_ptr<const detail::Formatter> formatter) {
	ASSERT_RET_VOID2(formatter != nullptr, "logger formatter cannot be nullptr");
	std::unique_lock<std::shared_mutex> _ {mutex};
	this->formatter = std::move(formatter);
}

std::shared_ptr<const detail::Formatter> Logger::Impl::get_formatter() const noexcept {
	std::shared_lock<std::shared_mutex> _ {mutex};
	return formatter;
}

void Logger::Impl::add_appender(Appender_ptr appender) {
	ASSERT_RET_VOID2(appender != nullptr, "logger appender cannot be nullptr");
	std::unique_lock<std::shared_mutex> _ {mutex};
	appenders.push_back(appender);
}

void Logger::Impl::remove_appender(const Appender_ptr &appender) {
	std::unique_lock<std::shared_mutex> _ {mutex};
	std::erase(this->appenders, appender);
}

void Logger::Impl::clear_appenders() {
	std::unique_lock<std::shared_mutex> _ {mutex};
	appenders.clear();
}

void Logger::Impl::flush() {
	std::shared_lock<std::shared_mutex> lock {mutex};
	if (appenders.empty()) {
		if (root != nullptr)
			root->impl->flush();
		return;
	}
	for (const auto &appender : appenders)
		appender->flush();
}

void Logger::Impl::sync() {
	std::shared_lock<std::shared_mutex> lock {mutex};
	if (appenders.empty()) {
		if (root != nullptr)
			root->impl->sync();
		return;
	}
	for (const auto &appender : appenders)
		appender->sync();
}

void Logger::Impl::set_root(Logger_ptr root) {
	ASSERT_RET_VOID2(root != nullptr, "logger root cannot be nullptr");
	std::shared_lock<std::shared_mutex> _ {mutex};
	this->root = root;
}

class Logger_manager {
public:
	Logger_manager() : root(std::make_shared<Logger>("root")) {
		root->add_appender(make_stdout_appender());
		loggers.emplace("root", root);
	}

	[[nodiscard]] Logger_ptr get_root() const noexcept { return root; }

	[[nodiscard]] Logger_ptr get_logger(std::string_view name) {
		ASSERT_RET_VAL2(!name.empty(), nullptr, "logger name cannot be empty");

		std::lock_guard<std::mutex> _ {mutex};
		if (const auto it = loggers.find(std::string(name)); it != loggers.end())
			return it->second;

		auto logger = std::make_shared<Logger>(std::string(name));
		loggers.emplace(std::string(name), logger);
		return logger;
	}

private:
	mutable std::mutex mutex;
	std::unordered_map<std::string, Logger_ptr> loggers;
	Logger_ptr root;
};

Logger_manager &get_logger_manager() {
	static Logger_manager instance;
	return instance;
}

Logger_ptr get_root_logger() { return get_logger_manager().get_root(); }

Logger_ptr get_logger(std::string_view name) { return get_logger_manager().get_logger(name); }

} // namespace lmns
