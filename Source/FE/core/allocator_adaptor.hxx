#ifndef _FE_CORE_ALLOCATOR_ADAPTORS_HXX_
#define _FE_CORE_ALLOCATOR_ADAPTORS_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/allocator.hxx>




BEGIN_NAMESPACE(FE)


struct boost_pool_tbb_scalable_align_allocator final
{
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;

	static char* malloc(const size_type bytes_p) noexcept
	{
		return (char*)scalable_aligned_malloc(bytes_p, FE::SIMD_auto_alignment::alignment_type::size);
	}

	static void free(void* const block_p) noexcept
	{
		scalable_aligned_free(block_p);
	}
};


END_NAMESPACE
#endif