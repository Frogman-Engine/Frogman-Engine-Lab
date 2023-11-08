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
template<class Key, class T, class Hasher = FE::hash<Key>, class KeyEqual = std::equal_to<Key>, class BucketStructure = FE::farray<std::pair<const Key, T>, 64>, class BucketAllocator = FE::std_style::scalable_aligned_allocator<BucketStructure>>
class hash_map
{
public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = std::pair<const Key, T>;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;
	using hasher = Hasher;
	using key_equal = KeyEqual;
	using bucket_allocator_type = BucketAllocator;
	using reference = value_type&;
	using const_reference = const reference;
	using pointer = value_type*;
	using const_pointer = const pointer;
	//using iterator = FE::iterator<FE::hash_map_iterator<>>;
	//using const_iterator = FE::const_iterator<FE::hash_map_iterator<>>;
	using local_iterator = typename BucketStructure::iterator;
	using const_local_iterator = typename BucketStructure::const_iterator;
	using bucket_type = BucketStructure;

	FE_STATIC_ASSERT((std::is_same<value_type, typename BucketStructure::value_type>::value), "BucketStructure::value_type must be the same as value_type");

private:
	std::vector<BucketStructure, BucketAllocator> m_buckets;
	hasher m_hasher;
	key_equal m_key_equal;

public:
	hash_map() noexcept {}
	hash_map(size_type bucket_count_p, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(bucket_count_p), m_hasher(hash_p), m_key_equal(equal_p) {}

	template<class InputIterator>
	hash_map(InputIterator first_p, InputIterator last_p, size_type bucket_count_p, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(bucket_count_p), m_hasher(hash_p), m_key_equal(equal_p) 
	{
		this->__take_contents_from_input_iterator(first_p, last_p);
	}
	
	hash_map(const hash_map& other_p) noexcept : m_buckets(other_p.m_buckets), m_hasher(other_p.m_hasher), m_key_equal(other_p.m_key_equal) {}
	hash_map(hash_map&& rvalue_p) noexcept : m_buckets(std::move(rvalue_p.m_buckets)), m_hasher(std::move(rvalue_p.m_hasher)), m_key_equal(std::move(rvalue_p.m_key_equal)) {}

	hash_map(std::initializer_list<value_type>&& initializer_list_p, size_type bucket_count_p, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(bucket_count_p), m_hasher(hash_p), m_key_equal(equal_p) 
	{
		this->__take_contents_from_input_initializer_list(initializer_list_p);
	}
	
	template<class Container>
	hash_map(container::range<Container>&& range_p, size_type bucket_count_p, const Hasher& hash_p = Hasher(), const key_equal& equal_p = key_equal()) noexcept : m_buckets(bucket_count_p), m_hasher(hash_p), m_key_equal(equal_p)
	{
		this->__take_contents_from_input_range(range_p);
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
	
	}

	void __take_contents_from_input_initializer_list(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
	
	}

	template<class Container>
	void __take_contents_from_range(container::range<Container>&& range_p) noexcept
	{
	
	}
};


END_NAMESPACE
#endif