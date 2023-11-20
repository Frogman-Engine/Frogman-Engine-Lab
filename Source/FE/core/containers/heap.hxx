#ifndef _FE_CORE_HEAP_HXX_
#define _FE_CORE_HEAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#ifndef TBB_PREVIEW_MEMORY_POOL
#define TBB_PREVIEW_MEMORY_POOL
#endif
#include <tbb/memory_pool.h>
#include <memory>




BEGIN_NAMESPACE(FE)


namespace internal::heap
{
	enum struct SHOUD_NODES_BE_POOLED
	{
		_POSITIVE,
		_NEGATIVE
	};

	constexpr SHOUD_NODES_BE_POOLED evaluate_type_size(size_t size_in_bytes_p) noexcept
	{
		if (size_in_bytes_p > 1024)
		{
			return SHOUD_NODES_BE_POOLED::_NEGATIVE;
		}
		
		return SHOUD_NODES_BE_POOLED::_POSITIVE;
	}

	template<typename T>
	struct tbb_memory_pool_deleter
	{
		FE::ref<tbb::memory_pool_allocator<T>> m_manager;
		
		_FORCE_INLINE_ void operator()(void* ptr_p) noexcept
		{
			if (ptr_p == nullptr || this->m_manager.is_null() == true) { return; }

			this->m_manager->deallocate(static_cast<T*>(ptr_p), _NULL_);
		}
	};

	template<class T>
	struct node
	{
		T _data;
		FE::ref<node> _parent;
		std::unique_ptr<node, tbb_memory_pool_deleter<T>> _left;
		std::unique_ptr<node, tbb_memory_pool_deleter<T>> _right;
	};
}


enum struct ORDER : uint8
{
	_PRE_ORDER = 0,
	_IN_ORDER = 1,
	_POST_ORDER = 2,
};


// The template argument Allocator is ignored and replaced with FE::engine_style::tbb_memory_pool_allocator<FE::std_style::scalable_aligned_allocator<T>> if ShouldNodesBePooled is _POSITIVE. Otherwise Allocator is used as is.
template<class T, class Comparator = std::less<T>, class Allocator = FE::scalable_aligned_allocator<T>, internal::heap::SHOUD_NODES_BE_POOLED ShouldNodesBePooled = internal::heap::evaluate_type_size(sizeof(T))>
class heap;


template<class T, class Comparator, class Allocator>
class heap<T, Comparator, Allocator, internal::heap::SHOUD_NODES_BE_POOLED::_POSITIVE>
{
public:
	using key_type = T;
	using value_type = T;
	using size_type = var::size_t;
	using difference_type = var::ptrdiff_t;
	using key_comparator = Comparator;
	using reference = value_type&;
	using const_reference = const reference;
	using pointer = value_type*;
	using const_pointer = const pointer;
	//using iterator = FE::iterator<FE::heap_iterator<>>;
	//using const_iterator = FE::const_iterator<FE::heap_iterator<>>;
	//using reverse_iterator = FE::reverse_iterator<FE::heap_iterator<>>;
	//using const_reverse_iterator = FE::const_reverse_iterator<FE::heap_iterator<>>;

private:
	tbb::memory_pool<FE::scalable_aligned_allocator<T>> m_pool;
	tbb::memory_pool_allocator<T> m_allocator;
	std::unique_ptr<internal::heap::node<T>, internal::heap::tbb_memory_pool_deleter<T>>  m_root;
	key_comparator m_comparator;

public:
	heap() noexcept : m_pool(), m_allocator(m_pool), m_root(), m_comparator() {}
	heap(const key_comparator& comparator_p) noexcept : m_pool(), m_allocator(m_pool), m_root(), m_comparator(comparator_p) {}
	
	template<class InputIterator>
	heap(InputIterator first_p, InputIterator last_p, const key_comparator& comparator_p = Comparator()) : m_pool(), m_allocator(m_pool), m_root(), m_comparator(comparator_p) 
	{
	}

