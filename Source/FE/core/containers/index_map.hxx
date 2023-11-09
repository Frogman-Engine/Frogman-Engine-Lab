#ifndef _FE_CORE_INDEX_MAP_HXX_
#define _FE_CORE_INDEX_MAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator.hxx>
#include <FE/core/allocator_adaptor.hxx>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>
#include <optional>
#include <vector>




BEGIN_NAMESPACE(FE)


template<typename T, class StatefulAllocator = FE::std_style::scalable_aligned_allocator<T>>
class index_map final
{
	FE_STATIC_ASSERT((std::is_same<T, StatefulAllocator::value_type>::value == false), "Static Assertion Failed: The template argument T and StatefulAllocator's value_type have be the same type.");
	FE_STATIC_ASSERT(std::is_class<StatefulAllocator>::value == false, "Static Assertion Failed: The template argument StatefulAllocator is not a class or a struct type.");

public:
	using key_type = typename std::vector<T, StatefulAllocator>::size_type;
	using mapped_type = T;
	using size_type = key_type;
	using difference_type = var::ptrdiff_t;
	using allocator_type = StatefulAllocator;
	using reference = mapped_type&;
	using const_reference = const mapped_type&;
	using pointer = mapped_type*;
	using const_pointer	= const mapped_type*;
	static constexpr key_type invalid_key_value = FE::max_value<key_type>;

private:
	std::vector<mapped_type, allocator_type> m_map;
	std::vector<key_type, FE::std_style::scalable_aligned_allocator<key_type>> m_fragmented_index_stack;

public:
	index_map() noexcept : m_map(), m_fragmented_index_stack() {}
	index_map(FE::reserve initial_capacity_p) noexcept : m_map(), m_fragmented_index_stack()
	{
		this->m_map.reserve(initial_capacity_p._value);
		this->m_fragmented_index_stack.reserve(initial_capacity_p._value);
	}
	index_map(const index_map& other_p) noexcept : m_map(other_p.m_map), m_fragmented_index_stack(other_p.m_fragmented_index_stack) {}
	index_map(index_map&& rvalue_p) noexcept : m_map(std::move(rvalue_p.m_map)), m_fragmented_index_stack(std::move(rvalue_p.m_fragmented_index_stack)) {}
	~index_map() noexcept {}

	_FORCE_INLINE_ index_map& operator=(const index_map& other_p) noexcept
	{
		this->m_map = other_p.m_map;
		this->m_fragmented_index_stack = other_p.m_fragmented_index_stack;
		return *this;
	}
	_FORCE_INLINE_ index_map& operator=(index_map&& rvalue_p) noexcept
	{
		this->m_map = std::move(rvalue_p.m_map);
		this->m_fragmented_index_stack = std::move(rvalue_p.m_fragmented_index_stack);
		return *this;
	}

	_FORCE_INLINE_ boolean is_empty() noexcept { return (this->m_map.size() - this->m_fragmented_index_stack.size()) == 0; }
	_FORCE_INLINE_ size_type size() noexcept { return this->m_map.size() - this->m_fragmented_index_stack.size(); }
	_FORCE_INLINE_ size_type capacity() noexcept { return this->m_map.capacity(); }
	_FORCE_INLINE_ size_type max_size() noexcept { return FE::max_value<size_type>; }

	_FORCE_INLINE_ void clear() noexcept 
	{ 
		this->m_map.clear();
		this->m_fragmented_index_stack.clear();
	}

	template<typename... Arguments>
	key_type emplace(Arguments&&... arguments_p) noexcept
	{
		key_type l_index_key_value;
		if (this->m_fragmented_index_stack.empty() == false)
		{
			l_index_key_value = this->m_fragmented_index_stack.back();
			this->m_fragmented_index_stack.pop_back();
			this->m_map[l_index_key_value] = mapped_type(arguments_p...);
			return l_index_key_value;
		}

		l_index_key_value = this->m_map.size();
		this->m_map.emplace_back(arguments_p...);
		return l_index_key_value;
	}

	_FORCE_INLINE_ void erase(const key_type key_p) noexcept
	{
		this->m_map[key_p] = T();
		this->m_fragmented_index_stack.emplace_back(key_p);
	}

	_FORCE_INLINE_ void swap(index_map& in_out_other_p) noexcept
	{
		index_map l_tmp = std::move(*this);
		*this = std::move(in_out_other_p);
		in_out_other_p = std::move(l_tmp);
	}

	_FORCE_INLINE_ mapped_type& find(const key_type key_p) noexcept
	{
		FE_ASSERT(key_p >= this->m_map.size(), "Assertion Failed. ${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(key_p));
#ifdef _DEBUG_
		for (auto invalid_key : this->m_fragmented_index_stack)
		{
			FE_ASSERT(key_p == invalid_key, "Assertion Failed. ${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(key_p));
		}
#endif

		return this->m_map[key_p];
	}

	_FORCE_INLINE_ boolean contains(const key_type key_p, const mapped_type& value_p) const noexcept
	{
		FE_ASSERT(key_p >= this->m_map.size(), "Assertion Failed. ${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(key_p));
#ifdef _DEBUG_
		for (auto invalid_key : this->m_fragmented_index_stack)
		{
			FE_ASSERT(key_p == invalid_key, "Assertion Failed. ${%s@0}: ${%s@1} is an invalid key.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(key_p));
		}
#endif

		return this->m_map[key_p] == value_p;
	}

	_FORCE_INLINE_ void reserve(const size_type new_capacity_p) noexcept
	{
		this->m_map.reserve(new_capacity_p);
		this->m_fragmented_index_stack.reserve(new_capacity_p);
	}

	_FORCE_INLINE_ void extend(const size_type extra_capacity_p) noexcept
	{
		this->m_map.reserve(this->m_map.capacity() + extra_capacity_p);
		this->m_fragmented_index_stack.reserve(this->m_fragmented_index_stack.capacity() + extra_capacity_p);
	}

	_FORCE_INLINE_ void shrink_to_fit() noexcept
	{
		this->m_map.shrink_to_fit();
		this->m_fragmented_index_stack.shrink_to_fit();
	}
};


END_NAMESPACE
#endif