#ifndef LMNS_SRC_LOGGER_BUFFER_HPP
#define LMNS_SRC_LOGGER_BUFFER_HPP

#include <array>
#include <vector>
#include <cstring>
#include <cstdint>
#include <streambuf>

namespace lmns::detail
{
    class Log_buffer
    {
    private:
        constexpr static int64_t BULLET_SIZE = 256;
        std::array<char, BULLET_SIZE> bullet{};
        std::vector<char> overflow;
        int64_t m_size = 0;

    public:
        void append(const char *data, int64_t m_size)
        {
            if (m_size <= 0)
                return;

            if (overflow.empty() && m_size + this->m_size <= BULLET_SIZE)
            {
                std::memcpy(bullet.data() + this->m_size, data, m_size);
                this->m_size += m_size;
                return;
            }

            if (overflow.empty())
            {
                const int64_t required_capacity = m_size + this->m_size;
                overflow.resize(std::max(BULLET_SIZE * 2, required_capacity));
                overflow.insert(overflow.end(), bullet.data(), bullet.data() + this->m_size);
            }
            overflow.insert(overflow.end(), data, data + m_size);
            this->m_size = overflow.size();
        }

        void append(std::string_view value)
        {
            append(value.data(), value.size());
        }
        void append(char value)
        {
            append(&value, 1);
        }
        [[nodiscard]] const char *data() const noexcept
        {
            return overflow.empty() ? bullet.data() : overflow.data();
        }
        [[nodiscard]] int64_t size() const noexcept
        {
            return m_size;
        }
        [[nodiscard]] std::string_view view() const noexcept
        {
            return {data(), static_cast<std::size_t>(size())};
        }
    };
    class Stream_buffer final : public std::streambuf
    {
    private:
        Log_buffer &bullet;

    public:
        explicit Stream_buffer(Log_buffer &bullet) noexcept : bullet(bullet) {}

    protected:
        std::streamsize xsputn(const char *data, std::streamsize m_size) override
        {
            if (m_size <= 0)
                return 0;

            bullet.append(data, static_cast<int64_t>(m_size));
            return m_size;
        }
        int_type overflow(int_type character) override
        {
            if (traits_type::eq_int_type(character, traits_type::eof()))
                return traits_type::not_eof(character);
            bullet.append(traits_type::to_char_type(character));
            return character;
        }
    };
}

#endif