#ifndef _FE_CORE_REF_TABLE_HXX_
#define _FE_CORE_REF_TABLE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/do_once.hxx>
#include <boost/stacktrace.hpp>
#include <vector>




namespace FE
{
	template<typename T>
	class proxy_ptr;

	template<typename T, class Allocator>
	class exclusive_ptr;

	template<typename T>
	class exclusive_ptr_base;
}


BEGIN_NAMESPACE(FE::internal)


template<typename T>
class ref_table_base 
{
	friend class exclusive_ptr_base<T>;

public:
	using const_pointer = typename const exclusive_ptr_base<T>*;
	using pointer = typename exclusive_ptr_base<T>*;
	using element_type = typename std::remove_extent<T>::type;

	using ref_table_type = std::vector<const_pointer, FE::scalable_aligned_allocator<const_pointer>>;

	using ref_table_value_type = typename ref_table_type::value_type;
	static_assert(std::is_same<ref_table_value_type, const exclusive_ptr_base<T>*>::value == true, "Static Assertion Failure: ref_table_value_type must be the same as exclusive_ptr_base<T>*");
	
	using ref_table_key_type = typename ref_table_type::size_type;
	using ref_table_recycler_type = std::vector<ref_table_key_type, FE::scalable_aligned_allocator<ref_table_key_type>>;

	static constexpr ref_table_key_type invalid_key_value = FE::max_value<ref_table_key_type>;
	static constexpr auto ref_table_initial_size = 1024;
	static constexpr auto ref_table_recycler_initial_size = ref_table_initial_size >> 1;
	
protected:
	thread_local static ref_table_type tl_s_ref_table;
	thread_local static ref_table_recycler_type tl_s_ref_table_recycler;
};

template<typename T>
thread_local typename ref_table_base<T>::ref_table_type ref_table_base<T>::tl_s_ref_table;

template<typename T>
thread_local typename ref_table_base<T>::ref_table_recycler_type ref_table_base<T>::tl_s_ref_table_recycler;




template<typename T, class Allocator>
class ref_table_for_exclusive_ptr : public ref_table_base<T>
{
	friend class exclusive_ptr<T, Allocator>;

public:
	using base_type = ref_table_base<T>;
	using const_pointer = typename base_type::const_pointer;
	using pointer = typename base_type::pointer;
	using element_type = typename base_type::element_type;

	using ref_table_type = typename base_type::ref_table_type;
	using ref_table_recycler_type = typename base_type::ref_table_recycler_type;

	using ref_table_key_type = typename base_type::ref_table_key_type;
	using ref_table_value_type = typename base_type::ref_table_value_type;
	static_assert(std::is_same<ref_table_value_type, const exclusive_ptr_base<T>*>::value == true, "Static Assertion Failure: ref_table_value_type must be the same as exclusive_ptr_base<T>*");

private:
	_FORCE_INLINE_ static size_t __register_ref(ref_table_value_type pointer_p) noexcept
	{
		ref_table_key_type l_index_key;
		
		if (base_type::tl_s_ref_table_recycler.empty() != true)
		{
			l_index_key = base_type::tl_s_ref_table_recycler.back();
			base_type::tl_s_ref_table_recycler.pop_back();
			base_type::tl_s_ref_table[l_index_key] = pointer_p;

			FE_SUSPECT(base_type::tl_s_ref_table[l_index_key] != pointer_p, "Assertion Failed: failed to register a reference to the table.\n${%s@0} must be equal to ${%p@1}.", TO_STRING(base_type::tl_s_ref_table[l_index_key]), pointer_p);
			//FE_LOG("Registered a key value: ${%lu@0}.\n\n\n", &l_index_key);
			return l_index_key;
		}

		l_index_key = base_type::tl_s_ref_table.size();
		base_type::tl_s_ref_table.emplace_back(pointer_p);

		FE_SUSPECT(base_type::tl_s_ref_table[l_index_key] != pointer_p, "Assertion Failed: failed to register a reference to the table.\n${%s@0} must be equal to ${%p@1}.", TO_STRING(base_type::tl_s_ref_table[l_index_key]), pointer_p);
		//FE_LOG("Registered a key value: ${%lu@0}.\n\n\n", &l_index_key);
		return l_index_key;
	}

	_FORCE_INLINE_ static void __unregister_ref(ref_table_key_type index_key_p, _MAYBE_UNUSED_ ref_table_value_type auth_p) noexcept
	{
		FE_SUSPECT(index_key_p >= base_type::tl_s_ref_table.size(), "${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(index_key_p));
		base_type::tl_s_ref_table[index_key_p] = nullptr;
		
		//FE_LOG("Unregistered a key value: ${%lu@0}.\n\n\n", &index_key_p);

		base_type::tl_s_ref_table_recycler.emplace_back(index_key_p);

#ifdef _DEBUG_

		auto l_begin = base_type::tl_s_ref_table_recycler.begin();
		auto l_end = base_type::tl_s_ref_table_recycler.end();

		while(l_begin != l_end)
		{
			FE_SUSPECT(base_type::tl_s_ref_table[*l_begin] != nullptr, "Assertion Failed: unregistered keys' value must be nullptr.");
			
			for (auto it = l_begin + 1; it != l_end; ++it)
			{
				FE_SUSPECT(*l_begin == *it, "Assertion Failed: Keys are always unique.");
			}
			++l_begin;
		}
#endif
	}

	_FORCE_INLINE_ static void __update_ref(ref_table_key_type index_key_p, ref_table_value_type pointer_p) noexcept
	{
		FE_SUSPECT(pointer_p == nullptr, "${%s@0}: ${%s@1} is nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(pointer_p));
		FE_SUSPECT(index_key_p >= base_type::tl_s_ref_table.size(), "${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(index_key_p));
		base_type::tl_s_ref_table[index_key_p] = pointer_p;
	}
};




template<typename T>
class ref_table_for_proxy_ptr : public ref_table_base<T>
{
	friend class proxy_ptr<T>;

public:
	using base_type = ref_table_base<T>;
	using const_pointer = typename base_type::const_pointer;
	using pointer = typename base_type::pointer;
	using element_type = typename base_type::element_type;

	using ref_table_type = typename base_type::ref_table_type;
	using ref_table_key_type = typename base_type::ref_table_key_type;

	using ref_table_value_type = typename base_type::ref_table_value_type;
	static_assert(std::is_same<ref_table_value_type, const exclusive_ptr_base<T>*>::value == true, "Static Assertion Failure: ref_table_value_type must be the same as exclusive_ptr_base<T>*");

private:
	_FORCE_INLINE_ static pointer __get_ref(typename ref_table_type::size_type index_p) noexcept
	{
		if (index_p == base_type::invalid_key_value)
		{
			return nullptr;
		}

		return const_cast<pointer>(base_type::tl_s_ref_table[index_p]);
	}
};


END_NAMESPACE
#endif