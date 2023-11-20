#ifndef _FE_CORE_ALLOCATOR_HXX_
#define _FE_CORE_ALLOCATOR_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <tbb/cache_aligned_allocator.h>
#include <tbb/scalable_allocator.h>
#include <FE/core/heap_memory.hpp>




BEGIN_NAMESPACE(FE)

// FE::pool_allocator and FE::namespace_pool_allocator must not be used an Implementation of this class to replace FE::new_delete_pool_allocator and FE::new_delete_namespace_pool_allocator.
template <class Implementation>
class new_delete_proxy_allocator final
{
public:
	using value_type = typename Implementation::value_type;
	using pointer = typename Implementation::pointer;
	using const_pointer = typename Implementation::const_pointer;
	using reference = typename Implementation::reference;
	using const_reference = typename Implementation::const_reference;
	using size_type = typename Implementation::size_type;
	using difference_type = typename Implementation::difference_type;
	using allocator = Implementation;

	_MAYBE_UNUSED_ static constexpr inline auto is_trivial = FE::is_trivial<value_type>::value;
	_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = Implementation::is_address_aligned;

private:
	static allocator s_allocator;

public:
	constexpr new_delete_proxy_allocator() noexcept {}

	template <typename AnotherImplementation = Implementation>
	constexpr new_delete_proxy_allocator(_MAYBE_UNUSED_ const new_delete_proxy_allocator<AnotherImplementation>& other_p) noexcept {}


