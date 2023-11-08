#ifndef _FE_CORE_POOL_ALLOCATOR_HXX_
#define _FE_CORE_POOL_ALLOCATOR_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/pool.hxx>




BEGIN_NAMESPACE(FE)


template <typename T, size_t ChunkCapacity = 40960, class Alignment = FE::align_8bytes, class Allocator = FE::std_style::scalable_aligned_allocator<chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>>>
class new_delete_pool_allocator final
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
	_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = ADDRESS::_NOT_ALIGNED;


	_NODISCARD_ _FORCE_INLINE_ static pointer allocate(const size_type count_p) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

		return FE::generic_pool<ChunkCapacity, Allocator>::allocate<value_type, Alignment>(count_p).release();
	}


	_NODISCARD_ _FORCE_INLINE_ static pointer reallocate(pointer const pointer_p, const size_type prev_count_p, const size_type new_count_p) noexcept
	{
		if (new_count_p == 0)
		{
			new_delete_pool_allocator<T, ChunkCapacity, Alignment, Allocator>::deallocate(pointer_p, prev_count_p);
			return nullptr;
		}

		pointer l_result = (T*)new_delete_pool_allocator<std::byte, ChunkCapacity, Alignment, Allocator>::allocate(sizeof(T) * new_count_p);
	
		if (l_result != pointer_p) _LIKELY_
		{
			FE::memcpy<ADDRESS::_NOT_ALIGNED, ADDRESS::_NOT_ALIGNED>(l_result, new_count_p * sizeof(value_type), pointer_p, prev_count_p * sizeof(value_type));
			new_delete_pool_allocator<std::byte, ChunkCapacity, Alignment, Allocator>::deallocate(reinterpret_cast<std::byte*>(pointer_p), sizeof(T) * prev_count_p);
		}

		return l_result;
	}


	_FORCE_INLINE_ static void deallocate(pointer const pointer_p, _MAYBE_UNUSED_ const size_type count_p) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
		FE_ASSERT(pointer_p == nullptr, "${%s@0}: attempted to delete nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		FE::generic_pool<ChunkCapacity, Allocator>::deallocate<value_type, Alignment>(pointer_p, count_p);
	}
};




template <typename T, size_t ChunkCapacity = 40960, class Alignment = FE::align_8bytes, class Allocator = FE::std_style::scalable_aligned_allocator<chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>>>
class pool_allocator final
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
	_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = ADDRESS::_NOT_ALIGNED;


	_NODISCARD_ _FORCE_INLINE_ static pointer allocate(const size_type count_p) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

		return (T*)FE::generic_pool<ChunkCapacity, Allocator>::allocate<std::byte, Alignment>(sizeof(T) * count_p).release();
	}


	_NODISCARD_ _FORCE_INLINE_ static pointer reallocate(pointer const pointer_p, const size_type prev_count_p, const size_type new_count_p) noexcept
	{
		if (new_count_p == 0)
		{
			FE::pool_allocator<T, ChunkCapacity, Alignment, Allocator>::deallocate(pointer_p, sizeof(T) * prev_count_p);
			return nullptr;
		}

		pointer l_result = FE::pool_allocator<T, ChunkCapacity, Alignment, Allocator>::allocate(sizeof(T) * new_count_p);

		if (l_result != pointer_p) _LIKELY_
		{
			FE::memcpy<ADDRESS::_NOT_ALIGNED, ADDRESS::_NOT_ALIGNED>(l_result, sizeof(T) * new_count_p, pointer_p, sizeof(T) * prev_count_p);
			FE::pool_allocator<T, ChunkCapacity, Alignment, Allocator>::deallocate(pointer_p, sizeof(T) * prev_count_p);
		}

		return l_result;
	}


	_FORCE_INLINE_ static void deallocate(pointer const pointer_p, _MAYBE_UNUSED_ const size_type count_p) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
		FE_ASSERT(pointer_p == nullptr, "${%s@0}: attempted to delete nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		FE::generic_pool<ChunkCapacity, Allocator>::deallocate<std::byte, Alignment>(reinterpret_cast<std::byte* const>(pointer_p), sizeof(T) * count_p);
	}
};




namespace std_style
{
	template <typename T, class ChunkCapacity = capacity<40960>, class Alignment = FE::align_8bytes, class Allocator = FE::std_style::scalable_aligned_allocator<chunk<void, POOL_TYPE::_GENERIC, capacity<40960>::size>>>
	class new_delete_pool_allocator final
	{
	public:
		using allocator = FE::new_delete_pool_allocator<T, ChunkCapacity::size, Alignment, Allocator>;
		using value_type = typename allocator::value_type;
		using pointer = typename allocator::pointer;
		using const_pointer = typename allocator::const_pointer;
		using reference = typename allocator::reference;
		using const_reference = typename allocator::const_reference;
		using difference_type = typename allocator::difference_type;
		using size_type = typename allocator::size_type;

