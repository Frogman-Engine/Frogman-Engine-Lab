#ifndef _FE_CORE_HASH_MAP_HXX_
#define _FE_CORE_HASH_MAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/algorithm/math.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/containers/array.hxx> 
#include <FE/core/hash.hpp>
#include <FE/core/iterator.hxx>
#include <tbb/memory_pool.h>




BEGIN_NAMESPACE(FE)

//https://en.cppreference.com/w/cpp/container/unordered_map

/* 
Containers that support push_back() and pop_back() can be used as the BucketStructure.

List of compatible containers:
	- std::vector
	- std::deque
	- std::list
	- std::stack
	- std::queue
	- std::priority_queue
	- FE::farray
	- FE::fpriority_queue
	- FE::array (not implemented yet)
*/
template<class Key, class T, class Hasher = FE::hash<Key>, class KeyEqual = std::equal_to<Key>, class BucketStructure = FE::farray<FE::pair<Key, T>, 64>, class StatefulBucketAllocator = FE::std_style::scalable_aligned_allocator<BucketStructure>>
class hash_map
{
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
	//using iterator = FE::iterator<FE::hash_map_iterator<>>;
	//using const_iterator = FE::const_iterator<FE::hash_map_iterator<>>;
	using local_iterator = typename BucketStructure::iterator;
	using const_local_iterator = typename BucketStructure::const_iterator;
	using bucket_type = BucketStructure;
	using bucket_container_type = std::vector<BucketStructure, StatefulBucketAllocator>;

	FE_STATIC_ASSERT((std::is_same<value_type, typename BucketStructure::value_type>::value == false), "Static Assertion Failure: BucketStructure::value_type must be the same as value_type");

private:
	bucket_container_type m_buckets;
	hasher m_hasher;
	key_equal m_key_equal;

public:
	hash_map() noexcept {}
	hash_map(size_type bucket_count_p, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(bucket_count_p), m_hasher(hash_p), m_key_equal(equal_p) {}

	template<class InputIterator>
	hash_map(InputIterator first_p, InputIterator last_p, size_type bucket_count_p = 0, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_hasher(hash_p), m_key_equal(equal_p)
	{
		FE_ASSERT(first_p >= last_p, "Assertion Failure: ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(first_p), TO_STRING(last_p));
		
		this->m_buckets.reserve(algorithm::math::select_maximum<size_type>(__to_required_size(last_p - first_p), bucket_count_p));
		this->m_buckets.insert(this->m_buckets.cbegin(), this->m_buckets.capacity(), BucketStructure{});
		this->__take_contents_from_input_iterator(first_p, last_p);
	}
	
	hash_map(const hash_map& other_p) noexcept : m_buckets(other_p.m_buckets), m_hasher(other_p.m_hasher), m_key_equal(other_p.m_key_equal) {}
	hash_map(hash_map&& rvalue_p) noexcept : m_buckets(std::move(rvalue_p.m_buckets)), m_hasher(std::move(rvalue_p.m_hasher)), m_key_equal(std::move(rvalue_p.m_key_equal)) {}

	hash_map(std::initializer_list<value_type>&& initializer_list_p, size_type bucket_count_p = 0, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(algorithm::math::select_maximum<size_type>(__to_required_size(initializer_list_p.size()), bucket_count_p)), m_hasher(hash_p), m_key_equal(equal_p)
	{
		this->__take_contents_from_input_initializer_list(std::move(initializer_list_p));
	}
	
	template<class Container>
	hash_map(container::range<Container>&& range_p, size_type bucket_count_p = 0, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_hasher(hash_p), m_key_equal(equal_p)
	{
		FE_ASSERT(range_p._begin >= range_p._end, "Assertion Failure: Invalid container range detected. ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(range_p._begin), TO_STRING(range_p._end));

		this->m_buckets.reserve(algorithm::math::select_maximum<size_type>(__to_required_size(range_p._end - range_p._begin), bucket_count_p));
		this->m_buckets.insert(this->m_buckets.cbegin(), this->m_buckets.capacity(), BucketStructure{});
		this->__take_contents_from_range<Container>(std::move(range_p));
	}

	~hash_map() noexcept {}

	hash_map& operator=(const hash_map& other_p) noexcept 
	{
		return *this;
	}

	hash_map& operator=(hash_map&& rvalue_p) noexcept 
	{
		return *this;
	}

	hash_map& operator=(std::initializer_list<value_type>&& initializer_list_p) noexcept 
	{
		return *this;
	}
	
private:
	template<class InputIterator>
	void __take_contents_from_input_iterator(InputIterator first_p, InputIterator last_p) noexcept
	{
		FE_STATIC_ASSERT((std::is_same<typename InputIterator::value_type, value_type>::value == false), "Static Assertion Failure: typename InputIterator::value_type must be the same as value_type");
		FE_ASSERT((FE::iterator_cast<typename InputIterator::value_type*, decltype(first_p)>(first_p) == nullptr), "Assertion Failure: ${%s@0} must not be nullptr", TO_STRING(first_p));
		FE_ASSERT(first_p >= last_p, "Assertion Failure: ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(first_p), TO_STRING(last_p));

		while (first_p != last_p)
		{
			this->__add_an_element(std::move(*first_p));
			++first_p;
		}
	}

	void __take_contents_from_input_initializer_list(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		size_t l_input_size = initializer_list_p.size();
		FE_ASSERT(l_input_size == 0, "Assertion Failure: The initializer_list was emtpy, ${%s@0} was zero.", TO_STRING(initializer_list_p.size()));
		
		value_type* l_initializer_list_iterator = const_cast<value_type*>(initializer_list_p.begin());
		value_type* const l_initializer_list_end = l_initializer_list_iterator + l_input_size;

		while (l_initializer_list_iterator != l_initializer_list_end)
		{
			this->__add_an_element(*l_initializer_list_iterator);
			++l_initializer_list_iterator;
		}
	}

	template<class Container>
	void __take_contents_from_range(container::range<Container>&& range_p) noexcept
	{
		FE_ASSERT(range_p._begin >= range_p._end, "Assertion Failure: Invalid container range detected. ${%s@0} must not be equal to, or greater than ${%s@1}", TO_STRING(range_p._begin), TO_STRING(range_p._end));
		
		auto l_range_iterator = range_p._container.get()->begin();
		FE_ASSERT((FE::iterator_cast<value_type*, decltype(l_range_iterator)>(l_range_iterator) == nullptr), "Assertion Failure: Invalid container range detected. The container reference was null.");

		auto l_end_of_range = l_range_iterator + (range_p._end - range_p._begin);

		while (l_range_iterator != l_end_of_range)
		{
			this->__add_an_element(*l_range_iterator);
			++l_range_iterator;
		}
	}

	_FORCE_INLINE_ void __add_an_element(value_type value_p) noexcept
	{
		uint64 l_hash = this->m_hasher(value_p._first);
		uint64 l_bucket_index = l_hash % FE::algorithm::math::to_prime(this->m_buckets.capacity());
		this->m_buckets[l_bucket_index].push_back(std::move(value_p));
	}

	_FORCE_INLINE_ size_type __to_required_size(size_type count_p) noexcept
	{
		return count_p * 2;
	}
};


END_NAMESPACE
#endif