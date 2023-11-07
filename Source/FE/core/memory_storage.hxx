#ifndef _FE_CORE_MEMORY_STORAGE_HXX_
#define _FE_CORE_MEMORY_STORAGE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/memory.hxx>
#include <FE/core/smart_pointers/smart_ptrs.h>
#define _MAYBE_ALIGNED_ allocator_type::is_address_aligned, allocator_type::is_address_aligned




BEGIN_NAMESPACE(FE)


template <class T, class Allocator, TYPE_TRIVIALITY IsTrivial>
class container_storage;

template<class T, class Allocator>
class container_storage<T, Allocator, TYPE_TRIVIALITY::_TRIVIAL> final
{
	FE_STATIC_ASSERT((std::is_same<T, Allocator::value_type>::value == false), "Static Assertion Failed: The template argument T and Allocator's value_type have be the same type.");
	FE_STATIC_ASSERT(std::is_class<Allocator>::value == false, "Static Assertion Failed: The template argument Allocator is not a class or a struct type.");

public:
	using smart_ptr = FE::exclusive_ptr<T[], Allocator>;
	using value_type = typename smart_ptr::element_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using allocator_type = typename smart_ptr::allocator_type;
	using size_type = var::size_t;

private:
	smart_ptr m_smart_array;
	size_type m_array_size;
	var::index_t m_last_index;

public:
	_CONSTEXPR20_ container_storage() noexcept : m_smart_array(), m_array_size(), m_last_index() {}
	_CONSTEXPR20_ container_storage(size_type initial_capacity_p) noexcept : m_smart_array(FE::make_exclusive<value_type[], allocator_type>(initial_capacity_p)), m_array_size(), m_last_index() {}
	_CONSTEXPR20_ ~container_storage() noexcept {}

	_CONSTEXPR20_ container_storage(const container_storage& other_p) noexcept : m_smart_array(FE::make_exclusive<value_type[], allocator_type>(other_p.m_smart_array.capacity())), m_array_size(other_p.m_array_size), m_last_index(other_p.m_last_index)
	{
		FE::memcpy<_MAYBE_ALIGNED_>(this->m_smart_array.get(), other_p.m_smart_array.get(), other_p.m_last_index * sizeof(value_type));
	}

	_CONSTEXPR20_ container_storage(container_storage&& rvalue_p) noexcept : m_smart_array(std::move(rvalue_p.m_smart_array)), m_array_size(rvalue_p.m_array_size), m_last_index(rvalue_p.m_last_index) {}


	_CONSTEXPR20_ container_storage& operator=(const container_storage& other_p) noexcept
	{
		if (other_p.m_array_size == 0) { return *this; }

		if (this->m_smart_array.capacity() < other_p.m_smart_array.capacity())
		{
			this->m_smart_array.reset(FE::resize_to{ other_p.m_smart_array.capacity() });
		}

		FE::memcpy<_MAYBE_ALIGNED_>(this->m_smart_array.get(), other_p.m_smart_array.get(), other_p.m_last_index * sizeof(value_type));

		this->m_array_size = other_p.m_array_size;
		this->m_last_index = other_p.m_last_index;

		return *this;
	}

	_FORCE_INLINE_ container_storage& operator=(container_storage&& rvalue_p) noexcept
	{
		this->m_smart_array = std::move(rvalue_p.m_smart_array);

		this->m_array_size = algorithm::utility::exchange(rvalue_p.m_array_size, 0);
		this->m_last_index = algorithm::utility::exchange(rvalue_p.m_last_index, 0);

		return *this;
	}

	_FORCE_INLINE_ reference operator[](size_type index_p) const noexcept
	{
		FE_ASSERT(index_p >= this->m_smart_array.capacity(), "${%s@0}: ${%s@1} exceeds the array capacity. ${%s@1} was ${%lu@2}, and ${%s@3} was ${%lu@4}", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(index_p), &index_p, TO_STRING(this->m_smart_array.capacity()), &FE::buffer<size_type>::set_and_get(this->m_smart_array.capacity()));

		return this->m_smart_array[index_p];
	}


