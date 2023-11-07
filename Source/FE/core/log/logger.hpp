#ifndef _FE_LOG_HPP_
#define _FE_LOG_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/types.hxx>
#include <FE/core/debug.h>
#include <FE/core/fstream_guard.hxx>
#include <tbb/cache_aligned_allocator.h>
#include <filesystem>
#include <fstream>
#include <string>


#if defined(_ENABLE_LOG_)
#define _IS_LOGGER_ENABLED_
#define IF_LOGGER_ENABLED(true_p, false_p) true_p
#define ENABLE_IF_LOGGER_ENABLED(code_p) code_p
#else
#define IF_LOGGER_ENABLED(true_p, false_p) false_p
#define ENABLE_IF_LOGGER_ENABLED(code_p)
#endif




BEGIN_NAMESPACE(FE::log)


class logger
{
public:
    _MAYBE_UNUSED_ static constexpr uint16 line_info_buffer_size = 32;
    _MAYBE_UNUSED_ static constexpr uint32 default_buffer_size = 10240;

#ifdef _WINDOWS_64BIT_OS_
    using directory_char_type = var::wchar;
#elif defined(_LINUX_64BIT_OS_)
    using directory_char_type = var::character
#endif
    using directory_buffer_type = std::basic_string<directory_char_type, std::char_traits<directory_char_type>, tbb::cache_aligned_allocator<directory_char_type>>;
    using filename_buffer_type = directory_buffer_type;
    using buffer_type = std::basic_string<char, std::char_traits<char>, tbb::cache_aligned_allocator<char>>;

protected:
    std::ofstream m_file_logger;
    FE::ofstream_guard m_file_guard;
    buffer_type  m_log_buffer;
    directory_buffer_type  m_directory_buffer;

    void __reserve() noexcept;

public:
    logger() noexcept : m_file_logger(), m_file_guard(m_file_logger) 
    {
        this->m_log_buffer.reserve(default_buffer_size);
        std::memset(this->m_log_buffer.data(), _NULL_, this->m_log_buffer.capacity() * sizeof(typename buffer_type::value_type));

        this->m_directory_buffer.reserve(default_buffer_size);
        std::memset(this->m_directory_buffer.data(), _NULL_, this->m_directory_buffer.capacity() * sizeof(typename directory_buffer_type::value_type));

        this->set_root_directory();
    }
    ~logger() noexcept {}

    void set_root_directory(const directory_char_type* root_directory_name_p = (std::filesystem::current_path() / "Logs").c_str()) noexcept;

    void mkdir(const directory_char_type* folder_name_p) noexcept;
    void cd(const directory_char_type* folder_name_p) noexcept;

    void do_log(const char* content_p, const directory_char_type* filename_p = nullptr) noexcept;

    template<class FN>
    static void log(FN lambda_p) noexcept { lambda_p(); }
};




class assertion_failure_log : public logger
{
public:
    using base_type = logger;

    assertion_failure_log() noexcept;
    ~assertion_failure_log() noexcept;

    void do_log(character* const message_p, character* const file_name_p, character* const function_name_p, uint32 line_p) noexcept;
};

class file_log : public logger
{
public:
    using base_type = logger;

    file_log() noexcept;
    ~file_log() noexcept;

    void do_log(character* const message_p, character* const file_name_p, character* const function_name_p, uint32 line_p) noexcept;
};


#ifdef _ENABLE_LOG_
_FORCE_INLINE_ void __FE_LOG_IMPLEMENTATION(const char* const message_p, const char* const file_name_p, const char* const function_name_p, FE::uint32 line_p) noexcept
{
    thread_local static ::FE::log::file_log tl_s_init;
    FE::log::logger::log([&]() { tl_s_init.do_log(message_p, file_name_p, function_name_p, line_p); });
}
#endif

#ifdef _ENABLE_ASSERT_
_FORCE_INLINE_ void __FE_ASSERT_IMPLEMENTATION(const char* const message_p, const char* const file_name_p, const char* const function_name_p, FE::uint32 line_p) noexcept
{
    thread_local static ::FE::log::assertion_failure_log tl_s_init;
    FE::log::logger::log([&]() { tl_s_init.do_log(message_p, file_name_p, function_name_p, line_p); });
}
#endif


END_NAMESPACE
#endif