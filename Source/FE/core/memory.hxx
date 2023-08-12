﻿#ifndef _FE_CORE_PRIVATE_MEMORY_HXX_
#define _FE_CORE_PRIVATE_MEMORY_HXX_
// Copyright © 2023~ UNKNOWN STRYKER. All Rights Reserved.
#ifdef UNALIGNED_MEMSET
#error UNALIGNED_MEMSET is a reserved macro keyword
#endif
#ifdef ALIGNED_MEMSET
#error ALIGNED_MEMSET is a reserved macro keyword
#endif
#ifdef UNALIGNED_MEMCPY
#error UNALIGNED_MEMCPY is a reserved macro keyword
#endif
#ifdef ALIGNED_MEMCPY
#error ALIGNED_MEMCPY is a reserved macro keyword
#endif
#define DIVIDE_BY_2(input_p) ((input_p) >> 1)
#define DIVIDE_BY_4(input_p) ((input_p) >> 2)
#define DIVIDE_BY_8(input_p) ((input_p) >> 3)
#define DIVIDE_BY_16(input_p) ((input_p) >> 4)
#define DIVIDE_BY_32(input_p) ((input_p) >> 5)
#define DIVIDE_BY_64(input_p) ((input_p) >> 6)
#define DIVIDE_BY_128(input_p) ((input_p) >> 7)

#define MODULO_BY_2(input_p) ((input_p) & 1)
#define MODULO_BY_4(input_p) ((input_p) & 3)
#define MODULO_BY_8(input_p) ((input_p) & 7)
#define MODULO_BY_16(input_p) ((input_p) & 15)
#define MODULO_BY_32(input_p) ((input_p) & 31)
#define MODULO_BY_64(input_p) ((input_p) & 63)
#define MODULO_BY_128(input_p) ((input_p) & 127)
#define ABS(x) ((x < 0) ? x * -1 : x)
#include <FE/core/prerequisite_symbols.h>
#include <immintrin.h>
#ifdef __AVX__
#define _AVX_ true
#else
#define _AVX_ false
#endif

#ifdef __AVX512F__
#define _AVX512_ true
#else
#define _AVX512_ false
#endif




BEGIN_NAMESPACE(FE)


#if _AVX512_ == true
_FORCE_INLINE_ void unaligned_memset_with_avx512(void* const dest_ptrc_p, int8 value_p, size_t total_bytes_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(total_bytes_p == 0, "ERROR: element_bytes_p is 0.");

	__m512i* l_m512i_dest_ptr = static_cast<__m512i*>(dest_ptrc_p);
	const __m512i l_m512i_value_to_be_assigned = _mm512_set1_epi8(value_p);

	var::size_t l_leftover_bytes = MODULO_BY_64(total_bytes_p);
	size_t l_avx_operation_count = MODULO_BY_64(total_bytes_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm512_storeu_si512(l_m512i_dest_ptr, l_m512i_value_to_be_assigned);
		++l_m512i_dest_ptr;
	}

	if (l_leftover_bytes >= 16)
	{
		memset(l_m512i_dest_ptr, value_p, l_leftover_bytes);
		return;
	}

	var::byte* l_byte_ptr = reinterpret_cast<var::byte*>(l_m512i_dest_ptr);
	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_byte_ptr = value_p;
		++l_byte_ptr;
	}
}

_FORCE_INLINE_ void aligned_memset_with_avx512(void* const dest_ptrc_p, int8 value_p, size_t total_bytes_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(total_bytes_p == 0, "ERROR: element_bytes_p is 0.");
	FE_ASSERT((reinterpret_cast<uintptr_t>(dest_ptrc_p) % 64) != 0, "ERROR: dest_ptrc_p is not aligned by 64.");

	__m512i* l_m512i_dest_ptr = static_cast<__m512i*>(dest_ptrc_p);
	const __m512i l_m512i_value_to_be_assigned = _mm512_set1_epi8(value_p);

	var::size_t l_leftover_bytes = MODULO_BY_64(total_bytes_p);
	size_t l_avx_operation_count = MODULO_BY_64(total_bytes_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm512_store_si512(l_m512i_dest_ptr, l_m512i_value_to_be_assigned);
		++l_m512i_dest_ptr;
	}

	if (l_leftover_bytes >= 16)
	{
		memset(l_m512i_dest_ptr, value_p, l_leftover_bytes);
		return;
	}

	var::byte* l_byte_ptr = reinterpret_cast<var::byte*>(l_m512i_dest_ptr);
	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_byte_ptr = value_p;
		++l_byte_ptr;
	}
}


