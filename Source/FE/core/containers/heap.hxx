#ifndef _FE_CORE_HEAP_HXX_
#define _FE_CORE_HEAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <boost/pool/pool_alloc.hpp>



BEGIN_NAMESPACE(FE)


template<class T, class Comparison = std::less<T>, class Allocator = FE::scalable_aligned_allocator<T>>
class heap
{
	// heap
};


END_NAMESPACE
#endif