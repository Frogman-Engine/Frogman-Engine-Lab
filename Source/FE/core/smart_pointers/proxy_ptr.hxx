#ifndef _FE_CORE_PROXY_PTR_HXX_
#define _FE_CORE_PROXY_PTR_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/smart_pointers/private/ref_table.hxx>
#include <FE/core/iterator.hxx>
#include <atomic>
#include <initializer_list>




BEGIN_NAMESPACE(FE)


template<typename T>
class proxy_ptr final
{
	using ref_table_type = internal::ref_table_for_proxy_ptr<T>;
	using ref_table_key_type = typename ref_table_type::ref_table_key_type;

public:
	using pointer = typename std::remove_all_extents<T>::type*;
	using element_type = typename std::remove_all_extents<T>::type;

private:
	mutable exclusive_ptr_base<element_type>* m_exclusive_ptr;
	mutable ref_table_key_type m_ref_table_key;

public:
	_CONSTEXPR20_ proxy_ptr() noexcept : m_exclusive_ptr(), m_ref_table_key(ref_table_type::invalid_key_value) {}

	_CONSTEXPR20_ proxy_ptr(const proxy_ptr& other_p) noexcept : m_exclusive_ptr(ref_table_type::__get_ref(other_p.m_ref_table_key)), m_ref_table_key(ref_table_type::invalid_key_value)
	{
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = other_p.m_ref_table_key;
		}
	}

	_CONSTEXPR20_ proxy_ptr(proxy_ptr&& rvalue_p) noexcept : m_exclusive_ptr(ref_table_type::__get_ref(rvalue_p.m_ref_table_key)), m_ref_table_key(ref_table_type::invalid_key_value)
	{
		if (this->m_exclusive_ptr != nullptr)
		{
			rvalue_p.m_exclusive_ptr = nullptr;
			this->m_ref_table_key = rvalue_p.m_ref_table_key;
			rvalue_p.m_ref_table_key = ref_table_type::invalid_key_value;
		}
	}

	template<class Allocator>
	_CONSTEXPR20_ proxy_ptr(const FE::exclusive_ptr<T, Allocator>& exclusive_ptr_p) noexcept : m_exclusive_ptr(ref_table_type::__get_ref(exclusive_ptr_p.get_ref_table_key())), m_ref_table_key(ref_table_type::invalid_key_value)
	{
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = exclusive_ptr_p.get_ref_table_key();
		}
	}

	_CONSTEXPR20_ proxy_ptr& operator=(const proxy_ptr& other_p) noexcept
	{
		if (other_p.m_exclusive_ptr == nullptr)
		{
			return *this;
		}

		this->m_exclusive_ptr = ref_table_type::__get_ref(other_p.m_ref_table_key);
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = other_p.m_ref_table_key;
		}
		return *this;
	}

	_CONSTEXPR20_ proxy_ptr& operator=(proxy_ptr&& rvalue_p) noexcept
	{
		if (rvalue_p.m_exclusive_ptr == nullptr)
		{
			return *this;
		}

		this->m_exclusive_ptr = ref_table_type::__get_ref(rvalue_p.m_ref_table_key);
		if (this->m_exclusive_ptr != nullptr)
		{
			rvalue_p.m_exclusive_ptr = nullptr;

			this->m_ref_table_key = rvalue_p.m_ref_table_key;
			rvalue_p.m_ref_table_key = ref_table_type::invalid_key_value;
		}
		return *this;
	}

	template<class Allocator>
	_CONSTEXPR20_ proxy_ptr& operator=(const FE::exclusive_ptr<T, Allocator>& exclusive_ptr_p) noexcept
	{
		if (!exclusive_ptr_p)
		{
			return *this;
		}
		
		ref_table_key_type l_retrieved_ref_table_key = exclusive_ptr_p.get_ref_table_key();
		this->m_exclusive_ptr = ref_table_type::__get_ref(l_retrieved_ref_table_key);
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = l_retrieved_ref_table_key;
		}
		return *this;
	}

	_FORCE_INLINE_ void reset() noexcept
	{
		this->m_exclusive_ptr = nullptr;
		this->m_ref_table_key = ref_table_type::invalid_key_value;
	}

	_FORCE_INLINE_ ref_table_key_type get_ref_table_key() const noexcept
	{
		return this->m_ref_table_key;
	}

 	_CONSTEXPR20_ void swap(proxy_ptr& in_out_other_p) noexcept
	{
		this->__validate_my_ref();
		in_out_other_p.__validate_my_ref();

		pointer l_temporary_smart_ptr = in_out_other_p.m_exclusive_ptr;
		in_out_other_p.m_exclusive_ptr = this->m_exclusive_ptr;
		this->m_exclusive_ptr = l_temporary_smart_ptr;

		ref_table_key_type l_temporary_key = in_out_other_p.m_ref_table_key;
		in_out_other_p.m_ref_table_key = this->m_ref_table_key;
		this->m_ref_table_key = l_temporary_key;
	}


	_FORCE_INLINE_ boolean is_expired() const noexcept
	{
		this->__validate_my_ref();
		return ((this->m_exclusive_ptr == nullptr) || (this->m_exclusive_ptr->get() == nullptr));
	}

	_FORCE_INLINE_ pointer get_unchecked() const noexcept
	{
		FE_ASSERT(this->m_exclusive_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ explicit operator bool() const noexcept
	{
		return !(this->is_expired());
	}

	_FORCE_INLINE_ boolean operator!() const noexcept
	{
		return this->is_expired();
	}

	_FORCE_INLINE_ element_type& operator*() const noexcept
	{
		this->__validate_my_ref();
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is pointing to an invalid object", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr->get()));
		return *(this->m_exclusive_ptr->get());
	}

	_FORCE_INLINE_ pointer operator->() const noexcept
	{
		this->__validate_my_ref();
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is pointing to an invalid object", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr->get()));
		return this->m_exclusive_ptr->get();
	}


	_FORCE_INLINE_ boolean operator==(std::nullptr_t nullptr_p) const noexcept
	{
		this->__validate_my_ref();

		if(this->m_exclusive_ptr == nullptr)
		{
			return true;
		}

		return this->m_exclusive_ptr->get() == nullptr_p;
	}

	_FORCE_INLINE_ boolean operator!=(std::nullptr_t nullptr_p) const noexcept
	{
		this->__validate_my_ref();

		if (this->m_exclusive_ptr != nullptr)
		{
			return this->m_exclusive_ptr->get() != nullptr_p;
		}

		return false;
	}


	_FORCE_INLINE_ boolean operator==(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() == other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator!=(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() != other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator>(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() > other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator>=(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() >= other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator<(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() < other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator<=(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() <= other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator==(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() == other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator!=(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() != other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator>(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() > other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator>=(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() >= other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator<(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() < other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator<=(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() <= other_p.m_exclusive_ptr->get();
	}

private:
	_FORCE_INLINE_ void __validate_my_ref() const noexcept
	{
		this->m_exclusive_ptr = ref_table_type::__get_ref(this->m_ref_table_key);
	}
};




template<typename T>
class proxy_ptr<T[]> final
{
	using ref_table_type = internal::ref_table_for_proxy_ptr<T[]>;
	using ref_table_key_type = typename ref_table_type::ref_table_key_type;

public:
	using pointer = typename std::remove_all_extents<T>::type*;
	using element_type = typename std::remove_all_extents<T>::type;

private:
	mutable exclusive_ptr_base<element_type[]>* m_exclusive_ptr;
	mutable pointer m_smart_ptr_end;
	mutable ref_table_key_type m_ref_table_key;

public:
	_CONSTEXPR20_ proxy_ptr() noexcept : m_exclusive_ptr(), m_smart_ptr_end(), m_ref_table_key(ref_table_type::invalid_key_value) {}

	_CONSTEXPR20_ proxy_ptr(const proxy_ptr& other_p) noexcept : m_exclusive_ptr(ref_table_type::__get_ref(other_p.m_ref_table_key)), m_smart_ptr_end(other_p.m_smart_ptr_end), m_ref_table_key(ref_table_type::invalid_key_value)
	{
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = other_p.m_ref_table_key;
		}
	}

	_CONSTEXPR20_ proxy_ptr(proxy_ptr&& rvalue_p) noexcept : m_exclusive_ptr(ref_table_type::__get_ref(rvalue_p.m_ref_table_key)), m_smart_ptr_end(rvalue_p.m_smart_ptr_end), m_ref_table_key(ref_table_type::invalid_key_value)
	{
		if (this->m_exclusive_ptr != nullptr)
		{
			rvalue_p.m_exclusive_ptr = nullptr;
			rvalue_p.m_smart_ptr_end = nullptr;

			this->m_ref_table_key = rvalue_p.m_ref_table_key;
			rvalue_p.m_ref_table_key = ref_table_type::invalid_key_value;
		}
	}

	template<class Allocator>
	_CONSTEXPR20_ proxy_ptr(const FE::exclusive_ptr<T[], Allocator>& exclusive_ptr_p) noexcept : m_exclusive_ptr(ref_table_type::__get_ref(exclusive_ptr_p.get_ref_table_key())), m_smart_ptr_end(), m_ref_table_key(ref_table_type::invalid_key_value)
	{
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = exclusive_ptr_p.get_ref_table_key();
			this->m_smart_ptr_end = exclusive_ptr_p.get() + exclusive_ptr_p.capacity();
		}
	}

	_CONSTEXPR20_ proxy_ptr& operator=(const proxy_ptr& other_p) noexcept
	{
		if (other_p.m_exclusive_ptr == nullptr)
		{
			return *this;
		}

		this->m_exclusive_ptr = ref_table_type::__get_ref(other_p.m_ref_table_key);
		this->m_smart_ptr_end = other_p.m_smart_ptr_end;
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = other_p.m_ref_table_key;
		}
		return *this;
	}

	_CONSTEXPR20_ proxy_ptr& operator=(proxy_ptr&& rvalue_p) noexcept
	{
		if (rvalue_p.m_exclusive_ptr == nullptr)
		{
			return *this;
		}

		this->m_exclusive_ptr = ref_table_type::__get_ref(rvalue_p.m_ref_table_key);
		this->m_smart_ptr_end = rvalue_p.m_smart_ptr_end;
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = rvalue_p.m_ref_table_key;
			rvalue_p.m_ref_table_key = ref_table_type::invalid_key_value;
		}
		return *this;
	}

	template<class Allocator>
	_CONSTEXPR20_ proxy_ptr& operator=(const FE::exclusive_ptr<T[], Allocator>& exclusive_ptr_p) noexcept
	{
		if (!exclusive_ptr_p)
		{
			return *this;
		}

		ref_table_key_type l_retrieved_ref_table_key = exclusive_ptr_p.get_ref_table_key();
		this->m_exclusive_ptr = ref_table_type::__get_ref(l_retrieved_ref_table_key);
		if (this->m_exclusive_ptr != nullptr)
		{
			this->m_ref_table_key = l_retrieved_ref_table_key;
			this->m_smart_ptr_end = this->m_exclusive_ptr->get() + exclusive_ptr_p.capacity();
		}
		return *this;
	}

	_FORCE_INLINE_ void reset() noexcept
	{
		this->m_exclusive_ptr = nullptr;
		this->m_smart_ptr_end = nullptr;
		this->m_ref_table_key = ref_table_type::invalid_key_value;
	}

	_FORCE_INLINE_ ref_table_key_type get_ref_table_key() const noexcept
	{
		return this->m_ref_table_key;
	}

	_FORCE_INLINE_ size_t capacity() const noexcept
	{
		return this->m_smart_ptr_end - this->m_exclusive_ptr;
	}

	_CONSTEXPR20_ void swap(proxy_ptr& in_out_other_p) noexcept
	{
		this->__validate_my_ref();
		in_out_other_p.__validate_my_ref();

		pointer l_temporary_smart_ptr = in_out_other_p.m_exclusive_ptr;
		in_out_other_p.m_exclusive_ptr = this->m_exclusive_ptr;
		this->m_exclusive_ptr = l_temporary_smart_ptr;
		
		pointer l_temporary_smart_ptr_end = in_out_other_p.m_smart_ptr_end;
		in_out_other_p.m_smart_ptr_end = this->m_smart_ptr_end;
		this->m_smart_ptr_end = l_temporary_smart_ptr_end;

		ref_table_key_type l_temporary_key = in_out_other_p.m_ref_table_key;
		in_out_other_p.m_ref_table_key = this->m_ref_table_key;
		this->m_ref_table_key = l_temporary_key;
	}


	_FORCE_INLINE_ boolean is_expired() const noexcept
	{
		this->__validate_my_ref();
		return ((this->m_exclusive_ptr == nullptr) || (this->m_exclusive_ptr->get() == nullptr));
	}

	_FORCE_INLINE_ pointer get_unchecked() const noexcept
	{
		FE_ASSERT(this->m_exclusive_ptr == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ explicit operator bool() const noexcept
	{
		return !(this->is_expired());
	}

	_FORCE_INLINE_ boolean operator!() const noexcept
	{
		return this->is_expired();
	}

	_FORCE_INLINE_ element_type& operator*() const noexcept
	{
		this->__validate_my_ref();
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return *(this->m_exclusive_ptr->get());
	}

	_FORCE_INLINE_ pointer operator->() const noexcept
	{
		this->__validate_my_ref();
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ element_type& operator[](index_t index_p) const noexcept
	{
		this->__validate_my_ref();
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		FE_ASSERT(static_cast<index_t>(this->m_smart_ptr_end - this->m_exclusive_ptr->get()) <= index_p, "${%s@0}: ${%s@1} exceeds the index boundary. ${%s@1} was ${%lu@2}.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE), TO_STRING(index_p), &index_p);
		return this->m_exclusive_ptr->get()[index_p];
	}


	_FORCE_INLINE_ boolean operator==(std::nullptr_t nullptr_p) const noexcept
	{
		this->__validate_my_ref();

		if (this->m_exclusive_ptr == nullptr)
		{
			return true;
		}

		return this->m_exclusive_ptr->get() == nullptr_p;
	}

	_FORCE_INLINE_ boolean operator!=(std::nullptr_t nullptr_p) const noexcept
	{
		this->__validate_my_ref();

		if (this->m_exclusive_ptr != nullptr)
		{
			return this->m_exclusive_ptr->get() != nullptr_p;
		}

		return false;
	}


	_FORCE_INLINE_ boolean operator==(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() == other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator!=(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() != other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator>(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() > other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator>=(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() >= other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator<(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() < other_p.m_exclusive_ptr->get();
	}

	_FORCE_INLINE_ boolean operator<=(const proxy_ptr& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() <= other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator==(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() == other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator!=(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() != other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator>(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() > other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator>=(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() >= other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator<(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() < other_p.m_exclusive_ptr->get();
	}

	template<class Allocator>
	_FORCE_INLINE_ boolean operator<=(const FE::exclusive_ptr<T, Allocator>& other_p) const noexcept
	{
		this->__validate_my_ref();
		other_p.__validate_my_ref();
		return this->m_exclusive_ptr->get() <= other_p.m_exclusive_ptr->get();
	}


	_FORCE_INLINE_ FE::iterator<FE::contiguous_iterator<element_type>> begin() const noexcept
	{
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->operator->();
	}
	_FORCE_INLINE_ FE::iterator<FE::contiguous_iterator<element_type>> end() const noexcept
	{
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->m_smart_ptr_end;
	}
	_FORCE_INLINE_ FE::const_iterator<FE::contiguous_iterator<element_type>> cbegin() const noexcept
	{ 
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->operator->(); 
	}
	_FORCE_INLINE_ FE::const_iterator<FE::contiguous_iterator<element_type>> cend() const noexcept
	{
		return this->m_smart_ptr_end;
	}
	_FORCE_INLINE_ FE::reverse_iterator<FE::contiguous_iterator<element_type>> rbegin() const noexcept
	{
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->operator->();
	}
	_FORCE_INLINE_ FE::reverse_iterator<FE::contiguous_iterator<element_type>> rend() const noexcept
	{
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->m_smart_ptr_end;
	}
	_FORCE_INLINE_ FE::const_reverse_iterator<FE::contiguous_iterator<element_type>> crbegin() const noexcept
	{
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->operator->();
	}
	_FORCE_INLINE_ FE::const_reverse_iterator<FE::contiguous_iterator<element_type>> crend() const noexcept
	{ 
		FE_ASSERT(this->is_expired() == true, "${%s@0}: ${%s@1} is invalid", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_exclusive_ptr));
		return this->m_smart_ptr_end; 
	}

private:
	_FORCE_INLINE_ void __validate_my_ref() const noexcept
	{
		this->m_exclusive_ptr = ref_table_type::__get_ref(this->m_ref_table_key);
	}
};


END_NAMESPACE
#endif