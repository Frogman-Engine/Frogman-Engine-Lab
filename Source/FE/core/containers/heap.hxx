#ifndef _FE_CORE_HEAP_HXX_
#define _FE_CORE_HEAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/smart_pointers/unique_ptr.hxx>
#include <tbb/memory_pool.h>




BEGIN_NAMESPACE(FE)


namespace internal::heap
{
	enum struct SHOUD_NODES_BE_POOLED
	{
		_POSITIVE,
		_NEGATIVE
	};

	constexpr SHOUD_NODES_BE_POOLED evaluate_type_size(size_t size_in_bytes_p) noexcept
	{
		if (size_in_bytes_p > 1024)
		{
			return SHOUD_NODES_BE_POOLED::_NEGATIVE;
		}
		
		return SHOUD_NODES_BE_POOLED::_POSITIVE;
	}

	template<class T>
	struct node
	{
		T _data;
		FE::ref<node> _parent;
		FE::ref<node> _left;
		FE::ref<node> _right;
	};

	template<class StatefulAllocator>
	class tbb_memory_pool_allocator final
	{
	public:
		using allocator = StatefulAllocator;
		using value_type = typename allocator::value_type;
		using pointer = typename allocator::pointer;
		using const_pointer = typename allocator::const_pointer;
		using reference = typename allocator::reference;
		using const_reference = typename allocator::const_reference;
		using difference_type = typename allocator::difference_type;
		using size_type = typename allocator::size_type;


		_MAYBE_UNUSED_ static constexpr inline auto is_trivial = FE::is_trivial<value_type>::value;
		_MAYBE_UNUSED_ static constexpr inline ADDRESS is_address_aligned = StatefulAllocator::is_address_aligned;
		
		thread_local static tbb::memory_pool<StatefulAllocator> tl_s_memory_pool;
		thread_local static tbb::memory_pool_allocator<value_type> tl_s_memory_pool_allocator;


		_NODISCARD_ _FORCE_INLINE_ static pointer allocate(size_type count_p) noexcept
		{
			FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);

			return tl_s_memory_pool_allocator.allocate(count_p);
		}

		_NODISCARD_ _FORCE_INLINE_ static pointer reallocate(pointer pointer_p, size_type prev_count_p, size_type new_count_p) noexcept
		{
			if (new_count_p == 0)
			{
				tl_s_memory_pool_allocator.deallocate(pointer_p, prev_count_p);
				return nullptr;
			}

			pointer const l_result = static_cast<pointer>(tl_s_memory_pool_allocator.allocate(new_count_p));
			FE_SUSPECT(MODULO_BY_64(reinterpret_cast<uintptr_t>(l_result)) != 0, "${%s@0}: The allocated heap memory address not aligned by sixty-four. The address value was ${%p@1}", TO_STRING(MEMORY_ERROR_1XX::_ERROR_ILLEGAL_ADDRESS_ALIGNMENT), l_result);

			if (l_result != pointer_p) _LIKELY_
			{
				FE::memcpy<is_address_aligned, is_address_aligned>(l_result, new_count_p * sizeof(value_type), pointer_p, prev_count_p * sizeof(value_type));
				tl_s_memory_pool_allocator.deallocate(pointer_p, prev_count_p);
			}
			return l_result;
		}

		_FORCE_INLINE_ static void deallocate(pointer pointer_p, size_type count_p) noexcept
		{
			FE_SUSPECT(count_p == 0, "${%s@0}: queried allocation size is ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), &count_p);
			FE_SUSPECT(pointer_p == nullptr, "${%s@0}: attempted to delete ${%p@1}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), pointer_p);

			tl_s_memory_pool_allocator.deallocate(pointer_p, count_p);
		}
	};

	template<class StatefulAllocator>
	thread_local tbb::memory_pool<StatefulAllocator> tbb_memory_pool_allocator<StatefulAllocator>::tl_s_memory_pool;

	template<class StatefulAllocator>
	thread_local tbb::memory_pool_allocator<typename StatefulAllocator::value_type> tbb_memory_pool_allocator<StatefulAllocator>::tl_s_memory_pool_allocator{ tl_s_memory_pool };
}




// The template argument Allocator is ignored and replaced with FE::engine_style::tbb_memory_pool_allocator<FE::std_style::scalable_aligned_allocator<T>> if ShouldNodesBePooled is _POSITIVE. Otherwise Allocator is used as is.
template<class T, class Comparison = std::less<T>, class Allocator = FE::scalable_aligned_allocator<T>, internal::heap::SHOUD_NODES_BE_POOLED ShouldNodesBePooled = internal::heap::evaluate_type_size(sizeof(T))>
class heap;


template<class T, class Comparison, class Allocator>
class heap<T, Comparison, Allocator, internal::heap::SHOUD_NODES_BE_POOLED::_POSITIVE>
{
public:
	using key_type = T;
	using value_type = T;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;
	using key_compare = Comparison;
	using allocator_type = internal::heap::tbb_memory_pool_allocator< FE::std_style::scalable_aligned_allocator< internal::heap::node<T> > >;
	using reference = value_type&;
	using const_reference = const reference;
	using pointer = value_type*;
	using const_pointer = const pointer;
	//using iterator = FE::iterator<FE::heap_iterator<>>;
	//using const_iterator = FE::const_iterator<FE::heap_iterator<>>;
	//using reverse_iterator = FE::reverse_iterator<FE::heap_iterator<>>;
	//using const_reverse_iterator = FE::const_reverse_iterator<FE::heap_iterator<>>;

private:
	FE::unique_ptr<internal::heap::node<T>, allocator_type> m_root;
};


END_NAMESPACE
#endif