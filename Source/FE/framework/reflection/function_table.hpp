#ifndef _FE_CORE_FUNCTION_TABLE_HXX_
#define _FE_CORE_FUNCTION_TABLE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/algorithm/string.hxx>
#include <FE/core/function.hxx>
#include <FE/core/hash.hpp>
#include <string_view>
#include <unordered_map> 




BEGIN_NAMESPACE(FE::framework)


class application;


// function_table is currently under development. It might not be suitable to use
class function_table
{
	friend class application;
	
public:												
	using underlying_container = std::unordered_map<const char*, 
													FE::task_base*, 
													FE::hash<const char*>,
													algorithm::string::equal_to<char>,
													FE::std_style::scalable_aligned_allocator< std::pair<const char* const, FE::task_base*> >
													>;
	
	/* 
		::std::unordered_map uses node-based linked list for its underlying Container.
		Since the inner bucket data structure has poor cache access patterns, ::std::unordered_map needs to be replaced with a hash map with contiguous buckets
	*/

private:
	thread_local static underlying_container tl_s_hash_map;
	// + function object pool member will be added

public:
	function_table() noexcept = delete;
	~function_table() noexcept = delete;
	function_table(const function_table& other_cref_p) noexcept = delete;
	function_table(function_table&& rvalue_p) noexcept = delete;

	_FORCE_INLINE_ static void register_method(underlying_container::value_type value_p) noexcept { tl_s_hash_map.insert(value_p); }
	_FORCE_INLINE_ static boolean check_presence(underlying_container::key_type key_p) noexcept { return tl_s_hash_map.find(key_p) != tl_s_hash_map.end(); }
	_FORCE_INLINE_ static FE::task_base* retrieve(underlying_container::key_type key_p) noexcept { return tl_s_hash_map.find(key_p)->second; }
	_FORCE_INLINE_ static void invoke(underlying_container::key_type key_p) noexcept { tl_s_hash_map.find(key_p)->second->operator()(); }
};


END_NAMESPACE
#endif
