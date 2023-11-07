#ifndef _FE_CORE_ARRAY_HXX_
#define _FE_CORE_ARRAY_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator.hxx>
#include <FE/core/algorithm/utility.hxx>
#include <FE/core/iterator.hxx>
#include <FE/core/memory.hxx>
#include <FE/core/memory_storage.hxx>
#include <FE/core/smart_pointers/smart_ptrs.h>
#include <atomic>
#include <initializer_list>




BEGIN_NAMESPACE(FE)


template <typename T, class Allocator = FE::scalable_aligned_allocator<T>>
class array final
{
	FE_STATIC_ASSERT((std::is_same<T, Allocator::value_type>::value == false), "Static Assertion Failed: The template argument T and Allocator's value_type have be the same type.");
	FE_STATIC_ASSERT(std::is_class<Allocator>::value == false, "Static Assertion Failed: The template argument Allocator is not a class or a struct type.");

public:
	using allocator_type = Allocator;
	using value_type = typename Allocator::value_type;
	using size_type = typename Allocator::size_type;
	using difference_type = var::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const reference;
	using pointer = typename Allocator::pointer;
	using const_pointer = const pointer;
	using iterator = FE::iterator<FE::contiguous_iterator<T>>;
	using const_iterator = FE::const_iterator<FE::contiguous_iterator<T>>;
	using reverse_iterator = FE::reverse_iterator<FE::contiguous_iterator<T>>;
	using const_reverse_iterator = FE::const_reverse_iterator<FE::contiguous_iterator<T>>;

private:
	container_storage<T, Allocator, is_trivial<T>::value> m_smart_storage;

public:
	_CONSTEXPR20_ array() noexcept  {}

