﻿#ifndef _FE_CORE_FSTREAM_GUARD_HXX_
#define _FE_CORE_FSTREAM_GUARD_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/macros/macro_definitions.h>
#include <fstream>




BEGIN_NAMESPACE(FE)

template<class FStream>
class basic_fstream_guard final
{
private:
	FStream& m_fstream;

public:
	typedef FStream fstream_type;

	_FORCE_INLINE_ basic_fstream_guard(FStream& file_io_p) noexcept : m_fstream(file_io_p) {}

	_FORCE_INLINE_ basic_fstream_guard(FStream& file_io_p, const typename FStream::char_type* string_p) noexcept : m_fstream(file_io_p)
	{
		if (this->m_fstream.is_open() == false)
		{
			this->m_fstream.open(string_p);
		}
	}

	_FORCE_INLINE_ basic_fstream_guard& operator=(FStream& file_io_p) noexcept
	{
		this->m_fstream = file_io_p;
		return *this;
	}

	basic_fstream_guard(const basic_fstream_guard& other_p) noexcept = delete;
	basic_fstream_guard(basic_fstream_guard&& rvalue_p) noexcept = delete;
	basic_fstream_guard& operator=(const basic_fstream_guard& other_p) noexcept = delete;
	basic_fstream_guard& operator=(basic_fstream_guard&& rvalue_p) noexcept = delete;

	_FORCE_INLINE_ ~basic_fstream_guard() noexcept
	{
		if (this->m_fstream.is_open() == true)
		{
			this->m_fstream.close();
		}
	}

	_FORCE_INLINE_ void close() noexcept { this->~basic_fstream_guard(); }

	_FORCE_INLINE_ void operator<<(const typename FStream::char_type* buffer_pointer_p) noexcept { this->m_fstream << buffer_pointer_p; }

	_FORCE_INLINE_ void operator<<(const typename FStream::char_type buffer_p) noexcept { this->m_fstream << buffer_p; }

	_FORCE_INLINE_ bool operator>>(typename FStream::char_type* buffer_pointer_p) noexcept
	{
		if (this->m_fstream.is_open() == false) { return false; }

		this->m_fstream << buffer_pointer_p;
		return true;
	}

	_FORCE_INLINE_ bool operator>>(typename FStream::char_type buffer_p) noexcept
	{
		if (this->m_fstream.is_open() == false) { return false; }

		this->m_fstream << buffer_p;
		return true;
	}

	_FORCE_INLINE_ bool is_open() noexcept { return this->m_fstream.is_open(); }
	_FORCE_INLINE_ void endl() noexcept { this->m_fstream << std::endl; }
	_FORCE_INLINE_ FStream& get_fstream_reference() noexcept { return this->m_fstream; }
};

using fstream_guard = FE::basic_fstream_guard<std::fstream>;
using wfstream_guard = FE::basic_fstream_guard<std::wfstream>;

using ofstream_guard = FE::basic_fstream_guard<std::ofstream>;
using wofstream_guard = FE::basic_fstream_guard<std::wofstream>;

using ifstream_guard = FE::basic_fstream_guard<std::ifstream>;
using wifstream_guard = FE::basic_fstream_guard<std::wifstream>;

END_NAMESPACE
#endif
