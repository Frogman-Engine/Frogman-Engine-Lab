#ifndef _FE_CORE_HEAP_MEMORY_TRACKER_HPP_
#define _FE_CORE_HEAP_MEMORY_TRACKER_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/miscellaneous/misc.h>
#include <FE/core/memory_metrics.h>
#include <tbb/scalable_allocator.h>
#include <FE/core/memory.hxx>
#include <FE/core/thread.hpp>




BEGIN_NAMESPACE(FE)


template<typename T, class SourceAddressAlignment>
class heap_memory_tracker;


template <typename T, class Alignment>
class scalable_aligned_allocator;

template <typename T>
class cache_aligned_allocator;


class heap_memory_tracker_base
{
protected:
	static ::std::atomic_int64_t s_global_total_bytes;
	thread_local static var::int64 tl_s_thread_local_total_bytes;
};


template<typename U, class SourceAddressAlignment = typename FE::SIMD_auto_alignment::alignment_type>
class heap_memory_tracker final : public heap_memory_tracker_base
{
	friend class scalable_aligned_allocator<U, SourceAddressAlignment>;
	friend class cache_aligned_allocator<U>;


	static ::std::atomic_int64_t s_global_total_bytes_by_type;
	thread_local static var::int64 tl_s_thread_local_total_bytes_by_type;

	_FORCE_INLINE_ static void add(int64 size_bytes_p) noexcept
	{
		heap_memory_tracker_base::s_global_total_bytes.fetch_add(size_bytes_p, ::std::memory_order_relaxed);
		heap_memory_tracker_base::tl_s_thread_local_total_bytes += size_bytes_p;

		heap_memory_tracker::s_global_total_bytes_by_type.fetch_add(size_bytes_p, ::std::memory_order_relaxed);
		heap_memory_tracker::tl_s_thread_local_total_bytes_by_type += size_bytes_p;
	}

	_FORCE_INLINE_ static void sub(int64 size_bytes_p) noexcept
	{
		heap_memory_tracker_base::s_global_total_bytes.fetch_sub(size_bytes_p, ::std::memory_order_relaxed);
		heap_memory_tracker_base::tl_s_thread_local_total_bytes -= size_bytes_p;

		heap_memory_tracker::s_global_total_bytes_by_type.fetch_sub(size_bytes_p, ::std::memory_order_relaxed);
		heap_memory_tracker::tl_s_thread_local_total_bytes_by_type -= size_bytes_p;
	}

public:
	typedef U memory_value_type;

	_FORCE_INLINE_ static total_memory_utilization_data query_all_data() noexcept
	{
#if defined(_ENABLE_MEMORY_TRACKER_)
		total_memory_utilization_data l_data
		{
			heap_memory_tracker_base::s_global_total_bytes.load(std::memory_order_relaxed), heap_memory_tracker_base::tl_s_thread_local_total_bytes,
			heap_memory_tracker::s_global_total_bytes_by_type.load(std::memory_order_relaxed), heap_memory_tracker::tl_s_thread_local_total_bytes_by_type
		};
		return l_data;
#else
		total_memory_utilization_data l_null{_NULL_};
		return l_null;
#endif
	}
	
	_FORCE_INLINE_ static global_memory_utilization query_global_data() noexcept
	{
#if defined(_ENABLE_MEMORY_TRACKER_)
		global_memory_utilization l_data
		{
			heap_memory_tracker_base::s_global_total_bytes.load(std::memory_order_relaxed), heap_memory_tracker_base::tl_s_thread_local_total_bytes
		};
		return l_data;
#else
		global_memory_utilization l_null{ _NULL_ };
		return l_null;
#endif
	}