		_MAYBE_UNUSED_ static constexpr inline auto is_trivial = FE::is_trivial<value_type>::value;
		_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = ADDRESS::_NOT_ALIGNED;


		constexpr new_delete_pool_allocator() noexcept {}

		template <typename U>
		constexpr new_delete_pool_allocator(_MAYBE_UNUSED_ const new_delete_pool_allocator<U>& standard_allocator_p) noexcept {}


		_NODISCARD_ _FORCE_INLINE_ pointer allocate(const size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			return FE::generic_pool<ChunkCapacity::size, Allocator>::allocate<value_type, Alignment>(count_p).release();
		}


		_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer const pointer_p, const size_type prev_count_p, const size_type new_count_p) noexcept
		{
			if (new_count_p == 0)
			{
				FE::new_delete_pool_allocator<T, ChunkCapacity::size, Alignment, Allocator>::deallocate(pointer_p, prev_count_p);
				return nullptr;
			}

			pointer l_result = (T*)FE::new_delete_pool_allocator<std::byte, ChunkCapacity::size, Alignment, Allocator>::allocate(sizeof(T) * new_count_p);

			if (l_result != pointer_p) _LIKELY_
			{
				FE::memcpy<ADDRESS::_NOT_ALIGNED, ADDRESS::_NOT_ALIGNED>(l_result, new_count_p * sizeof(value_type), pointer_p, prev_count_p * sizeof(value_type));
				FE::new_delete_pool_allocator<std::byte, ChunkCapacity::size, Alignment, Allocator>::deallocate(reinterpret_cast<std::byte*>(pointer_p), sizeof(T) * prev_count_p);
			}

			return l_result;
		}


		_FORCE_INLINE_ void deallocate(pointer const pointer_p, _MAYBE_UNUSED_ const size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: attempted to delete nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

			FE::generic_pool<ChunkCapacity::size, Allocator>::deallocate<value_type, Alignment>(pointer_p, count_p);
		}
	};




	template <typename T, class ChunkCapacity = capacity<40960>, class Alignment = FE::align_8bytes, class Allocator = FE::std_style::scalable_aligned_allocator<chunk<void, POOL_TYPE::_GENERIC, capacity<40960>::size>>>
	class pool_allocator final
	{
	public:
		using allocator = FE::pool_allocator<T, ChunkCapacity::size, Alignment, Allocator>;
		using value_type = typename allocator::value_type;
		using pointer = typename allocator::pointer;
		using const_pointer = typename allocator::const_pointer;
		using reference = typename allocator::reference;
		using const_reference = typename allocator::const_reference;
		using difference_type = typename allocator::difference_type;
		using size_type = typename allocator::size_type;

		_MAYBE_UNUSED_ static constexpr inline auto is_trivial = FE::is_trivial<value_type>::value;
		_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = ADDRESS::_NOT_ALIGNED;


		constexpr pool_allocator() noexcept {}

		template <typename U>
		constexpr pool_allocator(_MAYBE_UNUSED_ const pool_allocator<U>& standard_allocator_p) noexcept {}


		_NODISCARD_ _FORCE_INLINE_ pointer allocate(const size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			return (T*)FE::generic_pool<ChunkCapacity::size, Allocator>::allocate<std::byte, Alignment>(sizeof(T) * count_p).release();
		}


		_NODISCARD_ _FORCE_INLINE_ pointer reallocate(pointer const pointer_p, const size_type prev_count_p, const size_type new_count_p) noexcept
		{
			if (new_count_p == 0)
			{
				FE::pool_allocator<T, ChunkCapacity::size, Alignment, Allocator>::deallocate(pointer_p, sizeof(T) * prev_count_p);
				return nullptr;
			}

			pointer l_result = FE::pool_allocator<T, ChunkCapacity::size, Alignment, Allocator>::allocate(sizeof(T) * new_count_p);

			if (l_result != pointer_p) _LIKELY_
			{
				FE::memcpy<ADDRESS::_NOT_ALIGNED, ADDRESS::_NOT_ALIGNED>(l_result, sizeof(T) * new_count_p, pointer_p, sizeof(T) * prev_count_p);
				FE::pool_allocator<T, ChunkCapacity::size, Alignment, Allocator>::deallocate(pointer_p, sizeof(T) * prev_count_p);
			}

			return l_result;
		}


		_FORCE_INLINE_ void deallocate(pointer const pointer_p, _MAYBE_UNUSED_ const size_type count_p) noexcept
		{
			FE_ASSERT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
			FE_ASSERT(pointer_p == nullptr, "${%s@0}: attempted to delete nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

			FE::generic_pool<ChunkCapacity::size, Allocator>::deallocate<std::byte, Alignment>(reinterpret_cast<std::byte* const>(pointer_p), sizeof(T) * count_p);
		}
	};
}


END_NAMESPACE
#endif