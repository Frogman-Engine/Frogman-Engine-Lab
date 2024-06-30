#ifndef _FE_FRAMEWORK_REFLECTION_VARIABLE_MAP_HPP_
#define _FE_FRAMEWORK_REFLECTION_VARIABLE_MAP_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/private/allocator_base.hpp>
#include <FE/core/hash.hpp>
#include <FE/core/string.hxx>

// std
#include <mutex>

// ronbin hood hash
#include <robin_hood.h>

// boost
#include <boost/core/demangle.hpp>
// https://github.com/getnamo/7zip-cpp?tab=readme-ov-file
/* serialized object data format
class_name="object",variable_name="_data",variable_offswet=8;
compress with 7z algorithm and change the file extension into .data
3D models and assets -> 7z algorithm -> .froggy
*/
BEGIN_NAMESPACE(FE::framework::reflection)


class variable_map
{
public:
    using underlying_container = robin_hood::unordered_map< FE::string, 
                                                            robin_hood::unordered_map<FE::string, var::ptrdiff, FE::hash<FE::string>>,
                                                            FE::hash<FE::string>
                                                            >;
	using lock_type = std::mutex;

    static constexpr count_t initial_capacity = 1000;

private:
	static underlying_container* s_variable_map;
	static lock_type s_lock;

public:
	variable_map() noexcept = default;
	~variable_map() noexcept = default;

	static boolean initialize() noexcept
	{
		FE_ASSERT(s_variable_map != nullptr, "Assertion failure: cannot initialize FE::framework::reflection::variable_map more than once.");
		if(s_variable_map == nullptr)
		{
			s_variable_map = (underlying_container*)FE_ALIGNED_ALLOC(sizeof(underlying_container), FE::align_CPU_L1_cache_line::size);
			new(s_variable_map) underlying_container(variable_map::initial_capacity);
			return true;
		}

		return false;
	}
	
	static boolean clean_up() noexcept
	{
		FE_ASSERT(s_variable_map == nullptr, "Assertion failure: unable to clean_up() FE::framework::reflection::variable_map. It is null.");

		if(s_variable_map != nullptr)
		{
			s_variable_map->~underlying_container();
			FE_ALIGNED_FREE(s_variable_map);
			s_variable_map = nullptr;
			return true;
		}

		return false;
	}

    _FORCE_INLINE_ static void register_variable(const typename underlying_container::key_type& class_name_p, const typename underlying_container::key_type& variable_name_p, FE::ptrdiff offset_p) noexcept
    {
		{
			std::lock_guard<std::mutex> l_lock(s_lock);

        	auto l_class_search_result = s_variable_map->find(class_name_p);
			if(l_class_search_result == s_variable_map->end())
			{
				auto l_result = s_variable_map->emplace(class_name_p, typename underlying_container::mapped_type{});
				l_result.first->second.emplace(variable_name_p, offset_p);
				return;
			}
			else
			{
        		l_class_search_result->second.emplace(variable_name_p, offset_p);
				return;
			}
		}
    }

	_FORCE_INLINE_ static FE::ptrdiff retrieve(const typename underlying_container::key_type& class_name_p, const typename underlying_container::key_type& variable_name_p) noexcept
	{
		{
			std::lock_guard<std::mutex> l_lock(s_lock);
			auto l_class_search_result = s_variable_map->find(class_name_p);
			FE_ASSERT(l_class_search_result == s_variable_map->end(), "Assertion failure: unable to retrieve an unregistered class/struct meta data.");
			
			auto l_variable_search_result = l_class_search_result->second.find(variable_name_p);
			FE_ASSERT(l_variable_search_result == l_class_search_result->second.end(), "Assertion failure: unable to retrieve an unregistered class/struct member variable offset.");
			return l_variable_search_result->second; 
		}
	}
};

END_NAMESPACE
#endif