	_FORCE_INLINE_ static type_memory_utilization query_type_data() noexcept
	{
#if defined(_ENABLE_MEMORY_TRACKER_)
		type_memory_utilization l_data
		{
			heap_memory_tracker::s_global_total_bytes_by_type.load(std::memory_order_relaxed), heap_memory_tracker::tl_s_thread_local_total_bytes_by_type
		};
		return l_data;
#else
		type_memory_utilization l_null{ _NULL_ };
		return l_null;
#endif
	}


#if defined(_ENABLE_MEMORY_TRACKER_)
	_FORCE_INLINE_ static void __log_heap_memory_allocation(uint64 size_in_bytes_to_allocate_p, const char* const allocator_name_p, U* const allocated_address_p, const char* const allocated_type_name_p) noexcept
	{
		uint64 l_this_thread_id = FE::thread::this_thread_id();

		global_memory_utilization l_prev_heap_mem_usage_in_bytes = heap_memory_tracker::query_global_data();
		heap_memory_tracker::add(size_in_bytes_to_allocate_p);
		FE_LOG("\n\n The previous total heap memory used by this application was ${%lu@0} bytes. \n The previous total heap memory used by this thread[id:${%lu@1}] was ${%lu@2} bytes.\n Allocated ${%lu@3} bytes of heap memory from ${%s@4}. \n The memory address is aligned by ${%d@5} bytes. \n The allocated memory data type is ${%s@6}.\n The allocated memory address is 0x${%p@7} \n", &(l_prev_heap_mem_usage_in_bytes._global_total_bytes), &l_this_thread_id, &(l_prev_heap_mem_usage_in_bytes._thread_local_total_bytes), &size_in_bytes_to_allocate_p, allocator_name_p, &SourceAddressAlignment::size, allocated_type_name_p, allocated_address_p);

		global_memory_utilization l_new_heap_mem_usage_in_bytes = heap_memory_tracker::query_global_data();
		FE_LOG("\n\n New total heap memory used by this application is now ${%lu@0} bytes. \n New total heap memory used by this thread[id:${%lu@1}] is now ${%lu@2} bytes.\n", &(l_new_heap_mem_usage_in_bytes._global_total_bytes), &l_this_thread_id, &(l_new_heap_mem_usage_in_bytes._thread_local_total_bytes));
	}

	_FORCE_INLINE_ static void __log_heap_memory_reallocation(uint64 prev_size_in_bytes_p, uint64 new_size_in_bytes_to_allocate_p, const char* const allocator_name_p, U* const allocated_address_p, const char* const allocated_type_name_p) noexcept
	{
		uint64 l_this_thread_id = FE::thread::this_thread_id();

		global_memory_utilization l_prev_heap_mem_usage_in_bytes = heap_memory_tracker::query_global_data();
		heap_memory_tracker::sub(prev_size_in_bytes_p);
		heap_memory_tracker::add(new_size_in_bytes_to_allocate_p);
		FE_LOG("\n\n The previous total heap memory used by this application was ${%lu@0} bytes. \n The previous total heap memory used by this thread[id:${%lu@1}] was ${%lu@2} bytes.\n Re-allocated ${%lu@3} bytes of heap memory from ${%s@4}. \n The memory address is aligned by ${%d@5} bytes. \n The allocated memory data type is ${%s@6}.\n The allocated memory address is 0x${%p@7} \n", &(l_prev_heap_mem_usage_in_bytes._global_total_bytes), &l_this_thread_id, &(l_prev_heap_mem_usage_in_bytes._thread_local_total_bytes), &new_size_in_bytes_to_allocate_p, allocator_name_p, &SourceAddressAlignment::size, allocated_type_name_p, allocated_address_p);

		global_memory_utilization l_new_heap_mem_usage_in_bytes = heap_memory_tracker::query_global_data();
		FE_LOG("\n\n New total heap memory used by this application is now ${%lu@0} bytes. \n New total heap memory used by this thread[id:${%lu@1}] is now ${%lu@2} bytes.\n", &(l_new_heap_mem_usage_in_bytes._global_total_bytes), &l_this_thread_id, &(l_new_heap_mem_usage_in_bytes._thread_local_total_bytes));
	}

	_FORCE_INLINE_ static void __log_heap_memory_deallocation(uint64 size_in_bytes_to_deallocate_p, const char* const allocator_name_p, U* const allocated_address_p, const char* const allocated_type_name_p) noexcept
	{
		uint64 l_this_thread_id = FE::thread::this_thread_id();

		global_memory_utilization l_prev_heap_mem_usage_in_bytes = heap_memory_tracker::query_global_data();
		heap_memory_tracker::sub(size_in_bytes_to_deallocate_p);
		FE_LOG("\n\n The previous total heap memory used by this application was ${%lu@0} bytes. \n The previous total heap memory used by this thread[id:${%lu@1}] was ${%lu@2} bytes.\n De-allocated ${%lu@3} bytes of heap memory via ${%s@4}. \n The memory address is aligned by ${%d@5} bytes. \n The de-allocated memory data type is ${%s@6}.\n The de-allocated memory address is 0x${%p@7} \n", &(l_prev_heap_mem_usage_in_bytes._global_total_bytes), &l_this_thread_id, &(l_prev_heap_mem_usage_in_bytes._thread_local_total_bytes), &size_in_bytes_to_deallocate_p, allocator_name_p, &SourceAddressAlignment::size, allocated_type_name_p, allocated_address_p);

		global_memory_utilization l_new_heap_mem_usage_in_bytes = heap_memory_tracker::query_global_data();
		FE_LOG("\n\n New total heap memory used by this application is now ${%lu@0} bytes. \n New total heap memory used by this thread[id:${%lu@1}] is now ${%lu@2} bytes.\n", &(l_new_heap_mem_usage_in_bytes._global_total_bytes), &l_this_thread_id, &(l_new_heap_mem_usage_in_bytes._thread_local_total_bytes));
	}
#endif