_FORCE_INLINE_ void unaligned_memcpy_with_avx512(void* const dest_ptrc_p, const void* const source_ptrc_p, FE::size_t bytes_to_copy_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(bytes_to_copy_p == 0, "ERROR: element_bytes_p is 0.");

	__m512i* l_m512i_dest_ptr = static_cast<__m512i*>(dest_ptrc_p);
	const __m512i* l_m512i_source_ptr = static_cast<const __m512i*>(source_ptrc_p);

	var::size_t l_leftover_bytes = MODULO_BY_64(bytes_to_copy_p);
	size_t l_avx_operation_count = MODULO_BY_64(bytes_to_copy_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm512_storeu_si512(l_m512i_dest_ptr, _mm512_loadu_si512(l_m512i_source_ptr));
		++l_m512i_dest_ptr;
		++l_m512i_source_ptr;
	}

	if (l_leftover_bytes >= 16)
	{
		memcpy(l_m512i_dest_ptr, l_m512i_source_ptr, l_leftover_bytes);
		return;
	}

	var::byte* l_dest_byte_ptr = reinterpret_cast<var::byte*>(l_m512i_dest_ptr);
	byte* l_source_byte_ptr = reinterpret_cast<byte*>(l_m512i_source_ptr);
	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_dest_byte_ptr = *l_source_byte_ptr;
		++l_dest_byte_ptr;
		++l_source_byte_ptr;
	}
}

_FORCE_INLINE_ void aligned_memcpy_with_avx512(void* const dest_ptrc_p, const void* const source_ptrc_p, FE::size_t bytes_to_copy_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(bytes_to_copy_p == 0, "ERROR: element_bytes_p is 0.");
	FE_ASSERT((reinterpret_cast<uintptr_t>(dest_ptrc_p) % 64) != 0, "ERROR: dest_ptrc_p is not aligned by 64.");
	FE_ASSERT((reinterpret_cast<uintptr_t>(source_ptrc_p) % 64) != 0, "ERROR: source_ptrc_p is not aligned by 64.");

	__m512i* l_m512i_dest_ptr = static_cast<__m512i*>(dest_ptrc_p);
	const __m512i* l_m512i_source_ptr = static_cast<const __m512i*>(source_ptrc_p);

	var::size_t l_leftover_bytes = MODULO_BY_64(bytes_to_copy_p);
	size_t l_avx_operation_count = MODULO_BY_64(bytes_to_copy_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm512_store_si512(l_m512i_dest_ptr, _mm512_load_si512(l_m512i_source_ptr));
		++l_m512i_dest_ptr;
		++l_m512i_source_ptr;
	}

	if (l_leftover_bytes >= 16)
	{
		memcpy(l_m512i_dest_ptr, l_m512i_source_ptr, l_leftover_bytes);
		return;
	}

	var::byte* l_dest_byte_ptr = reinterpret_cast<var::byte*>(l_m512i_dest_ptr);
	byte* l_source_byte_ptr = reinterpret_cast<byte*>(l_m512i_source_ptr);
	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_dest_byte_ptr = *l_source_byte_ptr;
		++l_dest_byte_ptr;
		++l_source_byte_ptr;
	}
}
#elif _AVX_ == true
_FORCE_INLINE_ void unaligned_memset_with_avx(void* const dest_ptrc_p, int8 value_p, size_t total_bytes_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(total_bytes_p == 0, "ERROR: element_bytes_p is 0.");

	__m256i* l_m256i_dest_ptr = static_cast<__m256i*>(dest_ptrc_p);
	const __m256i l_m256i_value_to_be_assigned = _mm256_set1_epi8(value_p);

	var::size_t l_leftover_bytes = MODULO_BY_32(total_bytes_p);
	size_t l_avx_operation_count = DIVIDE_BY_32(total_bytes_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm256_storeu_si256(l_m256i_dest_ptr, l_m256i_value_to_be_assigned);
		++l_m256i_dest_ptr;
	}

	__m128i* l_m128i_dest_ptr = reinterpret_cast<__m128i*>(l_m256i_dest_ptr);
	if (l_leftover_bytes >= 16)
	{
		_mm_storeu_si128(l_m128i_dest_ptr, _mm_set1_epi8(value_p));
		++l_m128i_dest_ptr;
		l_leftover_bytes -= 16;
	}
	var::byte* l_byte_ptr = reinterpret_cast<var::byte*>(l_m128i_dest_ptr);

	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_byte_ptr = value_p;
		++l_byte_ptr;
	}
}

