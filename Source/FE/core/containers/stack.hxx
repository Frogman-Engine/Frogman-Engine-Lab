﻿#ifndef _FE_CORE_STACK_HXX_
#define _FE_CORE_STACK_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisite_symbols.h>
#include <FE/core/iterator.hxx>
#include <FE/core/memory.hxx>
#include <initializer_list>


BEGIN_NAMESPACE(FE)


template<class T, size_t max_element_count = 0, FE::OBJECT_TRIVIALITY is_trivial = FE::is_trivially_constructible_and_destructible<T>::_VALUE_>
class stack final
{
public:
	stack() noexcept = delete;
	~stack() noexcept = delete;
};




template<class T>
class dynamic_stack_base
{
public:
	using container_type = T;
	using pointer = typename T::pointer;
	using difference_type = typename T::difference_type;
	using const_pointer = typename T::const_pointer;
	using const_iterator = typename T::const_iterator;
	using const_reverse_iterator = typename T::const_reverse_iterator;
	using reference = typename T::reference;
	using const_reference = typename T::const_reference;
	using length_type = typename T::length_type;
	using size_type = typename T::size_type;
	using value_type = typename T::value_type;
	using alignment = typename T::alignment;
	using allocator_type = typename T::allocator;

protected:
	_FORCE_INLINE_ dynamic_stack_base() noexcept {}
	_FORCE_INLINE_ _CONSTEXPR20_ ~dynamic_stack_base() noexcept {}
};


template<class T>
class stack<T, 0, FE::OBJECT_TRIVIALITY::_TRIVIAL> final : dynamic_stack_base<T>
{
public:
	using base_type = dynamic_stack_base<T>;
	using container_type = typename base_type::container_type;
	using pointer = typename base_type::pointer;
	using difference_type = typename base_type::difference_type;
	using const_pointer = typename base_type::const_pointer;
	using const_iterator = typename base_type::const_iterator;
	using const_reverse_iterator = typename base_type::const_reverse_iterator;
	using reference = typename base_type::reference;
	using const_reference = typename base_type::const_reference;
	using length_type = typename base_type::length_type;
	using size_type = typename base_type::size_type;
	using value_type = typename base_type::value_type;
	using alignment = typename base_type::alignment;
	using allocator_type = typename base_type::allocator;

	container_type _underlying_container;
};


template<class T>
class stack<T, 0, FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL> final : dynamic_stack_base<T>
{
public:
	using base_type = dynamic_stack_base<T>;
	using container_type = typename base_type::container_type;
	using pointer = typename base_type::pointer;
	using difference_type = typename base_type::difference_type;
	using const_pointer = typename base_type::const_pointer;
	using const_iterator = typename base_type::const_iterator;
	using const_reverse_iterator = typename base_type::const_reverse_iterator;
	using reference = typename base_type::reference;
	using const_reference = typename base_type::const_reference;
	using length_type = typename base_type::length_type;
	using size_type = typename base_type::size_type;
	using value_type = typename base_type::value_type;
	using alignment = typename base_type::alignment;
	using allocator_type = typename base_type::allocator;

	container_type _underlying_container;
};




template<class T, size_t max_element_count>
class static_stack_base
{
public:
	using value_type = T;
	using length_type = var::size_t;
	using size_type = var::size_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using const_iterator = FE::const_iterator<FE::contiguous_iterator<T>>;
	using const_reverse_iterator = FE::const_reverse_iterator<FE::contiguous_iterator<T>>;
	using difference_type = ptrdiff_t;

protected:
	var::byte m_memory[sizeof(value_type) * max_element_count];
	pointer m_top_ptr;
	pointer const m_absolute_begin_ptrc;

	_FORCE_INLINE_ static_stack_base() noexcept : m_memory(), m_top_ptr(reinterpret_cast<pointer>(m_memory)), m_absolute_begin_ptrc(m_top_ptr) {}
	_FORCE_INLINE_ _CONSTEXPR20_ ~static_stack_base() noexcept {}

public:
	_FORCE_INLINE_ const_reference top() const noexcept
	{
		return *(this->m_top_ptr - 1);
	}

