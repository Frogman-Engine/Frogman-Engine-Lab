#ifndef _FE_CORE_UNIQUE_PTR_HXX_
#define _FE_CORE_UNIQUE_PTR_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator.hxx>
#include <FE/core/iterator.hxx>
#include <FE/core/memory.hxx>
#include <atomic>
#include <initializer_list>




BEGIN_NAMESPACE(FE)


template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::scalable_aligned_allocator<typename std::remove_all_extents<T>::type>>>
class unique_ptr final
{
	FE_STATIC_ASSERT((std::is_same<T, Allocator::value_type>::value == false), "static assertion failed: enforcing Allocator's value_type to be equivalent to the typename T. The template parameter T must be identical to the value_type of the Allocator.");
	FE_STATIC_ASSERT(std::is_class<Allocator>::value == false, "Static Assertion Failed: The template argument Allocator is not a class or a struct type.");

public:
	using pointer = typename Allocator::pointer;
	using element_type = typename Allocator::value_type;
	using allocator_type = Allocator;

private:
	pointer m_smart_ptr;

public:
	_FORCE_INLINE_ unique_ptr() noexcept : m_smart_ptr() {}
	
	_CONSTEXPR23_ ~unique_ptr() noexcept
	{
		if (this->m_smart_ptr == nullptr)
		{
			return;
		}

		Allocator::deallocate(this->m_smart_ptr, 1);
		this->m_smart_ptr = nullptr;
	}

	_CONSTEXPR20_ unique_ptr(const unique_ptr& other_p) noexcept = delete;

	_FORCE_INLINE_ unique_ptr(unique_ptr&& rvalue_p) noexcept : m_smart_ptr(rvalue_p.m_smart_ptr)
	{
		rvalue_p.m_smart_ptr = nullptr;
	}

	_FORCE_INLINE_ unique_ptr(element_type value_p) noexcept : m_smart_ptr(Allocator::allocate(1))
	{
		*this->m_smart_ptr = std::move(value_p);
	}

	_CONSTEXPR20_ unique_ptr& operator=(const unique_ptr& other_p) noexcept = delete;

	_CONSTEXPR20_ unique_ptr& operator=(unique_ptr&& rvalue_p) noexcept
	{
		if (rvalue_p.m_smart_ptr == nullptr)
		{
			return *this;
		}

		if (this->m_smart_ptr != nullptr)
		{
			Allocator::deallocate(this->m_smart_ptr, 1);
		}

		this->m_smart_ptr = rvalue_p.m_smart_ptr;
		rvalue_p.m_smart_ptr = nullptr;
		return *this;
	}

	_CONSTEXPR20_ unique_ptr& operator=(element_type value_p) noexcept
	{
		if (this->m_smart_ptr == nullptr)
		{
			this->m_smart_ptr = Allocator::allocate(1);
		}

		*this->m_smart_ptr = std::move(value_p);
		return *this;
	}

	_NODISCARD_ _CONSTEXPR20_ pointer release() noexcept
	{
		pointer l_result = this->m_smart_ptr;
		this->m_smart_ptr = nullptr;
		return l_result;
	}

	_FORCE_INLINE_ void reset() noexcept
	{
		this->~unique_ptr();
	}

	_CONSTEXPR20_ void reset(element_type value_p) noexcept
	{
		this->operator=(std::move(value_p));
	}

	_CONSTEXPR20_ void swap(unique_ptr& in_out_other_p) noexcept
	{
		pointer l_temporary_smart_ptr = in_out_other_p.m_smart_ptr;
		in_out_other_p.m_smart_ptr = this->m_smart_ptr;
		this->m_smart_ptr = l_temporary_smart_ptr;
	}

	_FORCE_INLINE_ pointer get() const noexcept
	{
		return this->m_smart_ptr;
	}

	_FORCE_INLINE_ explicit operator bool() const noexcept
	{
		return this->m_smart_ptr != nullptr;
	}

	_FORCE_INLINE_ bool operator!() const noexcept
	{
		return this->m_smart_ptr == nullptr;
	}

