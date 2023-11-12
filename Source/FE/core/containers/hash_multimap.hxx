#ifndef _FE_CORE_HASH_MULTIMAP_HXX_
#define _FE_CORE_HASH_MULTIMAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/algorithm/math.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/containers/array.hxx> 
#include <FE/core/hash.hpp>
#include <FE/core/iterator.hxx>
#include <tbb/memory_pool.h>




BEGIN_NAMESPACE(FE)


template<class Key, class T, class Hasher, class KeyEqual, class BucketStructure, class StatefulBucketAllocator>
class hash_multimap;


template<class Key, class T, class Hasher, class KeyEqual, class BucketStructure, class StatefulBucketAllocator>
struct hash_map_iterator
{
	using category = hash_map_iterator<Key, typename FE::remove_const_reference<T>::type, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>;
	using value_type = FE::pair<Key, T>;
	using difference_type = var::ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const pointer;
	using const_reference = const reference;
	using index_type = Key;

	using key_type = Key;
	using mapped_type = T;
	using hash_map_type = hash_multimap<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>;

protected:
	hash_map_type* m_bucket_pointer;
	var::index_t m_current_bucket_index;
	pointer m_local_iterator;

public:
	_FORCE_INLINE_ hash_map_iterator() noexcept : m_bucket_pointer(), m_current_bucket_index(), m_local_iterator() {}
	_FORCE_INLINE_ hash_map_iterator(hash_map_type& value_p) noexcept : m_bucket_pointer(&value_p), m_current_bucket_index(), m_local_iterator(value_p.m_buckets.front().begin().operator->()) {}
	_FORCE_INLINE_ hash_map_iterator(hash_map_type& value_p, index_t current_bucket_index_p, pointer local_iterator_p) noexcept : m_bucket_pointer(&value_p), m_current_bucket_index(current_bucket_index_p), m_local_iterator(local_iterator_p) {}
	_FORCE_INLINE_ hash_map_iterator(const hash_map_iterator& other_p) noexcept : m_bucket_pointer(other_p.m_bucket_pointer), m_current_bucket_index(other_p.m_current_bucket_index), m_local_iterator(other_p.m_local_iterator) {}
	_FORCE_INLINE_ hash_map_iterator(hash_map_iterator&& rvalue_p) noexcept : m_bucket_pointer(rvalue_p.m_bucket_pointer), m_current_bucket_index(rvalue_p.m_current_bucket_index), m_local_iterator(rvalue_p.m_local_iterator)
	{
		rvalue_p.m_bucket_pointer = nullptr;
		rvalue_p.m_current_bucket_index = 0;
		rvalue_p.m_local_iterator = nullptr;
	}
	_FORCE_INLINE_ hash_map_iterator(const pointer value_p) noexcept : m_bucket_pointer(), m_current_bucket_index(), m_local_iterator(value_p) {}
	_FORCE_INLINE_ ~hash_map_iterator() noexcept {}

	_FORCE_INLINE_ boolean is_null() const noexcept { return ((this->m_bucket_pointer == nullptr) || (this->m_bucket_pointer->is_empty() == true)); }