	heap(const heap& other_p) noexcept : m_pool(), m_allocator(m_pool), m_root(), m_comparator() 
	{
	}

	heap(heap&& rvalue_p) noexcept : m_pool(), m_allocator(m_pool), m_root(std::move(rvalue_p.m_root)), m_comparator(std::move(rvalue_p.m_comparator)) 
	{

	}

	heap(std::initializer_list<value_type>&& initializer_list_p, const key_comparator& comparator_p = Comparator()) noexcept : m_pool(), m_allocator(m_pool), m_root(), m_comparator(comparator_p) 
	{
	
	}

	template<class Container>
	heap(container::range<Container>&& range_p, const key_comparator& comparator_p = Comparator()) noexcept : m_pool(), m_allocator(m_pool), m_root(), m_comparator(comparator_p) 
	{
		FE_SUSPECT(range_p._begin >= range_p._end, "%{%s@0}: The container range %{%s@1} index cannot be greater or equal to the container range %{%s@2} index.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ILLEGAL_POSITION), TO_STRING(range_p._begin), TO_STRING(range_p._end));
		FE_SUSPECT(range_p._container.is_null() == true, "The container range reference was null.");
	}

	~heap() noexcept
	{

	}

	heap& operator=(const heap& other_p) noexcept
	{
		return *this;
	}

	heap& operator=(heap&& rvalue_p) noexcept
	{
		return *this;
	}

	heap& operator=(std::initializer_list<value_type> initializer_list_p) noexcept
	{
		return *this;
	}

private:
	void __create_new_node(T value_p) noexcept
	{
		FE_SUSPECT(this->m_root.get() == nullptr, "Unable to append a new node. The root is null.");

		// TO DO
		// 1. iterator impl
		// 2. construct an element before an end iterator
	}

	template<class InputIterator>
	void __copy_data_from_input_iterator(InputIterator first_p, InputIterator last_p) noexcept
	{
		FE_SUSPECT(first_p >= last_p, "%{%s@0}: The iterator %{%s@1}'s address value cannot be greater or equal to the iterator %{%s@2}'s address.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ILLEGAL_POSITION), TO_STRING(first_p), TO_STRING(last_p));
		FE_SUSPECT(first_p == nullptr, "The iterator %{%s@0} was nullptr.", TO_STRING(first_p));
		FE_SUSPECT(last_p == nullptr, "The iterator %{%s@0} was nullptr.", TO_STRING(last_p));
	}

	void __move_data_from_initializer_list(std::initializer_list<value_type>&& initializer_list_p) noexcept
	{
		size_type l_input_size = initializer_list_p.size();
		FE_SUSPECT(l_input_size == 0, "An initializer_list passed to a heap constructor was empty.");
	}

	template<class Container>
	void __copy_data_from_container_range(container::range<Container>&& range_p) noexcept
	{
		FE_SUSPECT(range_p._begin >= range_p._end, "%{%s@0}: The container range %{%s@1} index cannot be greater or equal to the container range %{%s@2} index.", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_ILLEGAL_POSITION), TO_STRING(range_p._begin), TO_STRING(range_p._end));
		FE_SUSPECT(range_p._container.is_null() == true, "The container range reference was null.");
	}

/*
-- FE::heap --
+ constructor() noexcept
+ destructor() noexcept
+ operator=() noexcept
+ top() noexcept
+ begin(ORDER) noexcept
+ end(ORDER) noexcept
+ cbegin(ORDER) noexcept
+ cend(ORDER) noexcept
+ is_empty() noexcept
+ size() noexcept
+ clear() noexcept
+ push() noexcept
+ pop() noexcept
+ contains(value_type) noexcept
+ find(value_type) noexcept
+ swap(heap) noexcept

+ begin()
+ end()
+ cbegin()
+ cend()

*/
};


END_NAMESPACE
#endif