_FORCE_INLINE_ void aligned_memset_with_avx(void* const dest_ptrc_p, int8 value_p, size_t total_bytes_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(total_bytes_p == 0, "ERROR: element_bytes_p is 0.");
	FE_ASSERT((reinterpret_cast<uintptr_t>(dest_ptrc_p) % 32) != 0, "ERROR: dest_ptrc_p is not aligned by 32.");

	__m256i* l_m256i_dest_ptr = static_cast<__m256i*>(dest_ptrc_p);
	const __m256i l_m256i_value_to_be_assigned = _mm256_set1_epi8(value_p);

	var::size_t l_leftover_bytes = MODULO_BY_32(total_bytes_p);
	size_t l_avx_operation_count = DIVIDE_BY_32(total_bytes_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm256_store_si256(l_m256i_dest_ptr, l_m256i_value_to_be_assigned);
		++l_m256i_dest_ptr;
	}

	__m128i* l_m128i_dest_ptr = reinterpret_cast<__m128i*>(l_m256i_dest_ptr);
	if (l_leftover_bytes >= 16)
	{
		_mm_store_si128(l_m128i_dest_ptr, _mm_set1_epi8(value_p));
		++l_m128i_dest_ptr;
		l_leftover_bytes -= 16;
	}
	var::byte* l_byte_ptr = reinterpret_cast<var::byte*>(l_m128i_dest_ptr);

	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_byte_ptr = value_p;
		++l_byte_ptr;
	}
}


_FORCE_INLINE_ void unaligned_memcpy_with_avx(void* const dest_ptrc_p, const void* const source_ptrc_p, FE::size_t bytes_to_copy_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(bytes_to_copy_p == 0, "ERROR: element_bytes_p is 0.");

	__m256i* l_m256i_dest_ptr = static_cast<__m256i*>(dest_ptrc_p);
	const __m256i* l_m256i_source_ptr = static_cast<const __m256i*>(source_ptrc_p);

	var::size_t l_leftover_bytes = MODULO_BY_32(bytes_to_copy_p);
	size_t l_avx_operation_count = DIVIDE_BY_32(bytes_to_copy_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm256_storeu_si256(l_m256i_dest_ptr, _mm256_loadu_si256(l_m256i_source_ptr));
		++l_m256i_dest_ptr;
		++l_m256i_source_ptr;
	}

	__m128i* l_m128i_dest_ptr = reinterpret_cast<__m128i*>(l_m256i_dest_ptr);
	const __m128i* l_m128i_source_ptr = reinterpret_cast<const __m128i*>(l_m256i_source_ptr);
	if (l_leftover_bytes >= 16)
	{
		_mm_storeu_si128(l_m128i_dest_ptr, _mm_loadu_si128(l_m128i_source_ptr));
		++l_m128i_dest_ptr;
		++l_m128i_source_ptr;
		l_leftover_bytes -= 16;
	}

	var::byte* l_dest_byte_ptr = reinterpret_cast<var::byte*>(l_m128i_dest_ptr);
	byte* l_source_byte_ptr = reinterpret_cast<byte*>(l_m128i_source_ptr);
	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_dest_byte_ptr = *l_source_byte_ptr;
		++l_dest_byte_ptr;
		++l_source_byte_ptr;
	}
}