	_NODISCARD_ _FORCE_INLINE_ var::boolean is_empty() noexcept
	{
		return (this->m_top_ptr == this->m_absolute_begin_ptrc) ? true : false;
	}

	_FORCE_INLINE_ length_type length() noexcept
	{
		return this->m_top_ptr - this->m_absolute_begin_ptrc;
	}

	_FORCE_INLINE_ size_type size() noexcept
	{
		return this->m_top_ptr - this->m_absolute_begin_ptrc;
	}

	_FORCE_INLINE_ constexpr size_type max_size() const noexcept
	{
		return max_element_count;
	}

	_FORCE_INLINE_ constexpr size_type max_length() const noexcept
	{
		return max_element_count;
	}

	_FORCE_INLINE_ const_iterator cbegin() noexcept
	{
		return this->m_absolute_begin_ptrc;
	}

	_FORCE_INLINE_ const_iterator cend() noexcept
	{
		return this->m_top_ptr;
	}

	_FORCE_INLINE_ const_reverse_iterator crbegin() noexcept
	{
		return this->m_top_ptr - 1;
	}

	_FORCE_INLINE_ const_reverse_iterator crend() noexcept
	{
		return this->m_absolute_begin_ptrc - 1;
	}

	_FORCE_INLINE_ constexpr FE::OBJECT_TRIVIALITY is_trivial() noexcept { return FE::is_trivially_constructible_and_destructible<T>::_VALUE_; }

	template<FE::OBJECT_TRIVIALITY is_trivial = FE::is_trivially_constructible_and_destructible<T>::_VALUE_>
	_FORCE_INLINE_ static void swap(stack<T, max_element_count, is_trivial>& first_ref_p, stack<T, max_element_count, is_trivial>& second_ref_p) noexcept
	{
		stack<T, max_element_count, is_trivial> l_temporary = std::move(first_ref_p);
		first_ref_p = std::move(second_ref_p);
		second_ref_p = std::move(l_temporary);
	}

	template<FE::OBJECT_TRIVIALITY is_trivial = FE::is_trivially_constructible_and_destructible<T>::_VALUE_>
	var::boolean operator==(stack<T, max_element_count, is_trivial>& other_ref_p) noexcept
	{
		return FE::memcmp_s(this->cbegin(), this->cend(), other_ref_p.cbegin(), other_ref_p.cend());
	}

	template<FE::OBJECT_TRIVIALITY is_trivial = FE::is_trivially_constructible_and_destructible<T>::_VALUE_>
	var::boolean operator!=(stack<T, max_element_count, is_trivial>& other_ref_p) noexcept
	{
		return !FE::memcmp_s(this->cbegin(), this->cend(), other_ref_p.cbegin(), other_ref_p.cend());
	}

protected:
	_FORCE_INLINE_ void __jump_top_pointer(difference_type ptrdiff_p) noexcept
	{
		this->m_top_ptr += ptrdiff_p;
	}

	_FORCE_INLINE_ void __set_top_pointer_to_zero() noexcept
	{
		this->m_top_ptr = this->m_absolute_begin_ptrc;
	}
};


template<class T, size_t max_element_count>
class stack<T, max_element_count, FE::OBJECT_TRIVIALITY::_TRIVIAL> final : public static_stack_base<T, max_element_count>
{
public:
	using base_type = static_stack_base<T, max_element_count>;
	using value_type = typename base_type::value_type;
	using length_type = typename base_type::length_type;
	using size_type = typename base_type::size_type;
	using reference = typename base_type::reference;
	using const_reference = typename base_type::const_reference;
	using pointer = typename base_type::pointer;
	using const_pointer = typename base_type::const_pointer;
	using const_iterator = typename base_type::const_iterator;
	using const_reverse_iterator = typename base_type::const_reverse_iterator;
	using difference_type = typename base_type::difference_type;