	_FORCE_INLINE_ element_type& operator*() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return *this->m_smart_ptr;
	}

	_FORCE_INLINE_ pointer operator->() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr;
	}


	_FORCE_INLINE_ boolean operator==(std::nullptr_t nullptr_p) const noexcept
	{
		return this->m_smart_ptr == nullptr_p;
	}

	_FORCE_INLINE_ boolean operator!=(std::nullptr_t nullptr_p) const noexcept
	{
		return this->m_smart_ptr != nullptr_p;
	}

	_FORCE_INLINE_ boolean operator==(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr == other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator!=(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr != other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator>(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr > other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator>=(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr >= other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator<(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr < other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator<=(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr <= other_p.m_smart_ptr;
	}
};

template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::scalable_aligned_allocator<T>>>
_CONSTEXPR23_ _NODISCARD_ unique_ptr<T, Allocator> make_unique() noexcept
{
	static_assert(std::is_array<T>::value == false, "static assertion failed: The typename T must not be an array type");
	return unique_ptr<T, Allocator>(T());
}

template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::scalable_aligned_allocator<T>>>
_CONSTEXPR23_ _NODISCARD_ unique_ptr<T, Allocator> make_unique(T value_p) noexcept
{
	static_assert(std::is_array<T>::value == false, "static assertion failed: The typename T must not be an array type");
	return unique_ptr<T, Allocator>( T( std::move(value_p) ) );
}




template<typename T, class Allocator>
class unique_ptr<T[], Allocator> final
{
	FE_STATIC_ASSERT((std::is_same<T, Allocator::value_type>::value == false), "static assertion failed: enforcing Allocator's value_type to be equivalent to the typename T. The template parameter T must be identical to the value_type of the Allocator.");
	FE_STATIC_ASSERT(std::is_class<Allocator>::value == false, "Static Assertion Failed: The template argument Allocator is not a class or a struct type.");

public:
	using pointer = typename Allocator::pointer;
	using element_type = typename Allocator::value_type;
	using allocator_type = Allocator;

private:
	pointer m_smart_ptr;
	pointer m_smart_ptr_end;

public:
	_FORCE_INLINE_ unique_ptr() noexcept : m_smart_ptr(), m_smart_ptr_end() {}

	_CONSTEXPR23_ ~unique_ptr() noexcept
	{
		if (this->m_smart_ptr == nullptr)
		{
			return;
		}
	
		Allocator::deallocate(this->m_smart_ptr, this->m_smart_ptr_end - this->m_smart_ptr);
		this->m_smart_ptr = nullptr;
		this->m_smart_ptr_end = nullptr;
	}

	_CONSTEXPR20_ unique_ptr(const unique_ptr& other_p) noexcept = delete;

	_CONSTEXPR20_ unique_ptr(unique_ptr&& rvalue_p) noexcept : m_smart_ptr(rvalue_p.m_smart_ptr), m_smart_ptr_end(rvalue_p.m_smart_ptr_end)
	{
		rvalue_p.m_smart_ptr = nullptr;
		rvalue_p.m_smart_ptr_end = nullptr;
	}

	_FORCE_INLINE_ unique_ptr(FE::reserve&& array_size_p) noexcept : m_smart_ptr(Allocator::allocate(array_size_p._value)), m_smart_ptr_end(m_smart_ptr + array_size_p._value)
	{
	}

	_CONSTEXPR20_ unique_ptr(std::initializer_list<element_type>&& values_p) noexcept : m_smart_ptr(Allocator::allocate( values_p.size() )), m_smart_ptr_end(m_smart_ptr + values_p.size())
	{
		if (values_p.size() == 0)
		{
			return;
		}

		this->__copy_from_initializer_list(std::move(values_p));
	}

	_CONSTEXPR20_ unique_ptr& operator=(const unique_ptr& other_p) noexcept = delete;

	_CONSTEXPR20_ unique_ptr& operator=(unique_ptr&& rvalue_p) noexcept
	{
		if (rvalue_p.m_smart_ptr == nullptr)
		{
			return *this;
		}

		if (this->m_smart_ptr != nullptr)
		{
			Allocator::deallocate(this->m_smart_ptr, this->m_smart_ptr_end - this->m_smart_ptr);
		}

		this->m_smart_ptr = rvalue_p.m_smart_ptr;
		this->m_smart_ptr_end = rvalue_p.m_smart_ptr_end;

		rvalue_p.m_smart_ptr = nullptr;
		rvalue_p.m_smart_ptr_end = nullptr;
		return *this;
	}

	_CONSTEXPR20_ unique_ptr& operator=(std::initializer_list<element_type>&& values_p) noexcept
	{
		if (values_p.size() == 0)
		{
			return *this;
		}

		this->__reallocate(values_p.size());
		this->__copy_from_initializer_list(std::move(values_p));
		return *this;
	}

	_FORCE_INLINE_ unique_ptr& operator=(FE::resize_to&& new_array_size_p) noexcept
	{
		this->__reallocate(new_array_size_p._value);
		return *this;
	}

	_NODISCARD_ _CONSTEXPR20_ pointer release() noexcept
	{
		pointer l_result = this->m_smart_ptr;
		this->m_smart_ptr = nullptr;
		this->m_smart_ptr_end = nullptr;
		return l_result;
	}

	_FORCE_INLINE_ void reset() noexcept
	{
		this->~unique_ptr();
	}

	_CONSTEXPR23_ void reset(std::initializer_list<element_type>&& values_p) noexcept
	{
		this->operator=(std::move(values_p));
	}

	_FORCE_INLINE_ void reset(FE::resize_to&& new_array_size_p) noexcept
	{
		this->operator=(std::move(new_array_size_p));
	}

	_FORCE_INLINE_ var::size_t capacity() const noexcept
	{
		return this->m_smart_ptr_end - this->m_smart_ptr;
	}

	_CONSTEXPR20_ void swap(unique_ptr& in_out_other_p) noexcept
	{
		pointer l_temporary_smart_ptr = in_out_other_p.m_smart_ptr;
		pointer l_temporary_smart_ptr_end = in_out_other_p.m_smart_ptr_end;
		in_out_other_p.m_smart_ptr = this->m_smart_ptr;
		in_out_other_p.m_smart_ptr_end = this->m_smart_ptr_end;
		this->m_smart_ptr = l_temporary_smart_ptr;
		this->m_smart_ptr_end = l_temporary_smart_ptr_end;
	}

	_FORCE_INLINE_ pointer get() const noexcept
	{
		return this->m_smart_ptr;
	}

	_FORCE_INLINE_ explicit operator bool() const noexcept
	{
		return this->m_smart_ptr != nullptr;
	}

	_FORCE_INLINE_ bool operator!() const noexcept
	{
		return this->m_smart_ptr == nullptr;
	}

	_FORCE_INLINE_ element_type& operator*() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return *this->m_smart_ptr;
	}

	_FORCE_INLINE_ pointer operator->() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr;
	}

	_FORCE_INLINE_ element_type& operator[](index_t index_p) const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		FE_ASSERT(static_cast<index_t>(this->m_smart_ptr_end - this->m_smart_ptr) <= index_p, "${%s@0}: ${%s@1} exceeds the index boundary. ${%s@1} was ${%lu@2}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE), TO_STRING(index_p), &index_p);

		return this->m_smart_ptr[index_p];
	}


	_CONSTEXPR23_ _FORCE_INLINE_ boolean operator==(std::nullptr_t nullptr_p) const noexcept
	{
		return this->m_smart_ptr == nullptr_p;
	}

	_CONSTEXPR23_ _FORCE_INLINE_ boolean operator!=(std::nullptr_t nullptr_p) const noexcept
	{
		return this->m_smart_ptr != nullptr_p;
	}

	_FORCE_INLINE_ boolean operator==(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr == other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator!=(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr != other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator>(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr > other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator>=(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr >= other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator<(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr < other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ boolean operator<=(const unique_ptr& other_p) const noexcept
	{
		return this->m_smart_ptr <= other_p.m_smart_ptr;
	}

	_FORCE_INLINE_ FE::iterator<FE::contiguous_iterator<element_type>> begin() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr;
	}
	_FORCE_INLINE_ FE::iterator<FE::contiguous_iterator<element_type>> end() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr_end;
	}
	_FORCE_INLINE_ FE::const_iterator<FE::contiguous_iterator<element_type>> cbegin() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr;
	}
	_FORCE_INLINE_ FE::const_iterator<FE::contiguous_iterator<element_type>> cend() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr_end;
	}
	_FORCE_INLINE_ FE::reverse_iterator<FE::contiguous_iterator<element_type>> rbegin() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr;
	}
	_FORCE_INLINE_ FE::reverse_iterator<FE::contiguous_iterator<element_type>> rend() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr_end;
	}
	_FORCE_INLINE_ FE::const_reverse_iterator<FE::contiguous_iterator<element_type>> crbegin() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr;
	}
	_FORCE_INLINE_ FE::const_reverse_iterator<FE::contiguous_iterator<element_type>> crend() const noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
		return this->m_smart_ptr_end;
	}