_FORCE_INLINE_ void aligned_memcpy_with_avx(void* const dest_ptrc_p, const void* const source_ptrc_p, FE::size_t bytes_to_copy_p) noexcept
{
	FE_ASSERT(dest_ptrc_p == nullptr, "ERROR: dest_ptrc_p is nullptr.");
	FE_ASSERT(bytes_to_copy_p == 0, "ERROR: element_bytes_p is 0.");
	FE_ASSERT((reinterpret_cast<uintptr_t>(dest_ptrc_p) % 32) != 0, "ERROR: dest_ptrc_p is not aligned by 32.");
	FE_ASSERT((reinterpret_cast<uintptr_t>(source_ptrc_p) % 32) != 0, "ERROR: source_ptrc_p is not aligned by 32.");

	__m256i* l_m256i_dest_ptr = static_cast<__m256i*>(dest_ptrc_p);
	const __m256i* l_m256i_source_ptr = static_cast<const __m256i*>(source_ptrc_p);

	var::size_t l_leftover_bytes = MODULO_BY_32(bytes_to_copy_p);
	size_t l_avx_operation_count = DIVIDE_BY_32(bytes_to_copy_p - l_leftover_bytes);

	for (var::size_t executed_operation_count = 0; executed_operation_count != l_avx_operation_count; ++executed_operation_count)
	{
		_mm256_store_si256(l_m256i_dest_ptr, _mm256_load_si256(l_m256i_source_ptr));
		++l_m256i_dest_ptr;
		++l_m256i_source_ptr;
	}

	__m128i* l_m128i_dest_ptr = reinterpret_cast<__m128i*>(l_m256i_dest_ptr);
	const __m128i* l_m128i_source_ptr = reinterpret_cast<const __m128i*>(l_m256i_source_ptr);
	if (l_leftover_bytes >= 16)
	{
		_mm_store_si128(l_m128i_dest_ptr, _mm_load_si128(l_m128i_source_ptr));
		++l_m128i_dest_ptr;
		++l_m128i_source_ptr;
		l_leftover_bytes -= 16;
	}

	var::byte* l_dest_byte_ptr = reinterpret_cast<var::byte*>(l_m128i_dest_ptr);
	byte* l_source_byte_ptr = reinterpret_cast<byte*>(l_m128i_source_ptr);
	for (var::size_t i = 0; i != l_leftover_bytes; ++i)
	{
		*l_dest_byte_ptr = *l_source_byte_ptr;
		++l_dest_byte_ptr;
		++l_source_byte_ptr;
	}
}
#endif


#if _AVX512_ == true
#define UNALIGNED_MEMSET(dest_ptrc_p, value_p, total_bytes_p) ::FE::unaligned_memset_with_avx512(dest_ptrc_p, value_p, total_bytes_p)
#define ALIGNED_MEMSET(dest_ptrc_p, value_p, total_bytes_p) ::FE::aligned_memset_with_avx512(dest_ptrc_p, value_p, total_bytes_p)
#define UNALIGNED_MEMCPY(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p) ::FE::unaligned_memcpy_with_avx512(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p)
#define ALIGNED_MEMCPY(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p) ::FE::aligned_memcpy_with_avx512(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p)

#elif _AVX_ == true
#define UNALIGNED_MEMSET(dest_ptrc_p, value_p, total_bytes_p) ::FE::unaligned_memset_with_avx(dest_ptrc_p, value_p, total_bytes_p)
#define ALIGNED_MEMSET(dest_ptrc_p, value_p, total_bytes_p) ::FE::aligned_memset_with_avx(dest_ptrc_p, value_p, total_bytes_p)
#define UNALIGNED_MEMCPY(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p) ::FE::unaligned_memcpy_with_avx(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p)
#define ALIGNED_MEMCPY(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p) ::FE::aligned_memcpy_with_avx(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p)

#else
#define UNALIGNED_MEMSET(dest_ptrc_p, value_p, total_bytes_p) ::std::memset(dest_ptrc_p, value_p, total_bytes_p)
#define ALIGNED_MEMSET(dest_ptrc_p, value_p, total_bytes_p) ::std::memset(dest_ptrc_p, value_p, total_bytes_p)
#define UNALIGNED_MEMCPY(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p) ::std::memcpy(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p)
#define ALIGNED_MEMCPY(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p) ::std::memcpy(dest_ptrc_p, source_ptrc_p, bytes_to_copy_p)
#endif


_FORCE_INLINE_ void unaligned_memcpy(void* const dest_memblock_ptrc_p, length_t dest_length_p, size_t dest_element_bytes_p, const void* const source_memblock_ptrc_p, length_t source_length_p, size_t source_element_bytes_p) noexcept
{
	ABORT_IF(dest_memblock_ptrc_p == nullptr, "ERROR: dest_memblock_ptrc_p is nullptr.");
	ABORT_IF(source_memblock_ptrc_p == nullptr, "ERROR: source_memblock_ptrc_p is nullptr.");

	ABORT_IF(dest_length_p == 0, "ERROR: dest_length_p is 0.");
	ABORT_IF(dest_element_bytes_p == 0, "ERROR: dest_bytes_p is 0.");
	ABORT_IF(source_element_bytes_p == 0, "ERROR: source_bytes_p is 0.");

	size_t l_source_size = source_element_bytes_p * source_length_p;
	size_t l_dest_size = dest_element_bytes_p * dest_length_p;

	if (l_source_size >= l_dest_size)
	{
		UNALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, l_dest_size);
	}
	else
	{
		UNALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, l_source_size);
	}
}

