﻿#ifndef _FE_CORE_QUEUE_HXX_
#define _FE_CORE_QUEUE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/iterator.hxx>
#include <FE/core/memory.hxx>
#include <initializer_list>




BEGIN_NAMESPACE(FE)


template<class container>
class queue
{
public:
	using container_type = container;
	using pointer = typename container::pointer;
	using difference_type = typename container::difference_type;
	using const_pointer = typename container::const_pointer;
	using const_iterator = typename container::const_iterator;
	using const_reverse_iterator = typename container::const_reverse_iterator;
	using reference = typename container::reference;
	using const_reference = typename container::const_reference;
	using length_type = typename container::length_type;
	using size_type = typename container::size_type;
	using value_type = typename container::value_type;
	using alignment = typename container::alignment;
	using allocator_type = typename container::allocator;

	container_type _underlying_container;

	_FORCE_INLINE_ queue() noexcept {}
	_FORCE_INLINE_ _CONSTEXPR20_ ~queue() noexcept {}
};




template<class T, size_t max_element_count, class type_trait = FE::type_trait<T, FE::stack_memory<T>>>
class fqueue final
{
public:
	using value_type = T;
	using length_type = var::count_t;
	using size_type = var::count_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using const_iterator = FE::const_iterator<FE::contiguous_iterator<T>>;
	using const_reverse_iterator = FE::const_reverse_iterator<FE::contiguous_iterator<T>>;
	using difference_type = var::ptrdiff_t;

protected:
	var::byte m_memory[sizeof(value_type) * max_element_count];
	pointer m_front_ptr;
	pointer m_back_ptr;
	pointer const m_absolute_begin_ptrc;
	size_type m_element_count;

public:
	_CONSTEXPR20_ _FORCE_INLINE_ fqueue() noexcept : m_memory(), m_front_ptr(reinterpret_cast<pointer>(m_memory)), m_back_ptr(m_front_ptr), m_absolute_begin_ptrc(m_front_ptr), m_element_count() {}
	_CONSTEXPR20_ _FORCE_INLINE_ ~fqueue() noexcept { this->pop_all(); }

	_CONSTEXPR20_ _FORCE_INLINE_ fqueue(std::initializer_list<value_type>&& initializer_list_p) noexcept : m_memory(), m_front_ptr(reinterpret_cast<pointer>(m_memory)), m_back_ptr(m_front_ptr), m_absolute_begin_ptrc(m_front_ptr), m_element_count(initializer_list_p.size())
	{
		FE_ASSERT(initializer_list_p.size() > max_element_count, "${%s@0}!: The length of std::initializer_list exceeds the max_element_count", TO_STRING(MEMORY_ERROR_1XX::_ERROR_INVALID_SIZE));
		FE_ASSERT(initializer_list_p.size() == 0, "${%s@0}!: Cannot assign an empty initializer_list", TO_STRING(MEMORY_ERROR_1XX::_ERROR_INVALID_SIZE));

		type_trait::move_construct(this->m_front_ptr, const_cast<value_type*>(initializer_list_p.begin()), initializer_list_p.size());

		this->__jump_back_pointer(initializer_list_p.size());
	}

	_CONSTEXPR20_ _FORCE_INLINE_ fqueue(fqueue& other_ref_p) noexcept : m_memory(), m_front_ptr(reinterpret_cast<pointer>(m_memory)), m_back_ptr(m_front_ptr), m_absolute_begin_ptrc(m_front_ptr), m_element_count(other_ref_p.m_element_count)
	{
		if (other_ref_p.is_empty())
		{
			return;
		}

		type_trait::copy_construct(this->m_front_ptr, other_ref_p.m_front_ptr, other_ref_p.size());

		this->__jump_front_pointer(other_ref_p.m_front_ptr - other_ref_p.m_absolute_begin_ptrc);
		this->__jump_back_pointer(other_ref_p.m_back_ptr - other_ref_p.m_front_ptr);
	}

	_CONSTEXPR20_ _FORCE_INLINE_ fqueue(fqueue&& rvalue_p) noexcept : m_memory(), m_front_ptr(reinterpret_cast<pointer>(m_memory)), m_back_ptr(m_front_ptr), m_absolute_begin_ptrc(m_front_ptr), m_element_count(rvalue_p.m_element_count)
	{
		if (rvalue_p.is_empty())
		{
			return;
		}

		type_trait::move_construct(this->m_front_ptr, rvalue_p.m_front_ptr, rvalue_p.size());

		this->__jump_front_pointer(rvalue_p.m_front_ptr - rvalue_p.m_absolute_begin_ptrc);
		this->__jump_back_pointer(rvalue_p.m_back_ptr - rvalue_p.m_front_ptr);
		rvalue_p.__set_front_pointer_to_zero();
		rvalue_p.__set_back_pointer_to_zero();
	}

