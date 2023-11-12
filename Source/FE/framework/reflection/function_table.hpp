#ifndef _FE_CORE_FUNCTION_TABLE_HXX_
#define _FE_CORE_FUNCTION_TABLE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/function.hxx>
#include <FE/core/fstring.hxx>
#include <FE/core/hash.hpp>
#include <FE/core/pool_allocator.hxx>
#include <unordered_map> 




BEGIN_NAMESPACE(FE::framework)


class application;
using method_signature_t = FE::fstring<64>;


// function_table is currently under development. It might not be suitable to use
class function_table
{
	friend class application;
	
public:		

	constexpr static size_t function_pool_size = 1 MB;
	using function_pool = FE::generic_pool<function_pool_size>;

	template<typename T>
	using smart_function_ptr = FE::generic_pool_ptr<T, function_pool_size>;

	using underlying_container = std::unordered_map<method_signature_t,
													FE::task_base*, 
													FE::hash<method_signature_t>,
													std::equal_to<method_signature_t>,
													FE::std_style::pool_allocator<std::pair<const method_signature_t, FE::task_base*>, FE::capacity<function_pool_size>, FE::align_8bytes>
													>;
	
	/* 
		::std::unordered_map uses node-based linked list for its underlying Container.
		Since the inner bucket data structure has poor cache access patterns, ::std::unordered_map needs to be replaced with a hash map with contiguous buckets
	*/
private:
	thread_local static underlying_container tl_s_task_map;


public:
	function_table() noexcept = delete;
	~function_table() noexcept = delete;
	function_table(const function_table& other_cref_p) noexcept = delete;
	function_table(function_table&& rvalue_p) noexcept = delete;

	template<class TaskType, typename FunctionPtr>
	_FORCE_INLINE_ static void register_method(const underlying_container::key_type& key_p, FunctionPtr function_p) noexcept
	{	
		FE_STATIC_CHECK((std::is_pointer<FunctionPtr>::value == false), "An invalid function type detected.");
		FE_STATIC_CHECK((std::is_base_of<FE::task_base, TaskType>::value == false), "An invalid function type detected.");

		smart_function_ptr<TaskType> l_unique_ptr = function_pool::allocate<TaskType>(1);
		*(l_unique_ptr.get()) = function_p;
		FE::task_base* const l_task_ptr = l_unique_ptr.release();
		tl_s_task_map.emplace(key_p, l_task_ptr);
	}

	_FORCE_INLINE_ static boolean check_presence(const underlying_container::key_type& key_p) noexcept { return tl_s_task_map.find(key_p) != tl_s_task_map.end(); }
	_FORCE_INLINE_ static FE::task_base* retrieve(const underlying_container::key_type& key_p) noexcept { return tl_s_task_map.find(key_p)->second; }

	template<class... Arguments>
	_FORCE_INLINE_ static void invoke(const underlying_container::key_type& key_p) noexcept { tl_s_task_map.find(key_p)->second->operator()(); }
};


END_NAMESPACE
#endif
