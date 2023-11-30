#ifndef _FE_LOG_MACRO_H_
#define _FE_LOG_MACRO_H_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#ifdef FE_LOG
#error FE_LOG is a reserved Frogman Engine macro keyword.
#endif 
#ifdef FE_ASSERT
#error FE_ASSERT is a reserved Frogman Engine macro keyword.
#endif 
#ifdef FE_EXIT
#error FE_EXIT is a reserved Frogman Engine macro keyword.
#endif 

#include <FE/core/log/logger.hpp>
#include <FE/core/log/format_string.h>
#include <FE/core/macros/attributes.h>
#include <cassert>
#include <cstdlib>




#ifdef _ENABLE_LOG_
// ${%d at n} - int32
// ${%u at n} - uint32
// ${%ld at n} - int64
// ${%lu at n} - uint64
// ${%lf at n} - float64
// ${%f at n} - float32
// ${%b at n} - bool
// ${%c at n} - char
// ${%s at n} - string
// ${%p at n} - hexadecimal 64bit pointer
#define FE_LOG(...) \
{ \
	const char* __FE_LOG_SOURCE_DIR__ = __FILE__; \
	const char* __FE_LOG_FUNCTION_NAME__ = __func__; \
	::FE::uint32 __FE_LOG_CODE_LINE__ = __LINE__; \
	::FE::log::__FE_LOG_IMPLEMENTATION(::FE::log::buffered_string_formatter({ __VA_ARGS__ }), __FE_LOG_SOURCE_DIR__, __FE_LOG_FUNCTION_NAME__, __FE_LOG_CODE_LINE__); \
}
#else
#define FE_LOG(...)
#endif


#ifdef _ENABLE_ASSERT_
// ${%d at n} - int32
// ${%u at n} - uint32
// ${%ld at n} - int64
// ${%lu at n} - uint64
// ${%lf at n} - float64
// ${%f at n} - float32
// ${%b at n} - bool
// ${%c at n} - char
// ${%s at n} - string
// ${%p at n} - hexadecimal 64bit pointer  |  FE_ASSERT() invokes abort() if the expression is true.
#define FE_ASSERT(expression, ...) \
if(expression) _UNLIKELY_ \
{ \
	const char* __FE_ASSERT_SOURCE_DIR__ = __FILE__; \
	const char* __FE_ASSERT_FUNCTION_NAME__ = __func__; \
	::FE::uint32 __FE_ASSERT_CODE_LINE__ = __LINE__; \
	::FE::log::__FE_ABORT_IMPLEMENTATION(::FE::log::buffered_string_formatter({ __VA_ARGS__ }), __FE_ASSERT_SOURCE_DIR__, __FE_ASSERT_FUNCTION_NAME__, __FE_ASSERT_CODE_LINE__); \
	assert(!(expression)); \
}
#else
#define FE_ASSERT(expression, ...)
#endif


#ifdef _ENABLE_EXIT_
// ${%d at n} - int32
// ${%u at n} - uint32
// ${%ld at n} - int64
// ${%lu at n} - uint64
// ${%lf at n} - float64
// ${%f at n} - float32
// ${%b at n} - bool
// ${%c at n} - char
// ${%s at n} - string
// ${%p at n} - hexadecimal 64bit pointer  |  FE_EXIT() invokes abort() if the expression is true.
#define FE_EXIT(expression, error_code, ...) \
if(expression) _UNLIKELY_ \
{ \
	const char* __FE_EXIT_SOURCE_DIR__ = __FILE__; \
	const char* __FE_EXIT_FUNCTION_NAME__ = __func__; \
	::FE::uint32 __FE_EXIT_CODE_LINE__ = __LINE__; \
	::FE::log::__FE_ABORT_IMPLEMENTATION(::FE::log::buffered_string_formatter({ __VA_ARGS__ }), __FE_EXIT_SOURCE_DIR__, __FE_EXIT_FUNCTION_NAME__, __FE_EXIT_CODE_LINE__); \
	::std::exit(static_cast<::FE::var::int32>(error_code)); \
}
#else
#define FE_EXIT(expression, error_code, ...)
#endif


#define TO_STRING(p) #p

#define _NODEFAULT_ default: _UNLIKELY_ FE_ASSERT(true, "Reached Default Case: This switch has no default."); break;




enum struct INPUT_ERROR_2XX : FE::int32
{
	_FATAL_ERROR_INVALID_ARGUMENT = 200,
	_FATAL_ERROR_INVALID_KEY = 201
};


#endif