_FORCE_INLINE_ void aligned_memcpy(void* const dest_memblock_ptrc_p, length_t dest_length_p, size_t dest_element_bytes_p, const void* const source_memblock_ptrc_p, length_t source_length_p, size_t source_element_bytes_p) noexcept
{
	ABORT_IF(dest_memblock_ptrc_p == nullptr, "ERROR: dest_memblock_ptrc_p is nullptr.");
	ABORT_IF(source_memblock_ptrc_p == nullptr, "ERROR: source_memblock_ptrc_p is nullptr.");

	ABORT_IF(dest_length_p == 0, "ERROR: dest_length_p is 0.");
	ABORT_IF(dest_element_bytes_p == 0, "ERROR: dest_bytes_p is 0.");
	ABORT_IF(source_element_bytes_p == 0, "ERROR: source_bytes_p is 0.");

	size_t l_source_size = source_element_bytes_p * source_length_p;
	size_t l_dest_size = dest_element_bytes_p * dest_length_p;

	if (l_source_size >= l_dest_size)
	{
		ALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, l_dest_size);
	}
	else
	{
		ALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, l_source_size);
	}
}

_FORCE_INLINE_ void unaligned_memcpy(void* const dest_memblock_ptrc_p, size_t dest_bytes_p, const void* const source_memblock_ptrc_p, size_t source_bytes_p) noexcept
{
	ABORT_IF(dest_memblock_ptrc_p == nullptr, "ERROR: dest_memblock_ptrc_p is nullptr.");
	ABORT_IF(source_memblock_ptrc_p == nullptr, "ERROR: source_memblock_ptrc_p is nullptr.");

	ABORT_IF(dest_bytes_p == 0, "ERROR: dest_bytes_p is 0.");
	ABORT_IF(source_bytes_p == 0, "ERROR: source_bytes_p is 0.");

	if (source_bytes_p >= dest_bytes_p)
	{
		UNALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, dest_bytes_p);
	}
	else
	{
		UNALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, source_bytes_p);
	}
}

_FORCE_INLINE_ void aligned_memcpy(void* const dest_memblock_ptrc_p, size_t dest_bytes_p, const void* const source_memblock_ptrc_p, size_t source_bytes_p) noexcept
{
	ABORT_IF(dest_memblock_ptrc_p == nullptr, "ERROR: dest_memblock_ptrc_p is nullptr.");
	ABORT_IF(source_memblock_ptrc_p == nullptr, "ERROR: source_memblock_ptrc_p is nullptr.");

	ABORT_IF(dest_bytes_p == 0, "ERROR: dest_bytes_p is 0.");
	ABORT_IF(source_bytes_p == 0, "ERROR: source_bytes_p is 0.");

	if (source_bytes_p >= dest_bytes_p)
	{
		ALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, dest_bytes_p);
	}
	else
	{
		ALIGNED_MEMCPY(dest_memblock_ptrc_p, source_memblock_ptrc_p, source_bytes_p);
	}
}




enum struct MEMORY_SIZE_SCALABILITY : boolean
{
	_STATICALLY_SIZED = false,
	_DYNAMICALLY_SIZED = true
};

enum struct MEMORY_ERROR_1XX : int16
{
	_NONE = 0,
	_ERROR_INVALID_SIZE = 100,
	_FATAL_ERROR_NULLPTR = 101,
	_FATAL_ERROR_OUT_OF_RANGE = 102,
	_FATAL_ERROR_ACCESS_VIOLATION = 103
};

struct total_memory_utilization_data
{
	var::size_t _global_total_bytes = 0;
	var::size_t _thread_local_total_bytes = 0;

	var::size_t _global_total_bytes_by_type = 0;
	var::size_t _thread_local_total_bytes_by_type = 0;
};

struct global_memory_utilization
{
	var::size_t _global_total_bytes = 0;
	var::size_t _thread_local_total_bytes = 0;
};

struct type_memory_utilization
{
	var::size_t _global_total_bytes_by_type = 0;
	var::size_t _thread_local_total_bytes_by_type = 0;
};


_MAYBE_UNUSED_ constexpr uint8 _BYTE_SIZE_ = 1;
_MAYBE_UNUSED_ constexpr uint8 _WORD_SIZE_ = 2;
_MAYBE_UNUSED_ constexpr uint8 _DWORD_SIZE_ = 4;
_MAYBE_UNUSED_ constexpr uint8 _QWORD_SIZE_ = 8;


