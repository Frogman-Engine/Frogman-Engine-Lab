#ifndef _FE_CORE_HEAP_HXX_
#define _FE_CORE_HEAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/smart_pointers/unique_ptr.hxx>
#include <tbb/memory_pool.h>




BEGIN_NAMESPACE(FE)


enum struct SHOUD_NODES_BE_POOLED
{
	_POSITIVE,
	_NEGATIVE
};


namespace internal::heap
{
	template<class T>
	struct node
	{
		T _data;
		FE::ref<node> _parent;
		FE::ref<node> _left;
		FE::ref<node> _right;
	};

	constexpr SHOUD_NODES_BE_POOLED evaluate_type_size(size_t size_in_bytes_p) noexcept
	{
		if (size_in_bytes_p > 1024)
		{
			return SHOUD_NODES_BE_POOLED::_NEGATIVE;
		}
		
		return SHOUD_NODES_BE_POOLED::_POSITIVE;
	}
}




// The template argument Allocator is ignored and replaced with FE::engine_style::tbb_memory_pool_allocator<FE::std_style::scalable_aligned_allocator<T>> if ShouldNodesBePooled is _POSITIVE. Otherwise Allocator is used as is.
template<class T, class Comparison = std::less<T>, class Allocator = FE::scalable_aligned_allocator<T>, SHOUD_NODES_BE_POOLED ShouldNodesBePooled = internal::heap::evaluate_type_size(sizeof(T))>
class heap;


template<class T, class Comparison, class Allocator>
class heap<T, Comparison, Allocator, SHOUD_NODES_BE_POOLED::_POSITIVE>
{

public:
	using key_type = T;
	using value_type = T;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;
	using key_compare = Comparison;
	using allocator_type = FE::engine_style::tbb_memory_pool_allocator< FE::std_style::scalable_aligned_allocator< internal::heap::node<T> > >;
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