	_CONSTEXPR20_ _FORCE_INLINE_ fqueue& operator=(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		FE_ASSERT(initializer_list_p.size() > max_element_count, "${%s@0}!: The length of std::initializer_list exceeds the max_element_count", TO_STRING(MEMORY_ERROR_1XX::_ERROR_INVALID_SIZE));
		FE_ASSERT(initializer_list_p.size() == 0, "${%s@0}!: Cannot assign an empty initializer_list", TO_STRING(MEMORY_ERROR_1XX::_ERROR_INVALID_SIZE));

		this->~fqueue();
		new(this) fqueue(std::move(initializer_list_p));
		return *this;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ fqueue& operator=(fqueue& other_ref_p) noexcept
	{
		if (other_ref_p.is_empty())
		{
			return *this;
		}

		this->~fqueue();
		new(this) fqueue(other_ref_p);
		return *this;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ fqueue& operator=(fqueue&& rvalue_p) noexcept
	{
		if (rvalue_p.is_empty())
		{
			return *this;
		}

		this->~fqueue();
		new(this) fqueue(std::move(rvalue_p));
		return *this;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ void push(value_type value_p) noexcept
	{
		FE_ASSERT(this->is_empty() && (this->m_absolute_begin_ptrc != this->m_back_ptr), "${%s@0}: Exceeded the queue index boundary", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));

		if (this->m_back_ptr >= this->m_absolute_begin_ptrc + max_element_count)
		{
			this->__set_back_pointer_to_zero();
			FE_ASSERT(this->m_back_ptr >= this->m_front_ptr, "${%s@0}: Exceeded the queue index boundary", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));
		}

		type_trait::construct(*this->m_back_ptr, std::move(value_p));
		++this->m_back_ptr;
		++this->m_element_count;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ value_type pop() noexcept
	{
		if ((this->m_absolute_begin_ptrc + max_element_count) == this->m_front_ptr)
		{
			this->m_front_ptr = this->m_absolute_begin_ptrc;
			FE_ASSERT(this->is_empty() == true, "${%s@0}: Exceeded the queue index boundary", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));
		}

		T l_return_value_buffer = std::move(*this->m_front_ptr);

		if constexpr (type_trait::is_trivially_constructible_and_destructible == TYPE_TRIVIALITY::_NOT_TRIVIAL)
		{
			type_trait::destruct(*this->m_front_ptr);
		}

		++this->m_front_ptr;
		--this->m_element_count;
		return std::move(l_return_value_buffer);
	}

	_CONSTEXPR20_ _FORCE_INLINE_ void pop_all() noexcept
	{
		if (this->is_empty() == false)
		{
			if constexpr (type_trait::is_trivially_constructible_and_destructible == TYPE_TRIVIALITY::_NOT_TRIVIAL)
			{
				if (this->m_back_ptr > this->m_front_ptr)
				{
					type_trait::destruct(this->m_front_ptr, this->m_back_ptr);
				}
				else
				{
					type_trait::destruct(this->m_back_ptr, this->m_front_ptr);
				}
			}

			this->__set_front_pointer_to_zero();
			this->__set_back_pointer_to_zero();
			this->m_element_count = 0;
		}
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ const_reference front() const noexcept
	{
		return *this->m_front_ptr;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ const_reference back() const noexcept
	{
		return *(this->m_back_ptr - 1);
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ var::boolean is_empty() noexcept
	{
		return this->m_element_count == 0;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ size_type count() noexcept
	{
		return this->m_element_count;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ size_type size() noexcept
	{
		return this->m_element_count;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ size_type max_size() const noexcept
	{
		return max_element_count;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ size_type capacity() const noexcept
	{
		return max_element_count;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ const_iterator cbegin() noexcept
	{
		return this->m_front_ptr;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ const_iterator cend() noexcept
	{
		return this->m_front_ptr + max_element_count;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ const_reverse_iterator crbegin() noexcept
	{
		return (this->m_front_ptr + max_element_count) - 1;
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ const_reverse_iterator crend() noexcept
	{
		return this->m_front_ptr - 1;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ void swap(fqueue& second_ref_p) noexcept
	{
		fqueue l_temporary = std::move(*this);
		*this = std::move(second_ref_p);
		second_ref_p = std::move(l_temporary);
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ var::boolean operator==(fqueue& other_ref_p) noexcept
	{
		return FE::memcmp_s(this->cbegin(), this->cend(), other_ref_p.cbegin(), other_ref_p.cend());
	}

	_CONSTEXPR20_ _NODISCARD_ _FORCE_INLINE_ var::boolean operator!=(fqueue& other_ref_p) noexcept
	{
		return !FE::memcmp_s(this->cbegin(), this->cend(), other_ref_p.cbegin(), other_ref_p.cend());
	}

protected:
	_CONSTEXPR20_ _FORCE_INLINE_ void __jump_front_pointer(difference_type ptrdiff_p) noexcept
	{
		this->m_front_ptr += ptrdiff_p;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ void __set_front_pointer_to_zero() noexcept
	{
		this->m_front_ptr = this->m_absolute_begin_ptrc;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ void __jump_back_pointer(difference_type ptrdiff_p) noexcept
	{
		this->m_back_ptr += ptrdiff_p;
	}

	_CONSTEXPR20_ _FORCE_INLINE_ void __set_back_pointer_to_zero() noexcept
	{
		this->m_back_ptr = this->m_absolute_begin_ptrc;
	}
};


END_NAMESPACE
#endif