// it is used when reserving memory of Frogman Engine data containers.
struct reserve final
{
	var::uint64 _length = 0;
};

// it contains memory padding size.
struct align_null final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 0;
};

// it contains memory padding size.
struct align_2bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 2;
};

// it contains memory padding size.
struct align_4bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 4;
};

// it contains memory padding size.
struct align_8bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 8;
};

// it contains memory padding size.
struct align_16bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 16;
};

// it contains memory padding size.
struct align_32bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 32;
};

// it contains memory padding size.
struct align_64bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 64;
};

// it contains memory padding size.
struct align_128bytes final
{
	_MAYBE_UNUSED_ static constexpr uint16 size = 128;
};


// it contains memory padding size.
template<uint64 padding_size>
struct align_custom_bytes final
{
	_MAYBE_UNUSED_ static constexpr inline uint16 size = padding_size;
};


template <typename T, class alignment = align_null>
struct alignas(alignment::size) align
{
	T _data;
};


END_NAMESPACE




BEGIN_NAMESPACE(FE)


template<class iterator>
var::boolean memcmp_s(iterator left_iterator_begin_p, iterator left_iterator_end_p, iterator right_iterator_begin_p, iterator right_iterator_end_p) noexcept
{
	static_assert(std::is_class<iterator>::value == true);
	FE_ASSERT(left_iterator_begin_p == nullptr, "ERROR: left_iterator_begin_p is nullptr.");
	FE_ASSERT(left_iterator_end_p == nullptr, "ERROR: left_iterator_end_p is nullptr.");
	FE_ASSERT(right_iterator_begin_p == nullptr, "ERROR: right_iterator_begin_p is nullptr.");
	FE_ASSERT(right_iterator_end_p == nullptr, "ERROR: right_iterator_end_p is nullptr.");

	iterator l_left_iterator_begin = left_iterator_begin_p;

	if ((left_iterator_end_p - left_iterator_begin_p) != (right_iterator_end_p - right_iterator_begin_p))
	{
		return false;
	}

	while ((*l_left_iterator_begin == *right_iterator_begin_p) && (l_left_iterator_begin != left_iterator_end_p))
	{
		++l_left_iterator_begin;
		++right_iterator_begin_p;
	}

	if ((l_left_iterator_begin - left_iterator_begin_p) == (left_iterator_end_p - left_iterator_begin_p))
	{
		return true;
	}

	return false;
}




template <class T, typename ... arguments>
_FORCE_INLINE_ void assign(T& dest_object_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p, arguments&& ...arguments_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	if (dest_bool_mask_ref_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED)
	{
		new(&dest_object_ref_p) T(arguments_p...);
        dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
		return;
	}

	dest_object_ref_p = ::std::move(arguments_p...);
}

template <class T>
_FORCE_INLINE_ void copy_assign(T& dest_object_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p, const T& source_cref_p, _MAYBE_UNUSED_ OBJECT_LIFECYCLE source_bool_mask_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(source_bool_mask_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED, "ERROR: cannot copy a null object");

	if (dest_bool_mask_ref_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED)
	{
		new(&dest_object_ref_p) T(source_cref_p);
		dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
		return;
	}

	dest_object_ref_p = source_cref_p;
}

template <class T>
_FORCE_INLINE_ void move_assign(T& dest_object_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p, T&& source_rvalue_reference_p, _MAYBE_UNUSED_ OBJECT_LIFECYCLE source_bool_mask_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(source_bool_mask_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED, "ERROR: cannot copy a null object");

	if (dest_bool_mask_ref_p == OBJECT_LIFECYCLE::_DESTRUCTED)
	{
		new(&dest_object_ref_p) T(std::move(source_rvalue_reference_p));
		dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
		return;
	}

	dest_object_ref_p = std::move(source_rvalue_reference_p);
}




template <class T>
_FORCE_INLINE_ void construct(T& dest_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ref_p == FE::OBJECT_LIFECYCLE::_CONSTRUCTED, "ERROR: unable to double-construct.");

	new(&dest_ref_p) T();
	dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
}

template <class T>
_FORCE_INLINE_ void copy_construct(T& dest_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p, const T& source_cref_p, _MAYBE_UNUSED_ OBJECT_LIFECYCLE source_bool_mask_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ref_p == FE::OBJECT_LIFECYCLE::_CONSTRUCTED, "ERROR: unable to double-construct.");
	FE_ASSERT(source_bool_mask_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED, "ERROR: unable to copy null object.");

	new(&dest_ref_p) T(source_cref_p);
	dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
}