	_CONSTEXPR20_ array(size_type count_p, const T& value_p = T()) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(count_p));
		
	}

	template<class InputIterator>
	_CONSTEXPR20_ array(InputIterator first_p, InputIterator last_p) noexcept
	{
		size_t l_input_size = last_p - first_p;
		FE_STATIC_ASSERT(std::is_class<InputIterator>::value == true, "Static Assertion Failed: The template argument InputIterator is not a class or a struct type.");
		FE_ASSERT(l_input_size == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING((last_p - first_p)));

	}

	_CONSTEXPR20_ array(const array& other_p) noexcept 
	{


	}

	_CONSTEXPR20_ array(array&& rvalue_p) noexcept
	{

	}

	_CONSTEXPR20_ array(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		size_type l_initializer_list_size = initializer_list_p.size();
		FE_ASSERT(l_initializer_list_size == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(initializer_list_p.size()));


	}

	_CONSTEXPR20_ ~array() noexcept
	{

	}

	_CONSTEXPR20_ array& operator=(const array& other_p) noexcept
	{

		return *this;
	}

	_CONSTEXPR20_ array& operator=(array&& rvalue_p) noexcept
	{

		return *this;
	}

	_CONSTEXPR20_ array& operator=(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		size_type l_initializer_list_size = initializer_list_p.size();

		FE_ASSERT(l_initializer_list_size == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(initializer_list_p.size()));

		return *this;
	}

	_CONSTEXPR20_ void assign(size_type count_p, const T& value_p) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(count_p));
		FE_ASSERT(count_p > this->m_smart_array.capacity(), "${%s@0}: ${%s@1} cannot be greater than the array capacity.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(count_p));

	}

	template<class InputIterator>
	_CONSTEXPR20_ void assign(InputIterator first_p, InputIterator last_p) noexcept
	{
		size_type l_input_size = last_p - first_p;
		FE_ASSERT(first_p == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(dest_pointer_p));
		FE_ASSERT(last_p == nullptr, "${%s@0}: ${%s@1} is nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(source_pointer_p));
		FE_ASSERT(first_p > last_p, "${%s@0}: The begin iterator ${%s@1} must be pointing at the first element of a container.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ILLEGAL_POSITION), TO_STRING(dest_first_p));
		FE_ASSERT(l_input_size == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(l_input_size));
		FE_ASSERT(l_input_size > this->m_smart_array.capacity(), "${%s@0}: ${%s@1} cannot be greater than the array capacity.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(l_input_size));

	}

	_CONSTEXPR20_ void assign(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		FE_ASSERT(initializer_list_p.size() == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(initializer_list_p.size()));
		FE_ASSERT(initializer_list_p.size() > this->m_smart_array.capacity(), "${%s@0}: ${%s@1} cannot be greater than the array capacity.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(initializer_list_p.size()));

		this->operator=(std::move(initializer_list_p));
	}

	template<class Container>
	_CONSTEXPR23_ void assign_range(container::range<Container>&& range_p) noexcept
	{
		FE_STATIC_ASSERT(std::is_class<Container>::value == false, "Static Assertion Failed: The template argument Container must be a class or a struct type.");
		FE_ASSERT(range_p._container.is_valid() == false, "${%s@0}: The container refernce is invalid.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		size_type l_input_size = range_p._end - range_p._begin;
		FE_ASSERT(l_input_size > this->m_smart_array.capacity(), "${%s@0}: ${%s@1} cannot be greater than the array capacity.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(l_input_size));
		FE_ASSERT(range_p._begin > range_p._end, "${%s@0}: The begin index cannot be greater than the end index.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ILLEGAL_POSITION));


	}


	_NODISCARD_ _FORCE_INLINE_ container::range<FE::array<value_type>> make_range(index_t begin_p, index_t end_p) noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return container::range<FE::array<value_type>>{ *this, begin_p, end_p };
	}


	_NODISCARD_ _FORCE_INLINE_ operator proxy_ptr<value_type[]>() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return FE::proxy_ptr<value_type[]>(this->m_smart_array);
	}


	_NODISCARD_ _FORCE_INLINE_ reference operator[](index_t position_p) const noexcept
	{
		FE_ASSERT(this->m_smart_array == nullptr, "${%s@0}: FE::array is null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		FE_ASSERT(position_p >= this->size(), "${%s@0}: Unable to access an element that does not belong to this container. ${%s@1} was ${%lu@2}", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE), TO_STRING(position_p), &position_p);
		
		T l_faux;
		return l_faux;
	}

	_NODISCARD_ _FORCE_INLINE_ reference front() const noexcept
	{
		FE_ASSERT(this->m_smart_array == nullptr, "${%s@0}: FE::array is null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		T l_faux;
		return l_faux;
	}

	_NODISCARD_ _FORCE_INLINE_ reference back() const noexcept
	{
		FE_ASSERT(this->m_smart_array == nullptr, "${%s@0}: FE::array is null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		T l_faux;
		return l_faux;
	}

	_NODISCARD_ _FORCE_INLINE_ pointer data() const noexcept
	{
		FE_ASSERT(this->m_smart_array == nullptr, "${%s@0}: FE::array is null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		return this->m_smart_array.get();
	}


	_NODISCARD_ _FORCE_INLINE_ iterator begin() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return iterator{ this->m_smart_array.get() };
	}
	_NODISCARD_ _FORCE_INLINE_ iterator end() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return iterator{ this->m_smart_array.get() + this->m_last_index };
	}

	_NODISCARD_ _FORCE_INLINE_ const_iterator cbegin() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return const_iterator{ this->m_smart_array.get() };
	}
	_NODISCARD_ _FORCE_INLINE_ const_iterator cend() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return const_iterator{ this->m_smart_array.get() + this->m_last_index };
	}

	_NODISCARD_ _FORCE_INLINE_ reverse_iterator rbegin() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return reverse_iterator{ (this->m_smart_array.get() + this->m_last_index) - 1 };
	}
	_NODISCARD_ _FORCE_INLINE_ reverse_iterator rend() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return reverse_iterator{ this->m_smart_array.get() - 1 };
	}

	_NODISCARD_ _FORCE_INLINE_ const_reverse_iterator crbegin() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return const_reverse_iterator{ (this->m_smart_array.get() + this->m_last_index) - 1 };
	}
	_NODISCARD_ _FORCE_INLINE_ const_reverse_iterator crend() const noexcept
	{
		FE_ASSERT(this->m_smart_array.get() == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));
		return const_reverse_iterator{ this->m_smart_array.get() - 1 };
	}


	_NODISCARD_ _FORCE_INLINE_ boolean is_empty() const noexcept { return this->m_array_size == 0; }
	_NODISCARD_ _FORCE_INLINE_ size_type size() const noexcept { return this->m_array_size; }
	_NODISCARD_ _FORCE_INLINE_ size_type max_size() const noexcept { return FE::max_value<size_type>; }

	_CONSTEXPR20_ void reserve(const size_type new_capacity_p) noexcept
	{
		FE_ASSERT(new_capacity_p == 0, "${%s@0}: Unable to extend(). ${%s@1} was zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(new_capacity_p));
	
	}

	_CONSTEXPR20_ void extend(const size_type extra_capacity_p) noexcept
	{
		FE_ASSERT(extra_capacity_p == 0, "${%s@0}: Unable to extend(). ${%s@1} was zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(extra_capacity_p));
	}

	_NODISCARD_ _FORCE_INLINE_ size_type capacity() const noexcept { return this->m_smart_array.capacity(); }

	_CONSTEXPR20_ void shrink_to_fit() noexcept
	{
		FE_ASSERT(this->m_array_size == 0, "${%s@0}: Unable to shrink_to_fit() an empty string instance.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE));
		FE_ASSERT(this->m_smart_array == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array));

	}


	_CONSTEXPR20_ void clear() noexcept
	{
		FE_ASSERT(this->m_smart_array == nullptr, "${%s@0}: ${%s@1} was nullptr.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_smart_array.get()));

	}

	template<typename... Arguments >
	_CONSTEXPR20_ iterator emplace(index_t position_p, Arguments&&... arguments_p) noexcept
	{
		FE_ASSERT(position_p >= this->m_smart_array.capacity(), "${%s@0}: ${%s@1} exceeds the array capacity. ${%s@1} was ${%lu@2}, and ${%s@3} was ${%lu@4}", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(position_p), &position_p, TO_STRING(this->m_smart_array.capacity()), &FE::buffer<size_type>::set_and_get(this->m_smart_array.capacity()));

	}

	_CONSTEXPR20_ iterator erase(index_t position_p, size_type count_p = 1) noexcept
	{
		FE_ASSERT(position_p + count_p >= this->m_smart_array.capacity(), "${%s@0}: ${%s@1} exceeds the array capacity. ${%s@1} was ${%lu@2}, and ${%s@3} was ${%lu@4}", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(position_p + count_p), &FE::buffer<size_type>::set_and_get(position_p + count_p), TO_STRING(this->m_smart_array.capacity()), &FE::buffer<size_type>::set_and_get(this->m_smart_array.capacity()));
		FE_ASSERT(count_p == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(count_p));
		FE_ASSERT(position_p + count_p >= this->m_last_index, "Assertion Failure: It is not allowed to erase() the back of an array. Use pop_back() instead.");


	}

	template<typename... Arguments >
	_CONSTEXPR20_ iterator emplace_back(Arguments&&... arguments_p) noexcept
	{
		FE_ASSERT(this->m_last_index > this->m_smart_array.capacity(), "${%s@0}: Unable to pop the back of an empty array.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		
	
	}

	template<class Container>
	_CONSTEXPR23_ void append_range(container::range<Container>&& range_p) noexcept
	{
		FE_STATIC_ASSERT(std::is_class<Container>::value == false, "Static Assertion Failed: The template argument Container must be a class or a struct type.");
		FE_ASSERT(range_p._container.is_valid() == false, "${%s@0}: The container refernce is invalid.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		size_type l_input_size = range_p._end - range_p._begin;
		FE_ASSERT(l_input_size > this->m_smart_array.capacity(), "${%s@0}: ${%s@1} cannot be greater than the array capacity.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_CAPACITY), TO_STRING(l_input_size));
		FE_ASSERT(range_p._begin > range_p._end, "${%s@0}: The begin index cannot be greater than the end index.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ILLEGAL_POSITION));

	}

	_CONSTEXPR23_ value_type pop_back() noexcept
	{
		FE_ASSERT(this->m_array_size == 0, "${%s@0}: Unable to pop the back of an empty array.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));

	}

	_CONSTEXPR20_ void resize(size_type count_p) noexcept
	{
		FE_ASSERT(count_p == 0, "${%s@0}: ${%s@1} cannot be zero.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(count_p));
		
		
	}

	_CONSTEXPR20_ void swap(array& in_out_other_p) noexcept
	{
		algorithm::utility::swap(*this, in_out_other_p);
	}

	_NODISCARD_ _FORCE_INLINE_ boolean operator==(const array& other_p) const noexcept
	{
		return FE::memcmp(this->cbegin(), this->cend(), other_p.cbegin(), other_p.cend());
	}

	_NODISCARD_ _FORCE_INLINE_ boolean operator!=(const array& other_p) const noexcept
	{
		return !FE::memcmp(this->cbegin(), this->cend(), other_p.cbegin(), other_p.cend());
	}
};




END_NAMESPACE
#endif 