	_NODISCARD_ _FORCE_INLINE_ pointer allocate(size_type count_p) noexcept
	{
		FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

		pointer const l_result = s_allocator.allocate(count_p);

		if constexpr (is_trivial == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
		{
			pointer const l_end = l_result + count_p;
			for (pointer begin = l_result; begin != l_end; ++begin)
			{
				new(begin) value_type();
			}
		}
		return l_result;
	}


	_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
	{
		pointer const l_result = s_allocator.reallocate(pointer_p, prev_count_p, new_count_p);

		if constexpr (is_trivial == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
		{
			if (new_count_p > prev_count_p)
			{
				pointer const l_end = l_result + new_count_p;
				for (pointer begin = l_result + prev_count_p; begin != l_end; ++begin)
				{
					new(begin) value_type();
				}
			}
			else
			{
				pointer const l_end = pointer_p + prev_count_p;
				for (pointer begin = pointer_p + new_count_p; begin != l_end; ++begin)
				{
					begin->~value_type();
				}
			}
		}

		return l_result;
	}


	_FORCE_INLINE_ void deallocate(pointer pointer_p, size_type count_p) noexcept
	{
		FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
		FE_SUSPECT(pointer_p == nullptr, "${%s@0}: attempted to delete ${%p@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), pointer_p);
		
		if constexpr (is_trivial == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
		{
			pointer const l_end = pointer_p + count_p;
			for (pointer begin = pointer_p; begin != l_end; ++begin)
			{
				begin->~value_type();
			}
		}
		s_allocator.deallocate(pointer_p, count_p);
	}
};

template <class Implementation>
typename new_delete_proxy_allocator<Implementation>::allocator new_delete_proxy_allocator<Implementation>::s_allocator;




template<typename T, class Alignment = typename FE::SIMD_auto_alignment::alignment_type>
class scalable_aligned_allocator final
{
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using reference = value_type&;
	using const_reference = const reference;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;

	_MAYBE_UNUSED_ static constexpr inline auto is_trivial = FE::is_trivial<value_type>::value;
	_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = (std::is_same<FE::SIMD_auto_alignment::alignment_type, Alignment>::value == true) ? ADDRESS::_ALIGNED : ADDRESS::_NOT_ALIGNED;


	constexpr scalable_aligned_allocator() noexcept {}

	template <typename U = T>
	constexpr scalable_aligned_allocator(_MAYBE_UNUSED_ const scalable_aligned_allocator<U, Alignment>& other_p) noexcept {}


	_NODISCARD_ _FORCE_INLINE_ pointer allocate(size_type count_p) noexcept
	{
		FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

		return ::FE::trackable_calloc<value_type, Alignment>(count_p, sizeof(value_type));
	}


	_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
	{
		if (new_count_p == 0)
		{
			deallocate(pointer_p, prev_count_p);
			return nullptr;
		}

		return ::FE::trackable_realloc<value_type, Alignment>(pointer_p, prev_count_p * sizeof(value_type), new_count_p * sizeof(value_type));
	}


	_FORCE_INLINE_ void deallocate(pointer pointer_p, size_type count_p) noexcept
	{
		FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
		FE_SUSPECT(pointer_p == nullptr, "${%s@0}: attempted to delete ${%p@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), pointer_p);
		
		::FE::trackable_free<value_type, Alignment>(pointer_p, count_p, sizeof(value_type));
	}
};




template <typename T>
class cache_aligned_allocator final
{
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using reference = value_type&;
	using const_reference = const reference;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;

	_MAYBE_UNUSED_ static constexpr inline auto is_trivial = FE::is_trivial<value_type>::value;
	_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = ADDRESS::_ALIGNED;

	FE_STATIC_SUSPICION((FE::align_CPU_L1_cache_line::size % FE::SIMD_auto_alignment::alignment_type::size) != 0, "Static Assertion Failed: SIMD address aligned operations are incompatible with this machine.");


	constexpr cache_aligned_allocator() noexcept {}

	template <typename U = T>
	constexpr cache_aligned_allocator(_MAYBE_UNUSED_ const cache_aligned_allocator<U>& other_p) noexcept {}


	_NODISCARD_ _FORCE_INLINE_ pointer allocate(size_type count_p) noexcept
	{
		FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

		pointer const l_result = static_cast<pointer>(::tbb::detail::r1::cache_aligned_allocate(count_p * sizeof(value_type)));
		FE_SUSPECT(l_result == nullptr, "${%s@0}: failed to allocate memory from cache_aligned_allocator.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		FE_SUSPECT(MODULO_BY_64(reinterpret_cast<uintptr_t>(l_result)) != 0, "${%s@0}: The allocated heap memory address not aligned by sixty-four. The address value was ${%p@1}", TO_STRING(MEMORY_ERROR_1XX::_ERROR_ILLEGAL_ADDRESS_ALIGNMENT), l_result);

#if defined(_ENABLE_MEMORY_TRACKER_)
		::FE::heap_memory_tracker<T, align_64bytes>::__log_heap_memory_allocation(count_p * sizeof(value_type), TO_STRING(cache_aligned_allocate), l_result, typeid(T).name());
#endif
		return l_result;
	}
	

	_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
	{
		if (new_count_p == 0)
		{
			deallocate(pointer_p, prev_count_p);
			return nullptr;
		}

		pointer const l_result = static_cast<pointer>(cache_aligned_allocator<T>::allocate(new_count_p));
		FE_SUSPECT(MODULO_BY_64(reinterpret_cast<uintptr_t>(l_result)) != 0, "${%s@0}: The allocated heap memory address not aligned by sixty-four. The address value was ${%p@1}", TO_STRING(MEMORY_ERROR_1XX::_ERROR_ILLEGAL_ADDRESS_ALIGNMENT), l_result);
		 
		if (l_result != pointer_p) _LIKELY_
		{
			FE::memcpy<ADDRESS::_ALIGNED, ADDRESS::_ALIGNED>(l_result, new_count_p * sizeof(value_type), pointer_p, prev_count_p * sizeof(value_type));
			cache_aligned_allocator<value_type>::deallocate(pointer_p, prev_count_p);
		}
		return l_result;
	}


	_FORCE_INLINE_ void deallocate(pointer pointer_p, _MAYBE_UNUSED_ size_type count_p) noexcept
	{
		FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
		FE_SUSPECT(pointer_p == nullptr, "${%s@0}: attempted to delete ${%p@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), pointer_p);

#if defined(_ENABLE_MEMORY_TRACKER_)
		::FE::heap_memory_tracker<T, align_64bytes>::__log_heap_memory_deallocation(count_p * sizeof(value_type), TO_STRING(cache_aligned_deallocate), pointer_p, typeid(T).name());
#endif 
		tbb::detail::r1::cache_aligned_deallocate(pointer_p);
	}
};


END_NAMESPACE
#endif