private:
	_CONSTEXPR20_ void __copy_from_initializer_list(std::initializer_list<element_type>&& values_p) noexcept
	{
		FE_ASSERT(this->m_smart_ptr == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));

		if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_TRIVIAL)
		{
			std::memcpy(this->m_smart_ptr, const_cast<element_type*>(values_p.begin()), values_p.size() * sizeof(element_type));
		}
		else if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
		{
			count_t l_initializer_list_size = values_p.size();

			pointer l_initializer_list_iterator = const_cast<pointer>(values_p.begin());
			pointer l_smart_ptr_iterator = this->m_smart_ptr;
			for (var::count_t i = 0; i < l_initializer_list_size; ++i)
			{
				*l_smart_ptr_iterator = std::move(*l_initializer_list_iterator);
				++l_smart_ptr_iterator;
				++l_initializer_list_iterator;
			}
		}
	}

	_FORCE_INLINE_ void __reallocate(size_t new_count_p) noexcept
	{
		this->m_smart_ptr = Allocator::reallocate(this->m_smart_ptr, this->m_smart_ptr_end - this->m_smart_ptr, new_count_p);
		this->m_smart_ptr_end = this->m_smart_ptr + new_count_p;
	}
};

template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::scalable_aligned_allocator<typename std::remove_all_extents<T>::type>>>
_CONSTEXPR23_ _NODISCARD_ unique_ptr<typename std::remove_all_extents<T>::type[], Allocator> make_unique(size_t array_size_p) noexcept
{
	static_assert(std::is_array<T>::value == true, "static assertion failed: The typename T must be an array type");
	return unique_ptr<typename std::remove_all_extents<T>::type[], Allocator>(FE::reserve{ array_size_p });
}