	_FORCE_INLINE_ stack() noexcept : base_type() {}
	_FORCE_INLINE_ _CONSTEXPR20_ ~stack() noexcept {}

	_FORCE_INLINE_ stack(std::initializer_list<value_type>&& initializer_list_p) noexcept : base_type()
	{
		FE_ASSERT(initializer_list_p.size() > max_element_count, "ERROR!: The length of std::initializer_list exceeds the max_element_count");

		::FE::unaligned_memcpy(this->m_memory, max_element_count, sizeof(value_type),
					initializer_list_p.begin(), initializer_list_p.size(), sizeof(value_type));

		this->__jump_top_pointer(initializer_list_p.size());
	}

	_FORCE_INLINE_ stack(stack& other_ref_p) noexcept : base_type()
	{
		::FE::unaligned_memcpy(this->m_memory, max_element_count, sizeof(value_type),
					other_ref_p.m_memory, max_element_count, sizeof(value_type));

		this->__jump_top_pointer(other_ref_p.m_top_ptr - other_ref_p.m_absolute_begin_ptrc);
	}

	_FORCE_INLINE_ stack(stack&& rvalue_p) noexcept : base_type()
	{
		::FE::unaligned_memcpy(this->m_memory, max_element_count, sizeof(value_type),
					rvalue_p.m_memory, max_element_count, sizeof(value_type));

#ifdef _DEBUG_
		UNALIGNED_MEMSET(rvalue_p.m_memory, _NULL_, max_element_count * sizeof(value_type));
#endif

		this->__jump_top_pointer(rvalue_p.m_top_ptr - rvalue_p.m_absolute_begin_ptrc);
		rvalue_p.__set_top_pointer_to_zero();
	}

	_FORCE_INLINE_ stack& operator=(std::initializer_list<value_type> initializer_list_p) noexcept
	{
		FE_ASSERT(initializer_list_p.size() > max_element_count, "ERROR!: The length of std::initializer_list exceeds the max_element_count");

		::FE::unaligned_memcpy(this->m_memory, max_element_count, sizeof(value_type),
					initializer_list_p.begin(), initializer_list_p.size(), sizeof(value_type));

		this->__set_top_pointer_to_zero();
		this->__jump_top_pointer(initializer_list_p.size());
		return *this;
	}

	_FORCE_INLINE_ stack& operator=(stack& other_ref_p) noexcept
	{
		::FE::unaligned_memcpy(this->m_memory, max_element_count, sizeof(value_type),
					other_ref_p.m_memory, max_element_count, sizeof(value_type));

		this->__set_top_pointer_to_zero();
		this->__jump_top_pointer(other_ref_p.m_top_ptr - other_ref_p.m_absolute_begin_ptrc);
		return *this;
	}

	_FORCE_INLINE_ stack& operator=(stack&& rvalue_p) noexcept
	{
		::FE::unaligned_memcpy(this->m_memory, max_element_count, sizeof(value_type),
					rvalue_p.m_memory, max_element_count, sizeof(value_type));

#ifdef _DEBUG_
		UNALIGNED_MEMSET(rvalue_p.m_memory, _NULL_, max_element_count * sizeof(value_type));
#endif

		this->__set_top_pointer_to_zero();
		this->__jump_top_pointer(rvalue_p.m_top_ptr - rvalue_p.m_absolute_begin_ptrc);
		rvalue_p.__set_top_pointer_to_zero();
		return *this;
	}

	_FORCE_INLINE_ void push(value_type value_p) noexcept
	{
		FE_ASSERT(this->m_top_ptr >= this->m_absolute_begin_ptrc + max_element_count, "${%s@0}: The stack top exceeded the index boundary", TO_STRING(FE::MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));

		*this->m_top_ptr = value_p;
		++this->m_top_ptr;
	}