	template<typename T, class Alignment>
	friend _FORCE_INLINE_ T* trackable_calloc(length_t count_p, size_t bytes_p) noexcept;

	template<typename T, class Alignment>
	friend _FORCE_INLINE_ void trackable_free(T* const memblock_pointer_p, length_t count_p, size_t bytes_p) noexcept;

	template<typename T, class Alignment>
	friend _FORCE_INLINE_ T* trackable_realloc(T* const memblock_pointer_p, size_t prev_bytes_p, size_t new_bytes_p) noexcept;
};


template<typename T, class SourceAddressAlignment>
::std::atomic_int64_t FE::heap_memory_tracker<T, SourceAddressAlignment>::s_global_total_bytes_by_type = 0;

template<typename T, class SourceAddressAlignment>
thread_local var::int64 FE::heap_memory_tracker<T, SourceAddressAlignment>::tl_s_thread_local_total_bytes_by_type = 0;


template<typename T, class Alignment = typename FE::SIMD_auto_alignment::alignment_type>
_NODISCARD_ _FORCE_INLINE_ T* trackable_calloc(length_t count_p, size_t bytes_p) noexcept
{
	T* const l_result = (T*)::scalable_aligned_malloc(count_p * bytes_p, Alignment::size);
	ALIGNED_MEMSET(l_result, _NULL_, count_p * bytes_p);

	FE_SUSPECT(l_result == nullptr, "${%s@0}: Failed to allocate memory from scalable_aligned_malloc()", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
	FE_SUSPECT((reinterpret_cast<uintptr_t>(l_result) % Alignment::size) != 0, "${%s@0}: The allocated heap memory address not aligned by ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_ERROR_ILLEGAL_ADDRESS_ALIGNMENT), &Alignment::size);
	
#if defined(_ENABLE_MEMORY_TRACKER_)
	::FE::heap_memory_tracker<T, Alignment>::__log_heap_memory_allocation(count_p * bytes_p, TO_STRING(trackable_calloc), l_result, typeid(T).name());
#endif

	return l_result;
}


template<typename T, class Alignment = typename FE::SIMD_auto_alignment::alignment_type>
_FORCE_INLINE_ void trackable_free(T* const memblock_pointer_p, _MAYBE_UNUSED_ length_t count_p, _MAYBE_UNUSED_ size_t bytes_p) noexcept
{
	::scalable_aligned_free(memblock_pointer_p);

#if defined(_ENABLE_MEMORY_TRACKER_)
	::FE::heap_memory_tracker<T, Alignment>::__log_heap_memory_deallocation(count_p * bytes_p, TO_STRING(trackable_free), memblock_pointer_p, typeid(T).name());
#endif 
}


template<typename T, class Alignment = typename FE::SIMD_auto_alignment::alignment_type>
_NODISCARD_ _FORCE_INLINE_ T* trackable_realloc(T* const memblock_pointer_p, length_t prev_bytes_p, length_t new_bytes_p) noexcept
{
	T* l_realloc_result = (T*)::scalable_aligned_realloc(memblock_pointer_p, sizeof(T) * new_bytes_p, Alignment::size);

	if (l_realloc_result == nullptr) _UNLIKELY_
	{
		l_realloc_result = (T*)::scalable_aligned_malloc(new_bytes_p * new_bytes_p, Alignment::size);
		ALIGNED_MEMSET(l_realloc_result, _NULL_, new_bytes_p * new_bytes_p);

		FE_SUSPECT(l_realloc_result == nullptr, "${%s@0}: Failed to re-allocate memory from scalable_aligned_malloc()", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		FE::memcpy<ADDRESS::_ALIGNED, ADDRESS::_ALIGNED>(l_realloc_result, sizeof(T) * new_bytes_p, memblock_pointer_p, sizeof(T) * prev_bytes_p);
		
		if (l_realloc_result != memblock_pointer_p) _LIKELY_
		{
			::scalable_aligned_free(memblock_pointer_p);
		}
	}

	FE_SUSPECT((reinterpret_cast<uintptr_t>(l_realloc_result) % Alignment::size) != 0, "${%s@0}: The allocated heap memory address not aligned by ${%lu@1}.", TO_STRING(MEMORY_ERROR_1XX::_ERROR_ILLEGAL_ADDRESS_ALIGNMENT), &Alignment::size);
	
#if defined(_ENABLE_MEMORY_TRACKER_)
	::FE::heap_memory_tracker<T, Alignment>::__log_heap_memory_reallocation(prev_bytes_p * prev_bytes_p, new_bytes_p * new_bytes_p, TO_STRING(trackable_realloc), l_realloc_result, typeid(T).name());
#endif
	
	return l_realloc_result;
}


END_NAMESPACE
#endif 