template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::scalable_aligned_allocator<typename std::remove_all_extents<T>::type>>>
_CONSTEXPR23_ _NODISCARD_ unique_ptr<typename std::remove_all_extents<T>::type[], Allocator> make_unique(std::initializer_list<typename std::remove_all_extents<T>::type>&& values_p) noexcept
{
	static_assert(std::is_array<T>::value == true, "static assertion failed: The typename T must be an array type");
	return unique_ptr<typename std::remove_all_extents<T>::type[], Allocator>(std::move(values_p));
}








namespace concurrency
{
	template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::cache_aligned_allocator<typename std::remove_all_extents<T>::type>>>
	class unique_ptr final
	{
		FE_STATIC_ASSERT((std::is_same<T, Allocator::value_type>::value == false), "static assertion failed: enforcing Allocator's value_type to be equivalent to the typename T. The template parameter T must be identical to the value_type of the Allocator.");
		FE_STATIC_ASSERT(std::is_class<Allocator>::value == false, "Static Assertion Failed: The template argument Allocator is not a class or a struct type.");

	public:
		using pointer = typename Allocator::pointer;
		using element_type = typename Allocator::value_type;
		using allocator_type = Allocator;

	private:
		std::atomic<pointer> m_smart_ptr;

	public:
		_CONSTEXPR20_ unique_ptr() noexcept : m_smart_ptr() {}

		_CONSTEXPR23_ ~unique_ptr() noexcept
		{
			this->__wait_if_in_progress();
			/*
			1) l_pending_kill = this->m_smart_ptr;
			2) this->m_smart_ptr = nullptr;
			are done at once.
			*/
			pointer l_pending_kill = this->m_smart_ptr.exchange(nullptr);
	
			if (l_pending_kill == nullptr)
			{
				return;
			}

			Allocator::deallocate(l_pending_kill, 1);
		}

		_CONSTEXPR20_ unique_ptr(const unique_ptr& other_p) noexcept = delete;

		_CONSTEXPR20_ unique_ptr(unique_ptr&& rvalue_p) noexcept : m_smart_ptr(rvalue_p.m_smart_ptr.exchange(nullptr)) {}