template <class T>
_FORCE_INLINE_ void move_construct(T& dest_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p, T&& source_rvalue_reference_p, _MAYBE_UNUSED_ OBJECT_LIFECYCLE source_bool_mask_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ref_p == FE::OBJECT_LIFECYCLE::_CONSTRUCTED, "ERROR: unable to double-construct.");
	FE_ASSERT(source_bool_mask_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED, "ERROR: unable to copy null object.");

	new(&dest_ref_p) T(source_rvalue_reference_p);
	dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
}




template <typename iterator, typename ... arguments>
_FORCE_INLINE_ void assign(iterator begin_p, iterator end_p, OBJECT_LIFECYCLE* const boolean_mask_ptrc_p, arguments && ...arguments_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(boolean_mask_ptrc_p == nullptr, "ERROR: boolean_mask_ptrc_p is nullptr.");
	FE_ASSERT(begin_p == nullptr, "ERROR: begin_p is nullptr.");
	FE_ASSERT(end_p == nullptr, "ERROR: end_p is nullptr.");

	OBJECT_LIFECYCLE* l_boolean_mask_ptr = boolean_mask_ptrc_p;

	for (iterator it = begin_p; it != end_p; ++it)
	{
		if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
		{
			*it = std::move(arguments_p...);
			++l_boolean_mask_ptr;
		}
		else
		{
			new(it.operator->()) T(arguments_p...);
			*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
			++l_boolean_mask_ptr;
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void copy_assign(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator data_source_begin_p, capacity_t source_data_length_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_copy_assignable<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(data_source_begin_p == nullptr, "ERROR: data_source_begin_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_boolean_mask_ptr = dest_bool_mask_ptrc_p;
	iterator l_initializer_list_begin = data_source_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = *l_initializer_list_begin;

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
			else
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = *l_initializer_list_begin;

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
			else
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void move_assign(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator data_source_begin_p, capacity_t source_data_length_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_move_assignable<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(data_source_begin_p == nullptr, "ERROR: data_source_begin_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_boolean_mask_ptr = dest_bool_mask_ptrc_p;
	iterator l_initializer_list_begin = data_source_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = std::move(*l_initializer_list_begin);

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
			else
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = std::move(*l_initializer_list_begin);

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
			else
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void copy_assign(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator source_data_begin_p, capacity_t source_data_length_p, const OBJECT_LIFECYCLE* const source_data_bool_mask_ptrc_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_copy_assignable<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(source_data_begin_p == nullptr, "ERROR: source_data_begin_p is nullptr.");
	FE_ASSERT(source_data_bool_mask_ptrc_p == nullptr, "ERROR: source_data_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_dest_bool_mask_ptr = dest_bool_mask_ptrc_p;
	const OBJECT_LIFECYCLE* l_source_bool_mask_ptr = source_data_bool_mask_ptrc_p;

	iterator l_initializer_list_begin = source_data_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = *l_initializer_list_begin;

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
			else if (*l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = *l_initializer_list_begin;

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
			else if (*l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void move_assign(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator source_data_begin_p, capacity_t source_data_length_p, OBJECT_LIFECYCLE* const source_data_bool_mask_ptrc_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_move_assignable<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(source_data_begin_p == nullptr, "ERROR: source_data_begin_p is nullptr.");
	FE_ASSERT(source_data_bool_mask_ptrc_p == nullptr, "ERROR: source_data_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_dest_bool_mask_ptr = dest_bool_mask_ptrc_p;
	OBJECT_LIFECYCLE* l_source_bool_mask_ptr = source_data_bool_mask_ptrc_p;

	iterator l_initializer_list_begin = source_data_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = std::move(*l_initializer_list_begin);

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
			else if(*l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				*l_dest_iterator_begin = std::move(*l_initializer_list_begin);

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
			else if(*l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
}




template <class T>
_FORCE_INLINE_ void destruct(T& dest_ref_p, OBJECT_LIFECYCLE& dest_bool_mask_ref_p) noexcept
{
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ref_p == FE::OBJECT_LIFECYCLE::_DESTRUCTED, "ERROR: unable to destruct.");

	dest_ref_p.~T();
	dest_bool_mask_ref_p = FE::OBJECT_LIFECYCLE::_DESTRUCTED;
}

template<class iterator>
_FORCE_INLINE_ void destruct(iterator begin_p, iterator end_p, OBJECT_LIFECYCLE* const boolean_mask_ptrc_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_destructible<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(boolean_mask_ptrc_p == nullptr, "ERROR: boolean_mask_ptrc_p is nullptr.");
	FE_ASSERT(begin_p == nullptr, "ERROR: begin_p is nullptr.");
	FE_ASSERT(end_p == nullptr, "ERROR: end_p is nullptr.");

	OBJECT_LIFECYCLE* l_boolean_mask_ptr = boolean_mask_ptrc_p;

	for (iterator it = begin_p; it != end_p; ++it)
	{
		if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
		{
			it->~T();
			*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_DESTRUCTED;
			++l_boolean_mask_ptr;
		}
	}
}




template<class iterator>
_FORCE_INLINE_ void copy_construct(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator data_source_begin_p, capacity_t source_data_length_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_copy_constructible<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(data_source_begin_p == nullptr, "ERROR: data_source_begin_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_boolean_mask_ptr = dest_bool_mask_ptrc_p;
	iterator l_initializer_list_begin = data_source_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void move_construct(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator data_source_begin_p, capacity_t source_data_length_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_move_constructible<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(data_source_begin_p == nullptr, "ERROR: data_source_begin_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_boolean_mask_ptr = dest_bool_mask_ptrc_p;
	iterator l_initializer_list_begin = data_source_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_boolean_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_boolean_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_boolean_mask_ptr;
				++l_initializer_list_begin;
				++l_dest_iterator_begin;
			}
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void copy_construct(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator source_data_begin_p, capacity_t source_data_length_p, const OBJECT_LIFECYCLE* const source_data_bool_mask_ptrc_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_copy_constructible<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(source_data_begin_p == nullptr, "ERROR: source_data_begin_p is nullptr.");
	FE_ASSERT(source_data_bool_mask_ptrc_p == nullptr, "ERROR: source_data_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_dest_bool_mask_ptr = dest_bool_mask_ptrc_p;
	const OBJECT_LIFECYCLE* l_source_bool_mask_ptr = source_data_bool_mask_ptrc_p;

	iterator l_initializer_list_begin = source_data_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(*l_initializer_list_begin);
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
}

template<class iterator>
_FORCE_INLINE_ void move_construct(iterator dest_begin_p, capacity_t dest_length_p, OBJECT_LIFECYCLE* const dest_bool_mask_ptrc_p, iterator source_data_begin_p, capacity_t source_data_length_p, OBJECT_LIFECYCLE* const source_data_bool_mask_ptrc_p) noexcept
{
	using T = typename iterator::value_type;
	static_assert(std::is_class<iterator>::value == true);
	static_assert(std::is_move_constructible<T>::value == true);
	static_assert(FE::is_trivially_constructible_and_destructible<T>::_VALUE_ == FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL, "WARNING: T must not be trivially constructible and destructible. This function call has no effect and is a waste of computing resource");
	FE_ASSERT(dest_bool_mask_ptrc_p == nullptr, "ERROR: dest_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(source_data_begin_p == nullptr, "ERROR: source_data_begin_p is nullptr.");
	FE_ASSERT(source_data_bool_mask_ptrc_p == nullptr, "ERROR: source_data_bool_mask_ptrc_p is nullptr.");
	FE_ASSERT(dest_begin_p == nullptr, "ERROR: dest_begin_p is nullptr.");

	OBJECT_LIFECYCLE* l_dest_bool_mask_ptr = dest_bool_mask_ptrc_p;
	OBJECT_LIFECYCLE* l_source_bool_mask_ptr = source_data_bool_mask_ptrc_p;

	iterator l_initializer_list_begin = source_data_begin_p;
	iterator l_dest_iterator_begin = dest_begin_p;

	if (source_data_length_p >= dest_length_p)
	{
		for (var::index_t i = 0; i < dest_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
	else
	{
		for (var::index_t i = 0; i < source_data_length_p; ++i)
		{
			if (*l_dest_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_DESTRUCTED && *l_source_bool_mask_ptr == FE::OBJECT_LIFECYCLE::_CONSTRUCTED)
			{
				new(l_dest_iterator_begin.operator->()) T(std::move(*l_initializer_list_begin));
				*l_dest_bool_mask_ptr = FE::OBJECT_LIFECYCLE::_CONSTRUCTED;

				++l_initializer_list_begin;
				++l_dest_iterator_begin;

				++l_dest_bool_mask_ptr;
				++l_source_bool_mask_ptr;
			}
		}
	}
}




END_NAMESPACE

#endif
