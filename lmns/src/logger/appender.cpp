

#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <utility>

#include "logger/appender_impl.hpp"

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

namespace lmns {
Appender::Appender(std::unique_ptr<Impl> impl) noexcept : impl(std::move(impl)) {}

void Appender::set_level(Log_level::Level level) noexcept { impl->set_level(level); }

Log_level::Level Appender::get_level() const noexcept { return impl->get_level(); }

void Appender::flush() { impl->flush(); }

void Appender::sync() { impl->sync(); }

void Appender::Impl::append(Log_level::Level level, std::string_view formatted_record) noexcept {
	if (static_cast<std::uint8_t>(level) < static_cast<std::uint8_t>(get_level()))
		return;

	try {
		std::lock_guard<std::mutex> _ {mutex};
		write_unlocked(formatted_record);
		if (level == Log_level::Level::FATAL)
			flush_unlocked();
	} catch (...) {
		constexpr std::string_view message = "lmns logger: appender operation failed\n";
		std::fwrite(message.data(), 1, message.size(), stderr);
	}
}

void Appender::Impl::set_level(Log_level::Level level) noexcept { this->level.store(level, std::memory_order_release); }

Log_level::Level Appender::Impl::get_level() const noexcept { return level.load(std::memory_order_acquire); }

// flush接口，把数据从程序的内存刷到操作系统缓冲区
void Appender::Impl::flush() {
	std::lock_guard<std::mutex> lock(mutex);
	flush_unlocked();
}

// sync接口，把数据从操作系统的缓冲区刷到IO缓冲区
void Appender::Impl::sync() {
	std::lock_guard<std::mutex> lock(mutex);
	sync_unlocked();
}

class Stdout_appender_impl final : public Appender::Impl {
protected:
	void write_unlocked(std::string_view formatted_record) noexcept override { std::cout.write(formatted_record.data(), static_cast<std::streamsize>(formatted_record.size())); }
	void flush_unlocked() noexcept override { std::cout.flush(); }
	void sync_unlocked() noexcept override { flush_unlocked(); }
};

class File_appender_impl final : public Appender::Impl {

public:
	explicit File_appender_impl(const std::filesystem::path &path) {
		if (path.empty())
			throw std::invalid_argument("logger file name cannot be empty");
		const std::string file_name = path.string();
#ifdef _WIN32
		if (fopen_s(&file, file_name.c_str(), "ab") != 0 || file == nullptr) {
#else
		file = std::fopen(file_name.c_str(), "ab");
		if (file == nullptr) {
#endif
			throw std::system_error(errno, std::generic_category(),
						"failed to open logger file: " + file_name);
		}
	}

	~File_appender_impl() noexcept override {
		if (file == nullptr)
			return;
		const bool flush_failed = std::fflush(file) != 0;
		const bool close_failed = std::fclose(file) != 0;
		if (flush_failed || close_failed) {
			constexpr std::string_view message = "lmns logger: failed to close logger file\n";
			std::fwrite(message.data(), 1, message.size(), stderr);
		}
	}

protected:
	void write_unlocked(std::string_view formatted_record) noexcept override { std::fwrite(formatted_record.data(), 1, formatted_record.size(), file); }

	void flush_unlocked() noexcept override { std::fflush(file); }

	void sync_unlocked() noexcept override {
		flush_unlocked();
#if defined(_WIN32)
		::_commit(::_fileno(file));
#else
		::fdatasync(::fileno(file));
#endif
	}

private:
	std::FILE *file = nullptr;
};

Appender_ptr detail::Appender_access::make_stdout_appender() { return Appender_ptr(new Appender(std::make_unique<Stdout_appender_impl>())); }

Appender_ptr detail::Appender_access::make_file_appender(std::filesystem::path path) { return Appender_ptr(new Appender(std::make_unique<File_appender_impl>(path))); }

void detail::Appender_access::append(
	const Appender_ptr &appender, Log_level::Level level,
	std::string_view formatted_record) noexcept {
	if (appender != nullptr)
		appender->impl->append(level, formatted_record);
}

Appender_ptr make_stdout_appender() { return detail::Appender_access::make_stdout_appender(); }

Appender_ptr make_file_appender(std::filesystem::path path) { return detail::Appender_access::make_file_appender(std::move(path)); }
} // namespace lmns