		_CONSTEXPR20_ unique_ptr(element_type value_p) noexcept : m_smart_ptr(Allocator::allocate(1))
		{
			*this->m_smart_ptr = std::move(value_p);
		}

		_CONSTEXPR20_ unique_ptr& operator=(const unique_ptr& other_p) noexcept = delete;

		_CONSTEXPR20_ unique_ptr& operator=(unique_ptr&& rvalue_p) noexcept
		{
			pointer l_ownership_holder = rvalue_p.m_smart_ptr.exchange(nullptr);
			if (l_ownership_holder == nullptr)
			{
				return *this;
			}

			pointer l_previous_resource = this->m_smart_ptr.exchange(l_ownership_holder);
			if (l_previous_resource != nullptr)
			{
				Allocator::deallocate(l_previous_resource, 1);
			}
			return *this;
		}
		
		_CONSTEXPR20_ unique_ptr& operator=(element_type value_p) noexcept
		{
			// The first thread reaches this code has the authority to allocate the memory. Others will be ignored.
			pointer l_allocate_if_smart_ptr_was_null = this->m_smart_ptr.exchange(FE::concurrency::in_progress_signalptr_t<T>::value); // At this point this->m_smart_ptr is 0x1.
			
			if (l_allocate_if_smart_ptr_was_null == nullptr) // This if-statement cannot concurrently result in true.
			{
				l_allocate_if_smart_ptr_was_null = Allocator::allocate(1);
			}

			if (l_allocate_if_smart_ptr_was_null != FE::concurrency::in_progress_signalptr_t<T>::value)
			{
				*l_allocate_if_smart_ptr_was_null = std::move(value_p);
				this->m_smart_ptr.store(l_allocate_if_smart_ptr_was_null);
			}
			return *this;
		}

		_NODISCARD_ _CONSTEXPR20_ pointer release() noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.exchange(nullptr);
		}

		_FORCE_INLINE_ void reset() noexcept
		{
			this->~unique_ptr();
		}

		_CONSTEXPR20_ void reset(element_type value_p) noexcept
		{
			this->operator=(std::move(value_p));
		}

		_CONSTEXPR20_ void swap(unique_ptr& in_out_other_p) noexcept
		{
			in_out_other_p.__wait_if_in_progress();
			pointer l_temporary_other_smart_ptr = in_out_other_p.m_smart_ptr.exchange(nullptr);

			this->__wait_if_in_progress();
			pointer l_temporary_this_smart_ptr = this->m_smart_ptr.exchange(nullptr);

			in_out_other_p.m_smart_ptr.store(l_temporary_this_smart_ptr);
			this->m_smart_ptr.store(l_temporary_other_smart_ptr);
		}

