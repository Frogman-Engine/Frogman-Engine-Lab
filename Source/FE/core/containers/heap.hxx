#ifndef _FE_CORE_HEAP_HXX_
#define _FE_CORE_HEAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <boost/pool/pool_alloc.hpp>



BEGIN_NAMESPACE(FE)


namespace internal::heap
{
	struct node
	{

	};
}


template<class T, class Comparison = std::less<T>, class Allocator = FE::scalable_aligned_allocator<T>>
class heap
{
public:
	using key_type = T;
	using value_type = T;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;
	using key_compare = Comparison;
	using allocator_type = Allocator;
	using reference = value_type&;
	using const_reference = const reference;
	using pointer = value_type*;
	using const_pointer = const pointer;
	//using iterator = FE::iterator<FE::heap_iterator<>>;
	//using const_iterator = FE::const_iterator<FE::heap_iterator<>>;
	//using reverse_iterator = FE::reverse_iterator<FE::heap_iterator<>>;
	//using const_reverse_iterator = FE::const_reverse_iterator<FE::heap_iterator<>>;
};


END_NAMESPACE
#endif