	_FORCE_INLINE_ value_type pop() noexcept
	{
		FE_ASSERT(this->is_empty() == true, "${%s@0}: The stack top index reached zero. The index value_p must be greater than zero", TO_STRING(FE::MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));

		--this->m_top_ptr;
		T l_return_value_buffer = *this->m_top_ptr;
		*this->m_top_ptr = (T)_NULL_;
		return l_return_value_buffer;
	}
	
	_FORCE_INLINE_ void pop_all() noexcept
	{
		FE_ASSERT(this->is_empty() == true, "WARNING: It is pointless to pop empty elements.");

#ifdef _DEBUG_
		UNALIGNED_MEMSET(this->m_absolute_begin_ptrc, _NULL_, (this->m_top_ptr - this->m_absolute_begin_ptrc) * sizeof(T));
#endif
		this->__set_top_pointer_to_zero();
	}
};


template<class T, size_t max_element_count>
class stack<T, max_element_count, FE::OBJECT_TRIVIALITY::_NOT_TRIVIAL> final : public static_stack_base<T, max_element_count>
{
	OBJECT_LIFECYCLE m_bool_mask[max_element_count];

public:
	using base_type = static_stack_base<T, max_element_count>;
	using value_type = typename base_type::value_type;
	using length_type = typename base_type::length_type;
	using size_type = typename base_type::size_type;
	using reference = typename base_type::reference;
	using const_reference = typename base_type::const_reference;
	using pointer = typename base_type::pointer;
	using const_pointer = typename base_type::const_pointer;
	using const_iterator = typename base_type::const_iterator;
	using const_reverse_iterator = typename base_type::const_reverse_iterator;
	using difference_type = typename base_type::difference_type;

	_FORCE_INLINE_ stack() noexcept : base_type(), m_bool_mask() {}