		_FORCE_INLINE_ pointer get(std::memory_order order_p = std::memory_order_seq_cst) const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load(order_p);
		}

		_FORCE_INLINE_ explicit operator bool() const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() != nullptr;
		}

		_FORCE_INLINE_ bool operator!() const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() == nullptr;
		}

		_FORCE_INLINE_ element_type& operator*() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load() == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return *(this->m_smart_ptr.load());
		}

		_FORCE_INLINE_ pointer operator->() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load() == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load();
		}


		_FORCE_INLINE_ boolean operator==(std::nullptr_t nullptr_p) const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() == nullptr_p;
		}

		_FORCE_INLINE_ boolean operator!=(std::nullptr_t nullptr_p) const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() != nullptr_p;
		}

		_FORCE_INLINE_ boolean operator==(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() == other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator!=(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() != other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator>(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() > other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator>=(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() >= other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator<(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() < other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator<=(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() <= other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ void __wait_if_in_progress() const noexcept
		{
			while (this->m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value) {}
		}

		_FORCE_INLINE_ void __wait_if_in_progress(const unique_ptr& other_p) const noexcept
		{
			while ((this->m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value) || (other_p.m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value)) {}
		}
	};

	template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::cache_aligned_allocator<T>>>
	_CONSTEXPR23_ _NODISCARD_ unique_ptr<T, Allocator> make_unique() noexcept
	{
		static_assert(std::is_array<T>::value == false, "static assertion failed: The typename T must not be an array type");
		return unique_ptr<T, Allocator>(T());
	}

	template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::cache_aligned_allocator<T>>>
	_CONSTEXPR23_ _NODISCARD_ unique_ptr<T, Allocator> make_unique(T value_p) noexcept
	{
		static_assert(std::is_array<T>::value == false, "static assertion failed: The typename T must not be an array type");
		return unique_ptr<T, Allocator>(T(std::move(value_p)));
	}




	template<typename T, class Allocator>
	class unique_ptr<T[], Allocator> final
	{
		FE_STATIC_ASSERT((std::is_same<T, Allocator::value_type>::value == false), "static assertion failed: enforcing Allocator's value_type to be equivalent to the typename T. The template parameter T must be identical to the value_type of the Allocator.");
		FE_STATIC_ASSERT(std::is_class<Allocator>::value == false, "Static Assertion Failed: The template argument Allocator is not a class or a struct type.");

	public:
		using pointer = typename Allocator::pointer;
		using element_type = typename Allocator::value_type;
		using allocator_type = Allocator;

	private:
		std::atomic<pointer> m_smart_ptr = nullptr;
		std::atomic<pointer> m_smart_ptr_end = nullptr;

	public:
		_CONSTEXPR20_ unique_ptr() noexcept : m_smart_ptr(), m_smart_ptr_end() {}

		_CONSTEXPR23_ ~unique_ptr() noexcept
		{
			this->__wait_if_in_progress();

			/*
			1) l_pending_kill = this->m_smart_ptr;
			2) this->m_smart_ptr = nullptr;
			are done at once.
			*/
			pointer l_pending_kill = this->m_smart_ptr.exchange(nullptr);
			pointer l_pending_kill_end = this->m_smart_ptr_end.exchange(nullptr);

			if (l_pending_kill == nullptr)
			{
				return;
			}

			Allocator::deallocate(l_pending_kill, l_pending_kill_end - l_pending_kill);
		}

		_CONSTEXPR20_ unique_ptr(const unique_ptr& other_p) noexcept = delete;

		_CONSTEXPR20_ unique_ptr(unique_ptr&& rvalue_p) noexcept : m_smart_ptr(rvalue_p.m_smart_ptr.exchange(nullptr)), m_smart_ptr_end(rvalue_p.m_smart_ptr_end.exchange(nullptr)) {}

		_CONSTEXPR20_ unique_ptr(FE::reserve&& array_size_p) noexcept : m_smart_ptr(Allocator::allocate(array_size_p._value)), m_smart_ptr_end(m_smart_ptr.load(std::memory_order_relaxed) + array_size_p._value) {}

		_CONSTEXPR20_ unique_ptr(std::initializer_list<element_type>&& values_p) noexcept : m_smart_ptr(Allocator::allocate(values_p.size())), m_smart_ptr_end(m_smart_ptr.load(std::memory_order_relaxed) + values_p.size())
		{
			if (values_p.size() == 0)
			{
				return;
			}

			this->__copy_from_initializer_list(std::move(values_p));
		}

		_CONSTEXPR20_ unique_ptr& operator=(const unique_ptr& other_p) noexcept = delete;

		_CONSTEXPR20_ unique_ptr& operator=(unique_ptr&& rvalue_p) noexcept
		{
			pointer l_ownership_holder = rvalue_p.m_smart_ptr.exchange(nullptr);
			if (l_ownership_holder == nullptr)
			{
				return *this;
			}

			pointer l_previous_resource = this->m_smart_ptr.exchange(l_ownership_holder);
			pointer l_previous_resource_end = this->m_smart_ptr_end.exchange( rvalue_p.m_smart_ptr_end.exchange(nullptr) );
			if (l_previous_resource != nullptr)
			{
				Allocator::deallocate(l_previous_resource, l_previous_resource_end - l_previous_resource);
			}
			return *this;
		}

		_CONSTEXPR20_ unique_ptr& operator=(std::initializer_list<element_type>&& values_p) noexcept
		{
			if (values_p.size() == 0)
			{
				return *this;
			}

			this->__reallocate(values_p.size());
			this->__copy_from_initializer_list(std::move(values_p));
			return *this;
		}

		_FORCE_INLINE_ unique_ptr& operator=(FE::resize_to&& new_array_size_p) noexcept
		{
			this->__reallocate(new_array_size_p._value);
			return *this;
		}

		_NODISCARD_ _CONSTEXPR20_ pointer release() noexcept
		{
			this->__wait_if_in_progress();
			this->m_smart_ptr_end.store(nullptr);	
			return this->m_smart_ptr.exchange(nullptr);
		}

		_FORCE_INLINE_ void reset() noexcept
		{
			this->~unique_ptr();
		}

		_CONSTEXPR23_ void reset(std::initializer_list<element_type>&& values_p) noexcept
		{
			this->operator=(std::move(values_p));
		}

		_FORCE_INLINE_ void reset(FE::resize_to&& new_array_size_p) noexcept
		{
			this->operator=(std::move(new_array_size_p));
		}

		_FORCE_INLINE_ var::size_t capacity() const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr_end.load() - this->m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean is_being_used() const noexcept
		{
			return this->m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value;
		}

		_CONSTEXPR20_ void swap(unique_ptr& in_out_other_p) noexcept
		{
			in_out_other_p.__wait_if_in_progress();
			pointer l_temporary_other_smart_ptr = in_out_other_p.m_smart_ptr.exchange(nullptr);
			pointer l_temporary_other_smart_ptr_end = in_out_other_p.m_smart_ptr_end.exchange(nullptr, std::memory_order_acq_rel);

			this->__wait_if_in_progress();
			pointer l_temporary_this_smart_ptr = this->m_smart_ptr.exchange(nullptr);
			pointer l_temporary_this_smart_ptr_end = this->m_smart_ptr_end.exchange(nullptr, std::memory_order_acq_rel);

			in_out_other_p.m_smart_ptr.store(l_temporary_this_smart_ptr);
			this->m_smart_ptr.store(l_temporary_other_smart_ptr);

			in_out_other_p.m_smart_ptr_end.store(l_temporary_this_smart_ptr_end, std::memory_order_acq_rel);
			this->m_smart_ptr_end.store(l_temporary_other_smart_ptr_end, std::memory_order_acq_rel);
		}

		_FORCE_INLINE_ pointer get(std::memory_order order_p = std::memory_order_seq_cst) const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load(order_p);
		}

		_FORCE_INLINE_ explicit operator bool() const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() != nullptr;
		}

		_FORCE_INLINE_ bool operator!() const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() == nullptr;
		}

		_FORCE_INLINE_ element_type& operator*() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load() == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return *(this->m_smart_ptr.load());
		}

		_FORCE_INLINE_ pointer operator->() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load() == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load();
		}

		_FORCE_INLINE_ element_type& operator[](index_t index_p) const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load() == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			FE_ASSERT(static_cast<index_t>(this->m_smart_ptr_end.load() - this->m_smart_ptr.load()) <= index_p, "${%s@0}: ${%s@1} exceeds the index boundary. ${%s@1} was ${%lu@2}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE), TO_STRING(index_p), &index_p);
			return this->m_smart_ptr.load()[index_p];
		}


		_CONSTEXPR23_ _FORCE_INLINE_ boolean operator==(std::nullptr_t nullptr_p) const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() == nullptr_p;
		}

		_CONSTEXPR23_ _FORCE_INLINE_ boolean operator!=(std::nullptr_t nullptr_p) const noexcept
		{
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load() != nullptr_p;
		}

		_FORCE_INLINE_ boolean operator==(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() == other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator!=(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() != other_p.m_smart_pt.load();
		}

		_FORCE_INLINE_ boolean operator>(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() > other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator>=(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() >= other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator<(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() < other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ boolean operator<=(const unique_ptr& other_p) const noexcept
		{
			this->__wait_if_in_progress(other_p);
			return this->m_smart_ptr.load() <= other_p.m_smart_ptr.load();
		}

		_FORCE_INLINE_ FE::iterator<FE::contiguous_iterator<element_type>> unsafe_begin() const noexcept 
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::iterator<FE::contiguous_iterator<element_type>> unsafe_end() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr_end.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::const_iterator<FE::contiguous_iterator<element_type>> unsafe_cbegin() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::const_iterator<FE::contiguous_iterator<element_type>> unsafe_cend() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr_end.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::reverse_iterator<FE::contiguous_iterator<element_type>> unsafe_rbegin() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::reverse_iterator<FE::contiguous_iterator<element_type>> unsafe_rend() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr_end.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::const_reverse_iterator<FE::contiguous_iterator<element_type>> unsafe_crbegin() const noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr.load(std::memory_order_acquire);
		}
		_FORCE_INLINE_ FE::const_reverse_iterator<FE::contiguous_iterator<element_type>> unsafe_crend() const noexcept 
		{
			FE_ASSERT(this->m_smart_ptr.load(std::memory_order_acquire) == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));
			this->__wait_if_in_progress();
			return this->m_smart_ptr_end.load(std::memory_order_acquire); 
		}


	private:
		_CONSTEXPR20_ void __copy_from_initializer_list(std::initializer_list<element_type>&& values_p) noexcept
		{
			FE_ASSERT(this->m_smart_ptr.load() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_ptr));

			pointer const l_temporary_ownership_holder = this->m_smart_ptr.exchange(FE::concurrency::in_progress_signalptr_t<T>::value);
			if (l_temporary_ownership_holder != FE::concurrency::in_progress_signalptr_t<T>::value)
			{
				if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_TRIVIAL)
				{
					std::memcpy(l_temporary_ownership_holder, const_cast<element_type*>(values_p.begin()), values_p.size() * sizeof(element_type));
				}
				else if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
				{
					count_t l_initializer_list_size = values_p.size();
					pointer l_smart_ptr_iterator = l_temporary_ownership_holder;
					pointer l_initializer_list_iterator = const_cast<pointer>(values_p.begin());

					for (var::count_t i = 0; i < l_initializer_list_size; ++i)
					{
						*l_smart_ptr_iterator = std::move(*l_initializer_list_iterator);
						++l_smart_ptr_iterator;
						++l_initializer_list_iterator;
					}
				}

				this->m_smart_ptr.store(l_temporary_ownership_holder);
			}
		}

		_FORCE_INLINE_ void __reallocate(size_t new_count_p) noexcept
		{
			pointer l_temporary_ownership_holder = this->m_smart_ptr.exchange(FE::concurrency::in_progress_signalptr_t<T>::value);
			if (l_temporary_ownership_holder != FE::concurrency::in_progress_signalptr_t<T>::value)
			{
				l_temporary_ownership_holder = Allocator::reallocate(l_temporary_ownership_holder, this->m_smart_ptr_end.load(std::memory_order_acquire) - l_temporary_ownership_holder, new_count_p);
				this->m_smart_ptr_end.store(l_temporary_ownership_holder + new_count_p, std::memory_order_release);
				this->m_smart_ptr.store(l_temporary_ownership_holder);
			}
		}

		_FORCE_INLINE_ void __wait_if_in_progress() const noexcept
		{
			while (this->m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value) {}
		}

		_FORCE_INLINE_ void __wait_if_in_progress(const unique_ptr& other_p) const noexcept
		{
			while ((this->m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value) || (other_p.m_smart_ptr.load() == FE::concurrency::in_progress_signalptr_t<T>::value)) {}
		}
	};

	template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::cache_aligned_allocator<typename std::remove_all_extents<T>::type>>>
	_CONSTEXPR23_ _NODISCARD_ unique_ptr<typename std::remove_all_extents<T>::type[], Allocator> make_unique(size_t array_size_p) noexcept
	{
		static_assert(std::is_array<T>::value == true, "static assertion failed: The typename T must be an array type");
		return unique_ptr<typename std::remove_all_extents<T>::type[], Allocator>(FE::reserve{ array_size_p });
	}

	template<typename T, class Allocator = FE::new_delete_proxy_allocator<FE::cache_aligned_allocator<typename std::remove_all_extents<T>::type>>>
	_CONSTEXPR23_ _NODISCARD_ unique_ptr<typename std::remove_all_extents<T>::type[], Allocator> make_unique(std::initializer_list<typename std::remove_all_extents<T>::type>&& values_p) noexcept
	{
		static_assert(std::is_array<T>::value == true, "static assertion failed: The typename T must be an array type");
		return unique_ptr<typename std::remove_all_extents<T>::type[], Allocator>(std::move(values_p));
	}
}


template<typename T, class Allocator>
using atomic_unique_ptr = FE::concurrency::unique_ptr<T, Allocator>;


END_NAMESPACE
#endif