	_FORCE_INLINE_ void reserve(size_type new_capacity_p) noexcept
	{
		if (new_capacity_p > this->m_smart_array.capacity())
		{
			this->m_smart_array.reset(FE::resize_to{ new_capacity_p });
		}
	}

	_FORCE_INLINE_ void extend(size_type extra_capacity_p) noexcept
	{
		this->m_smart_array.reset(FE::resize_to{ this->m_smart_array.capacity() + extra_capacity_p });
	}

	_FORCE_INLINE_ void resize(size_type new_size_p) noexcept
	{
		this->m_smart_array.reset(FE::resize_to{ new_size_p });
	}

	_FORCE_INLINE_ void shrink_to_fit() noexcept
	{
		this->m_smart_array.reset(FE::resize_to{ this->m_last_index });
	}

	_FORCE_INLINE_ void clear() noexcept
	{
		this->m_array_size = 0;
		this->m_last_index = 0;
	}


	void assign_at(size_type index_p, T value_p) noexcept
	{

	}

	void assign_at(size_type from_index_p, std::initializer_list<T>&& values_p) noexcept
	{

	}

	template <class InputIterator>
	void assign_at(size_type from_index_p, InputIterator pointer_p, size_type count_p) noexcept
	{

	}
};

template<class T, class Allocator>
class container_storage<T, Allocator, TYPE_TRIVIALITY::_NOT_TRIVIAL> final
{
	using smart_ptr = FE::exclusive_ptr<T[], Allocator>;
	using value_type = typename smart_ptr::element_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using allocator_type = typename smart_ptr::allocator_type;

	using size_type = var::size_t;
	FE::unique_ptr<OBJECT_STATUS[], FE::scalable_aligned_allocator<OBJECT_STATUS>> m_status_table;
	smart_ptr m_smart_array;
	size_type m_array_size;
	var::index_t m_last_index;

public:
	_FORCE_INLINE_ reference operator[](size_type index_p) const noexcept
	{
		FE_ASSERT(index_p >= this->m_smart_array.capacity(), "${%s@0}: ${%s@1} exceeds the array capacity. ${%s@1} was ${%lu@2}, and ${%s@3} was ${%lu@4}", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(index_p), &index_p, TO_STRING(this->m_smart_array.capacity()), &FE::buffer<size_type>::set_and_get(this->m_smart_array.capacity()));
		FE_ASSERT(this->m_status_table[index_p] == OBJECT_STATUS::_DESTRUCTED, "${%s@0}: FE::array[${%s@1}] was not initialized.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION), TO_STRING(index_p));

		return this->m_smart_array[index_p];
	}


	void reserve(size_type new_capacity_p) noexcept
	{

	}

	void extend(size_type extra_capacity_p) noexcept
	{

	}

	void resize(size_type new_size_p) noexcept
	{

	}

	void shrink_to_fit() noexcept
	{

	}


	_FORCE_INLINE_ void construct_at(size_type index_p) noexcept
	{
		FE_ASSERT(index_p >= this->m_smart_array.capacity(), "${%s@0}: ${%s@1} exceeds the array capacity. ${%s@1} was ${%lu@2}, and ${%s@3} was ${%lu@4}", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(index_p), &index_p, TO_STRING(this->m_smart_array.capacity()), &FE::buffer<size_type>::set_and_get(this->m_smart_array.capacity()));

		this->m_array_size++;
	}

	void destruct_at(size_type index_p) noexcept
	{

	}

	void construct_range_if_possible(size_type begin_index_p, size_type end_index_p) noexcept
	{

	}

	void destruct_range_if_possible(size_type begin_index_p, size_type end_index_p) noexcept
	{

	}


	_FORCE_INLINE_ void clear() noexcept
	{

	}


	void assign_at(size_type index_p, T value_p) noexcept
	{

	}

	void assign_at(size_type from_index_p, std::initializer_list<T>&& values_p) noexcept
	{

	}

	template <class InputIterator>
	void assign_at(size_type from_index_p, InputIterator pointer_p, size_type count_p) noexcept
	{

	}
};


END_NAMESPACE
#endif