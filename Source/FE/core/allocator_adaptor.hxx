﻿#ifndef _FE_CORE_ALLOCATOR_ADAPTORS_HXX_
#define _FE_CORE_ALLOCATOR_ADAPTORS_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/allocator.hxx>




BEGIN_NAMESPACE(FE)


namespace std_style 
{
	template <class Implementation>
	class new_delete_proxy_allocator final
	{
	public:
		using value_type = typename Implementation::value_type;
		using pointer = typename Implementation::pointer;
		using const_pointer = const typename Implementation::const_pointer;
		using reference = typename Implementation::reference;
		using const_reference = const typename Implementation::const_reference;
		using size_type = typename Implementation::size_type;
		using allocator = Implementation;

		_MAYBE_UNUSED_ static constexpr inline auto is_trivially_constructible_and_destructible = FE::is_trivially_constructible_and_destructible<value_type>::value;
		_MAYBE_UNUSED_ static constexpr inline boolean is_allocated_from_an_address_aligned_allocator = Implementation::is_allocated_from_an_address_aligned_allocator;


		constexpr new_delete_proxy_allocator() noexcept {}

		template <typename another_implementation>
		constexpr new_delete_proxy_allocator(_MAYBE_UNUSED_ const new_delete_proxy_allocator<another_implementation>& standard_allocator_p) noexcept {}


		_NODISCARD_ _FORCE_INLINE_ pointer allocate(size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			pointer l_result = allocator::allocate(count_p);

			if constexpr (is_trivially_constructible_and_destructible == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
			{
				new(l_result) value_type[count_p]{};
			}
			return l_result;
		}

		_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer const pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
		{
			pointer l_result = allocator::reallocate(pointer_p, prev_count_p, new_count_p);

			if constexpr (is_trivially_constructible_and_destructible == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
			{
				size_type l_size_difference;
				if (new_count_p > prev_count_p)
				{
					l_size_difference = new_count_p - prev_count_p;
					new(l_result + prev_count_p) value_type[l_size_difference]{};
				}
				else
				{
					l_size_difference = prev_count_p - new_count_p;
					pointer l_end = pointer_p + prev_count_p;
					for (pointer begin = pointer_p + new_count_p; begin != l_end; ++begin)
					{
						begin->~value_type();
					}
				}
			}

			return l_result;
		}

		_FORCE_INLINE_ void deallocate(pointer const pointer_p, size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: attempted to delete ${%p@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), pointer_p);

			if constexpr (is_trivially_constructible_and_destructible == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
			{
				pointer const l_end = pointer_p + count_p;
				for (pointer begin = pointer_p; begin != l_end; ++begin)
				{
					begin->~value_type();
				}
			}
			allocator::deallocate(pointer_p, count_p);
		}
	};

	
#ifdef _AVX512_
	template<typename T, class Alignment = align_64bytes>
#elif defined(_AVX_)
	template<typename T, class Alignment = align_32bytes>
#endif
	class scalable_aligned_allocator final // trackable scalable_aligned_allocator
	{
	public:
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = var::size_t;
		using allocator = FE::scalable_aligned_allocator<T, Alignment>;

		_MAYBE_UNUSED_ static constexpr inline auto is_trivially_constructible_and_destructible = FE::is_trivially_constructible_and_destructible<value_type>::value;
		_MAYBE_UNUSED_ static constexpr inline boolean is_allocated_from_an_address_aligned_allocator = true;


		constexpr scalable_aligned_allocator() noexcept {}

		template <typename U>
		constexpr scalable_aligned_allocator(_MAYBE_UNUSED_ const scalable_aligned_allocator<U, Alignment>& standard_allocator_p) noexcept {}


		_NODISCARD_ _FORCE_INLINE_ pointer allocate(size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			return allocator::allocate(count_p);
		}

		_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
		{
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: ${%s@1} is nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(pointer_p));

			return allocator::reallocate(pointer_p, prev_count_p, new_count_p);
		}

		_FORCE_INLINE_ void deallocate(pointer const pointer_p, size_type count_p) noexcept
		{
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: ${%s@1} is nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(pointer_p));
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			allocator::deallocate(pointer_p, count_p);
		}
	};

	// trackable cache_aligned_allocator
	template<typename T>
	class cache_aligned_allocator final
	{
	public:
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = var::size_t;
		using allocator = FE::cache_aligned_allocator<T>;

		_MAYBE_UNUSED_ static constexpr inline auto is_trivially_constructible_and_destructible = FE::is_trivially_constructible_and_destructible<value_type>::value;
		_MAYBE_UNUSED_ static constexpr inline boolean is_allocated_from_an_address_aligned_allocator = true;


		constexpr cache_aligned_allocator() noexcept {}

		template <typename U>
		constexpr cache_aligned_allocator(_MAYBE_UNUSED_ const cache_aligned_allocator<U>& cache_aligned_allocator_p) noexcept {}


		_NODISCARD_ _FORCE_INLINE_ pointer allocate(size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			return allocator::allocate(count_p);
		}

		_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer const pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
		{
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: ${%s@1} is nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(pointer_p));

			return allocator::reallocate(pointer_p, prev_count_p, new_count_p);
		}

		_FORCE_INLINE_ void deallocate(pointer const pointer_p, size_type count_p) noexcept
		{
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: ${%s@1} is nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(pointer_p));
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			allocator::deallocate(pointer_p, count_p);
		}
	};
}


END_NAMESPACE
#endif