	_FORCE_INLINE_ ~stack() noexcept
	{
		if (this->is_empty() == true) { return; }

		FE::destruct<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, this->m_top_ptr, this->m_bool_mask);
	}

	_FORCE_INLINE_ stack(std::initializer_list<value_type>&& initializer_list_p) noexcept : base_type(), m_bool_mask()
	{
		FE_ASSERT(initializer_list_p.size() > max_element_count, "ERROR!: The length of std::initializer_list exceeds the max_element_count");

		FE::move_construct<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, initializer_list_p.size(), this->m_bool_mask,
                                                                              const_cast<T*>(initializer_list_p.begin()), initializer_list_p.size());
		this->__jump_top_pointer(initializer_list_p.size());
	}

	_FORCE_INLINE_ stack(stack& other_ref_p) noexcept : base_type(), m_bool_mask()
	{
		FE::copy_construct<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, max_element_count, static_cast<OBJECT_LIFECYCLE*>(this->m_bool_mask),
                                                                              other_ref_p.m_absolute_begin_ptrc, other_ref_p.m_top_ptr - other_ref_p.m_absolute_begin_ptrc, static_cast<const OBJECT_LIFECYCLE*>(other_ref_p.m_bool_mask));
		this->__jump_top_pointer(other_ref_p.m_top_ptr - other_ref_p.m_absolute_begin_ptrc);
	}

	_FORCE_INLINE_ stack(stack&& rvalue_p) noexcept : base_type(), m_bool_mask()
	{
		FE::move_construct<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, max_element_count, static_cast<OBJECT_LIFECYCLE*>(this->m_bool_mask),
                                                                              rvalue_p.m_absolute_begin_ptrc, rvalue_p.m_top_ptr - rvalue_p.m_absolute_begin_ptrc, static_cast<OBJECT_LIFECYCLE*>(rvalue_p.m_bool_mask));
	
		FE::destruct<FE::iterator<FE::contiguous_iterator<value_type>>>(rvalue_p.m_absolute_begin_ptrc, rvalue_p.m_top_ptr, rvalue_p.m_bool_mask);

		this->__jump_top_pointer(rvalue_p.m_top_ptr - rvalue_p.m_absolute_begin_ptrc);
		rvalue_p.__set_top_pointer_to_zero();
	}

	_FORCE_INLINE_ stack& operator=(std::initializer_list<value_type> initializer_list_p) noexcept
	{
		FE_ASSERT(initializer_list_p.size() > max_element_count, "ERROR!: The length of std::initializer_list exceeds the max_element_count");

		FE::move_assign<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, initializer_list_p.size(), this->m_bool_mask,
                                                                           const_cast<T*>(initializer_list_p.begin()), initializer_list_p.size());
		this->__set_top_pointer_to_zero();
		this->__jump_top_pointer(initializer_list_p.size());
		return *this;
	}

	_FORCE_INLINE_ stack& operator=(stack& other_ref_p) noexcept
	{
		FE::copy_assign<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, max_element_count, static_cast<OBJECT_LIFECYCLE*>(this->m_bool_mask),
                                                                           other_ref_p.m_absolute_begin_ptrc, other_ref_p.m_top_ptr - other_ref_p.m_absolute_begin_ptrc, static_cast<const OBJECT_LIFECYCLE*>(other_ref_p.m_bool_mask));
		this->__set_top_pointer_to_zero();
		this->__jump_top_pointer(other_ref_p.m_top_ptr - other_ref_p.m_absolute_begin_ptrc);
		return *this;
	}

	_FORCE_INLINE_ stack& operator=(stack&& rvalue_p) noexcept
	{
		FE::move_assign<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, max_element_count, static_cast<OBJECT_LIFECYCLE*>(this->m_bool_mask),
                                                                          rvalue_p.m_absolute_begin_ptrc, rvalue_p.m_top_ptr - rvalue_p.m_absolute_begin_ptrc, static_cast<OBJECT_LIFECYCLE*>(rvalue_p.m_bool_mask));
		
		FE::destruct<FE::iterator<FE::contiguous_iterator<value_type>>>(rvalue_p.m_absolute_begin_ptrc, rvalue_p.m_top_ptr, rvalue_p.m_bool_mask);
		
		this->__set_top_pointer_to_zero();
		this->__jump_top_pointer(rvalue_p.m_top_ptr - rvalue_p.m_absolute_begin_ptrc);
		rvalue_p.__set_top_pointer_to_zero();
		return *this;
	}

	_FORCE_INLINE_ void push(value_type value_p) noexcept
	{
		FE_ASSERT(this->m_top_ptr >= this->m_absolute_begin_ptrc + max_element_count, "${%s@0}: The stack top exceeded the index boundary", TO_STRING(FE::MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));
		
		FE::assign<T, value_type>(*this->m_top_ptr, this->m_bool_mask[this->m_top_ptr - this->m_absolute_begin_ptrc], std::move(value_p));
		++this->m_top_ptr;
	}

	_FORCE_INLINE_ value_type pop() noexcept
	{
		FE_ASSERT(this->is_empty() == true, "${%s@0}: The stack top index reached zero. The index value_p must be greater than zero", TO_STRING(FE::MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));

		--this->m_top_ptr;
		T l_return_value_buffer = *this->m_top_ptr;
		FE::destruct<T>(*this->m_top_ptr, this->m_bool_mask[this->m_top_ptr - this->m_absolute_begin_ptrc]);
		return l_return_value_buffer;
	}
	
	_FORCE_INLINE_ void pop_all() noexcept
	{
		FE_ASSERT(this->is_empty() == true, "WARNING: It is pointless to pop empty elements.");

		FE::destruct<FE::iterator<FE::contiguous_iterator<value_type>>>(this->m_absolute_begin_ptrc, this->m_top_ptr, this->m_bool_mask);
		this->__set_top_pointer_to_zero();
	}
};


END_NAMESPACE
#endif