	_FORCE_INLINE_ reference operator*() const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: Unable to dereference a null iterator.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return *(this->m_local_iterator);
	}

	_FORCE_INLINE_ pointer operator->() const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: Unable to access a null iterator.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator;
	}


	void operator++() noexcept
	{
		FE_CHECK((this->is_null() == true), "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		++(this->m_local_iterator);

		if (this->m_bucket_pointer->m_buckets[this->m_current_bucket_index].cend().operator->() == this->m_local_iterator)
		{
			FE_CHECK((this->m_current_bucket_index == this->m_bucket_pointer->m_buckets.size()), "${%s@0}: The iterator out of range.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));
			
			++(this->m_current_bucket_index);
			this->m_local_iterator = this->m_bucket_pointer->m_buckets[this->m_current_bucket_index].begin().operator->();

			while ((this->m_local_iterator == nullptr) && (this->m_current_bucket_index < this->m_bucket_pointer->m_buckets.size()))
			{
				++(this->m_current_bucket_index);
				this->m_local_iterator = this->m_bucket_pointer->m_buckets[this->m_current_bucket_index].begin().operator->();
			}
		}
	}

	void operator--() noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		--(this->m_local_iterator);

		if (this->m_bucket_pointer->m_buckets[this->m_current_bucket_index].crend() == this->m_local_iterator)
		{
			FE_CHECK(this->m_current_bucket_index == 0, "${%s@0}: The iterator out of range.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));

			--(this->m_current_bucket_index);
			this->m_local_iterator = this->m_bucket_pointer->m_buckets[this->m_current_bucket_index].crbegin().operator->();

			while ((this->m_local_iterator == nullptr) && (this->m_current_bucket_index > 0))
			{
				--(this->m_current_bucket_index);
				this->m_local_iterator = this->m_bucket_pointer->m_buckets[this->m_current_bucket_index].crbegin().operator->();
			}
		}
	}


	_FORCE_INLINE_ pointer operator+(const difference_type pointer_offset_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		
		hash_map_iterator l_result = *this;
		for (difference_type i = 0; i < pointer_offset_p; ++i)
		{
			++l_result;
		}
		return l_result.m_local_iterator;
	}

	_FORCE_INLINE_ void operator+=(const difference_type pointer_offset_p) noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));

		for (difference_type i = 0; i < pointer_offset_p; ++i)
		{
			this->operator++();
		}
	}


	_FORCE_INLINE_ pointer operator-(const difference_type pointer_offset_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		
		hash_map_iterator l_result = *this;
		for (difference_type i = 0; i < pointer_offset_p; ++i)
		{
			--l_result;
		}
		return l_result.m_local_iterator;
	}

	_FORCE_INLINE_ void operator-=(const difference_type pointer_offset_p) noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		
		for (difference_type i = 0; i < pointer_offset_p; ++i)
		{
			this->operator--();
		}
	}


	_FORCE_INLINE_ difference_type operator-(const hash_map_iterator& value_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator - value_p.m_local_iterator;
	}


	_FORCE_INLINE_ reference operator[](const index_type index_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		FE_CHECK(index_p == FE::max_value<index_t>, "${%s@0}: negative integers cannot be assigned to an index_t variable.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_OUT_OF_RANGE));
		
		return this->m_bucket_pointer->m_buckets[index_p];
	}


	_FORCE_INLINE_ hash_map_iterator& operator=(const hash_map_iterator& other_p) noexcept
	{
		this->m_bucket_pointer = other_p.m_bucket_pointer;
		this->m_current_bucket_index = other_p.m_current_bucket_index;
		this->m_local_iterator = other_p.m_local_iterator;
		return *this;
	}

	_FORCE_INLINE_ hash_map_iterator& operator=(hash_map_iterator&& rvalue_p) noexcept
	{
		this->m_bucket_pointer = rvalue_p.m_bucket_pointer;
		this->m_current_bucket_index = rvalue_p.m_current_bucket_index;
		this->m_local_iterator = rvalue_p.m_local_iterator;
		return *this;
	}


	_FORCE_INLINE_ boolean operator<(const hash_map_iterator& other_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator < other_p.m_local_iterator;
	}

	_FORCE_INLINE_ boolean operator<=(const hash_map_iterator& other_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator <= other_p.m_local_iterator;
	}

	_FORCE_INLINE_ boolean operator>(const hash_map_iterator& other_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator > other_p.m_local_iterator;
	}

	_FORCE_INLINE_ boolean operator>=(const hash_map_iterator& other_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator >= other_p.m_local_iterator;
	}

	_FORCE_INLINE_ boolean operator==(const hash_map_iterator& other_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator == other_p.m_local_iterator;
	}

	_FORCE_INLINE_ boolean operator!=(const hash_map_iterator& other_p) const noexcept
	{
		FE_CHECK(this->is_null() == true, "${%s@0}: The iterator was null.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR));
		return this->m_local_iterator != other_p.m_local_iterator;
	}
};



/* 
https://en.cppreference.com/w/cpp/container/unordered_multimap
// replace BucketStructure with memory pooled std list
*/
template<class Key, class T, class Hasher = FE::hash<Key>, class KeyEqual = std::equal_to<Key>, class BucketStructure = FE::farray<FE::pair<Key, T>, 128>, class StatefulBucketAllocator = FE::std_style::scalable_aligned_allocator<BucketStructure>>
class hash_multimap
{
	friend struct hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>;

public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = FE::pair<Key, T>;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;
	using hasher = Hasher;
	using key_equal = KeyEqual;
	using bucket_allocator_type = StatefulBucketAllocator;
	using reference = value_type&;
	using const_reference = const reference;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using iterator = FE::iterator<hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>>;
	using const_iterator = FE::const_iterator<hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>>;
	using local_iterator = typename BucketStructure::iterator;
	using const_local_iterator = typename BucketStructure::const_iterator;
	using bucket_type = BucketStructure;
	using bucket_container_type = std::vector<BucketStructure, StatefulBucketAllocator>;

	FE_STATIC_CHECK((std::is_same<value_type, typename BucketStructure::value_type>::value == false), "Static Assertion Failure: BucketStructure::value_type must be the same as value_type");

	static mapped_type s_null;

private:
	bucket_container_type m_buckets;
	hasher m_hasher;
	key_equal m_key_equal;

public:
	_FORCE_INLINE_ hash_multimap() noexcept {}
	hash_multimap(size_type bucket_count_p, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(bucket_count_p), m_hasher(hash_p), m_key_equal(equal_p) {}

	template<class InputIterator>
	hash_multimap(InputIterator first_p, InputIterator last_p, size_type bucket_count_p = 0, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_hasher(hash_p), m_key_equal(equal_p)
	{
		FE_CHECK(first_p >= last_p, "Assertion Failure: ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(first_p), TO_STRING(last_p));

		this->m_buckets.reserve(algorithm::math::select_maximum<size_type>(__to_required_size(last_p - first_p), bucket_count_p));
		this->m_buckets.insert(this->m_buckets.cbegin(), this->m_buckets.capacity(), BucketStructure{});
		this->__take_contents_from_input_iterator(first_p, last_p);
	}

	hash_multimap(const hash_multimap& other_p) noexcept : m_buckets(other_p.m_buckets), m_hasher(other_p.m_hasher), m_key_equal(other_p.m_key_equal) {}
	hash_multimap(hash_multimap&& rvalue_p) noexcept : m_buckets(std::move(rvalue_p.m_buckets)), m_hasher(std::move(rvalue_p.m_hasher)), m_key_equal(std::move(rvalue_p.m_key_equal)) {}

	hash_multimap(std::initializer_list<value_type>&& initializer_list_p, size_type bucket_count_p = 0, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(algorithm::math::select_maximum<size_type>(__to_required_size(initializer_list_p.size()), bucket_count_p)), m_hasher(hash_p), m_key_equal(equal_p)
	{
		this->__take_contents_from_input_initializer_list(std::move(initializer_list_p));
	}

	template<class Container>
	hash_multimap(container::range<Container>&& range_p, size_type bucket_count_p = 0, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_hasher(hash_p), m_key_equal(equal_p)
	{
		FE_CHECK(range_p._begin >= range_p._end, "Assertion Failure: Invalid container range detected. ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(range_p._begin), TO_STRING(range_p._end));

		this->m_buckets.reserve(algorithm::math::select_maximum<size_type>(__to_required_size(range_p._end - range_p._begin), bucket_count_p));
		this->m_buckets.insert(this->m_buckets.cbegin(), this->m_buckets.capacity(), BucketStructure{});
		this->__take_contents_from_range<Container>(std::move(range_p));
	}

	_FORCE_INLINE_ ~hash_multimap() noexcept {}

	hash_multimap& operator=(const hash_multimap& other_p) noexcept
	{
		this->m_buckets = other_p.m_buckets;
		this->m_hasher = other_p.m_hasher;
		this->m_key_equal = other_p.m_key_equal;
		return *this;
	}

	hash_multimap& operator=(hash_multimap&& rvalue_p) noexcept
	{
		this->m_buckets = std::move(rvalue_p.m_buckets);
		this->m_hasher = std::move(rvalue_p.m_hasher);
		this->m_key_equal = std::move(rvalue_p.m_key_equal);
		return *this;
	}

	hash_multimap& operator=(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		auto l_cbegin = this->m_buckets.cbegin();
		auto l_cend = this->m_buckets.cend();
		if (l_cbegin != l_cend)
		{
			this->m_buckets.erase(l_cbegin, l_cend);
		}
		this->m_buckets.reserve(__to_required_size(initializer_list_p.size()));
		this->m_buckets.insert(this->m_buckets.cbegin(), this->m_buckets.capacity(), BucketStructure{});
		this->__take_contents_from_input_initializer_list(std::move(initializer_list_p));
		return *this;
	}
	
	_FORCE_INLINE_ bucket_container_type get_allocator() const noexcept { return this->m_buckets.get_allocator(); }


	_FORCE_INLINE_ iterator begin() const noexcept 
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to create an iterater to an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		return hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>{ *const_cast<hash_multimap*>(this) };
	}
	_FORCE_INLINE_ const_iterator cbegin() const noexcept 
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to create a const_iterater to an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		return hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>{ *const_cast<hash_multimap*>(this) };
	}
	_FORCE_INLINE_ iterator end() const noexcept 
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to create an iterater to an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		BucketStructure& l_end = const_cast<BucketStructure&>(this->m_buckets.back());
		return hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>{ *const_cast<hash_multimap*>(this), this->m_buckets.size(), l_end.begin().operator->() + l_end.size() };
	}
	_FORCE_INLINE_ const_iterator cend() const noexcept 
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to create a const_iterater to an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		BucketStructure& l_end = const_cast<BucketStructure&>(this->m_buckets.back());
		return hash_map_iterator<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>{ *const_cast<hash_multimap*>(this), this->m_buckets.size(), l_end.begin().operator->() + l_end.size() };
	}


	_FORCE_INLINE_ _NODISCARD_ boolean is_empty() const noexcept { return this->m_buckets.empty(); }
	_FORCE_INLINE_ size_type size() const noexcept 
	{
		var::count_t l_sum = 0;
		for (auto bucket : this->m_buckets)
		{
			l_sum += bucket.size();
		}

		return l_sum;
	}

	_FORCE_INLINE_ size_type max_size() const noexcept { return FE::max_value<size_type> / sizeof(value_type); }


	_FORCE_INLINE_ void clear() noexcept { this->m_buckets.clear(); }

	iterator insert(const value_type& value_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to insert a value into an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		return this->__hash_an_element_and_append(value_p);
	}

	iterator insert(value_type&& value_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to insert a value into an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		return this->__hash_an_element_and_append(value_p);
	}

	template<class InputIterator>
	void insert(InputIterator first_p, InputIterator last_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to insert a value into an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		this->__take_contents_from_input_iterator(first_p, last_p);
	}

	void insert(std::initializer_list<value_type> initializer_list_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to insert a value into an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		this->__take_contents_from_input_initializer_list(std::move(initializer_list_p));
	}

	template<class Container>
	void insert_range(container::range<Container>&& range_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to insert a value into an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		this->__take_contents_from_range<Container>(std::move(range_p));
	}

	template<typename... Arguments>
	iterator emplace(Arguments&&... arguments_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Unable to insert a value into an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		return this->__hash_an_element_and_append(arguments_p...);
	}


	//iterator erase(iterator position_p) noexcept
	//{

	//}
	//iterator erase(const_iterator position_p) noexcept
	//{

	//}
	//iterator erase(const_iterator first_p, const_iterator last_p) noexcept
	//{

	//}
	//size_type erase(const Key& key_p) noexcept
	//{

	//}

	void swap(hash_multimap& in_out_other_p) noexcept
	{
		algorithm::utility::swap(this->m_buckets, in_out_other_p.m_buckets);
		algorithm::utility::swap(this->m_hasher, in_out_other_p.m_hasher);
		algorithm::utility::swap(this->m_key_equal, in_out_other_p.m_key_equal);
	}
	
	template<class OtherHasher, class OtherKeyEqual >
	void merge(FE::hash_multimap<Key, T, OtherHasher, OtherKeyEqual, StatefulBucketAllocator>& source_p) noexcept
	{
		FE_CHECK(source_p.is_empty() == true, "${%s@0}: Unable to merge an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		this->__rehash_all();
		for (auto value : source_p)
		{
			this->__hash_an_element_and_append(value);
		}
	}

	template<class OtherHasher, class OtherKeyEqual >
	void merge(FE::hash_multimap<Key, T, OtherHasher, OtherKeyEqual, StatefulBucketAllocator>&& source_p) noexcept
	{
		FE_CHECK(source_p.is_empty() == true, "${%s@0}: Unable to merge an empty FE::hash_multimap.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		this->__rehash_all();
		for (auto value : source_p)
		{
			this->__hash_an_element_and_append(std::move(value));
		}
	}


	mapped_type& operator[](const key_type& key_p) noexcept
	{
		FE_CHECK(this->is_empty() == true, "${%s@0}: Can't access an empty FE::hash_multimap.",TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));
		
		uint64 l_bucket_index = this->__hash_object(key_p);

		for (auto value : this->m_buckets[l_bucket_index])
		{
			if (this->m_key_equal(value._first, key_p) == true)
			{
				return value._second;
			}
		}
		
		FE_EXIT(true, INPUT_ERROR_2XX::_FATAL_ERROR_INVALID_KEY, "The hash map key does not exists.");
		return s_null;
	}


	_FORCE_INLINE_ void reserve(size_type new_capacity_p) noexcept
	{
		if (new_capacity_p > this->m_buckets.capacity())
		{
			auto l_new_begin = this->m_buckets.end();
			this->m_buckets.reserve(new_capacity_p);
			this->m_buckets.insert(l_new_begin, new_capacity_p, BucketStructure{});
		}
	}

	_FORCE_INLINE_ void extend(size_type extra_capacity_p) noexcept
	{
		auto l_new_begin = this->m_buckets.end();
		this->m_buckets.reserve(extra_capacity_p + this->m_buckets.capacity());
		this->m_buckets.insert(l_new_begin, extra_capacity_p, BucketStructure{});
	}

	_FORCE_INLINE_ void shrink_to_fit() noexcept
	{
		this->m_buckets.shrink_to_fit();
	}

private:

	template<class InputIterator>
	_FORCE_INLINE_ void __take_contents_from_input_iterator(InputIterator first_p, InputIterator last_p) noexcept
	{
		FE_STATIC_CHECK((std::is_same<typename InputIterator::value_type, value_type>::value == false), "Static Assertion Failure: typename InputIterator::value_type must be the same as value_type");
		FE_CHECK((FE::iterator_cast<typename InputIterator::value_type*, decltype(first_p)>(first_p) == nullptr), "Assertion Failure: ${%s@0} must not be nullptr", TO_STRING(first_p));
		FE_CHECK(first_p >= last_p, "Assertion Failure: ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(first_p), TO_STRING(last_p));

		while (first_p != last_p)
		{
			this->__hash_an_element_and_append(*first_p);
			++first_p;
		}
	}

	_FORCE_INLINE_ void __take_contents_from_input_initializer_list(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		size_t l_input_size = initializer_list_p.size();
		FE_CHECK(l_input_size == 0, "Assertion Failure: The initializer_list was emtpy, ${%s@0} was zero.", TO_STRING(initializer_list_p.size()));
		
		value_type* l_initializer_list_iterator = const_cast<value_type*>(initializer_list_p.begin());
		value_type* const l_initializer_list_end = l_initializer_list_iterator + l_input_size;

		while (l_initializer_list_iterator != l_initializer_list_end)
		{
			this->__hash_an_element_and_append(std::move(*l_initializer_list_iterator));
			++l_initializer_list_iterator;
		}
	}

	template<class Container>
	_FORCE_INLINE_ void __take_contents_from_range(container::range<Container>&& range_p) noexcept
	{
		FE_CHECK(range_p._begin >= range_p._end, "Assertion Failure: Invalid container range detected. ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(range_p._begin), TO_STRING(range_p._end));
		
		auto l_range_iterator = range_p._container.get().begin();
		FE_CHECK((FE::iterator_cast<value_type*, decltype(l_range_iterator)>(l_range_iterator) == nullptr), "Assertion Failure: Invalid container range detected. The container reference was null.");

		auto l_end_of_range = l_range_iterator + (range_p._end - range_p._begin);

		while (l_range_iterator != l_end_of_range)
		{
			this->__hash_an_element_and_append(*l_range_iterator);
			++l_range_iterator;
		}
	}

	_FORCE_INLINE_ iterator __hash_an_element_and_append(value_type value_p) noexcept
	{
		var::uint64 l_bucket_index = this->__hash_object(value_p._first);

		if (this->m_buckets[l_bucket_index].size() == this->m_buckets[l_bucket_index].capacity())
		{
			this->__rehash_all();
			l_bucket_index = this->__hash_object(value_p._first);
		}
		
		this->m_buckets[l_bucket_index].push_back(std::forward<value_type&>(value_p));
		return typename iterator::base_type{ *this, l_bucket_index, (this->m_buckets[l_bucket_index].begin() + this->m_buckets[l_bucket_index].size()).operator->() };
	}

	_FORCE_INLINE_ size_type __to_required_size(const size_type count_p) noexcept
	{
		return count_p * 3;
	}

	_FORCE_INLINE_ uint64 __hash_object(const key_type& key_p) noexcept
	{
		uint64 l_hash = this->m_hasher(key_p);
		return l_hash % this->m_buckets.capacity();
	}

	_FORCE_INLINE_ void __rehash_all() noexcept
	{
		hash_multimap l_new_buckets;
		l_new_buckets.m_buckets.reserve(this->m_buckets.capacity() * 3);
		l_new_buckets.m_buckets.insert(l_new_buckets.m_buckets.cbegin(), l_new_buckets.m_buckets.capacity(), BucketStructure{});

		for (auto previous_bucket : this->m_buckets)
		{
			for (auto value : previous_bucket)
			{
				l_new_buckets.__hash_an_element_and_append(std::move(value));
			}
		}

		*this = std::move(l_new_buckets);
	}
};


template<class Key, class T, class Hasher, class KeyEqual, class BucketStructure, class StatefulBucketAllocator>
hash_multimap<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>::mapped_type hash_multimap<Key, T, Hasher, KeyEqual, BucketStructure, StatefulBucketAllocator>::s_null;


END_NAMESPACE
#endif