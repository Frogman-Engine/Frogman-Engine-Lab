#ifndef _FE_CORE_POOL_HXX_
#define _FE_CORE_POOL_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/containers/stack.hxx>
#include <FE/core/containers/queue.hxx>
#include <FE/core/hash.hpp>
#include <array>
#include <memory>
#include <unordered_map>
#pragma warning (push)
#pragma warning (disable: 6262)




BEGIN_NAMESPACE(FE)


enum struct POOL_TYPE : uint8
{
    _BLOCK = 0,
    _GENERIC = 1,
};

struct memory_region_t 
{
    const char* _name = " ";
};

struct from_low_address final
{
    _NODISCARD _FORCE_INLINE_ boolean operator()(const std::pair<std::byte*, var::size_t>& left_p, const std::pair<std::byte*, var::size_t>& right_p) const noexcept
    {
        return (left_p.first > right_p.first);
    }
};


template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity>
struct chunk;

template<typename T, size_t ChunkCapacity>
struct chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

    using value_type = T;
    using pointer = value_type*;

private:
    alignas(FE::SIMD_auto_alignment::alignment_type::size) std::array<std::byte, sizeof(T) * ChunkCapacity> m_memory;

public:
    constexpr static count_t chunk_capacity = ChunkCapacity;

    FE::fstack<pointer, ChunkCapacity> _unused_blocks;
    pointer const _begin = reinterpret_cast<pointer const>(m_memory.data());
    pointer _page_iterator = _begin;
    pointer const _end = _begin + ChunkCapacity;

    _FORCE_INLINE_ boolean is_full() const noexcept
    {
        return (_unused_blocks.is_empty() == true) && (_page_iterator == _end);
    }
};

template<size_t ChunkCapacity>
struct chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>
{
    using pair_type = std::pair<std::byte*, var::size_t>;

private:
    alignas(FE::SIMD_auto_alignment::alignment_type::size) std::array<std::byte, ChunkCapacity> m_memory;

public:
    constexpr static count_t chunk_capacity = ChunkCapacity;

    /*
     std::pair's first contains the address of the memory block.
     std::pair's second contains the size of the memory block.
    */
    FE::fpriority_queue<pair_type, ChunkCapacity, from_low_address> _unused_blocks;
    std::byte* const _begin = m_memory.data();
    std::byte* _page_iterator = _begin;
    std::byte* const _end = _begin + m_memory.size();

    _FORCE_INLINE_ boolean is_full() const noexcept
    {
        return (_unused_blocks.is_empty() == true) && (_page_iterator == _end);
    }
};


template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class Alignment>
struct pool_deleter;

template<typename T, size_t ChunkCapacity, class Alignment>
struct pool_deleter<T, POOL_TYPE::_BLOCK, ChunkCapacity, Alignment>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

    using chunk_type = chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity>;
    using value_type = typename std::remove_const<typename std::remove_all_extents<typename std::remove_reference<T>::type>::type>::type;

private:
    chunk_type* m_host_chunk = nullptr;

public:
    constexpr static count_t chunk_capacity = ChunkCapacity;

    _FORCE_INLINE_ pool_deleter() noexcept : m_host_chunk() {}
    _FORCE_INLINE_ pool_deleter(chunk_type* host_p) noexcept : m_host_chunk(host_p) {}

    _FORCE_INLINE_ void operator()(value_type* ptr_p) const noexcept
    {
        FE_ASSERT(this->m_host_chunk == nullptr, "${%s@0}: ${%s@1} was nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_host_chunk));
        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }

        if constexpr (FE::is_trivial<value_type>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
        {
            ptr_p->~value_type();
        }
   
        this->m_host_chunk->_unused_blocks.push(ptr_p);
    }
};


template<size_t ChunkCapacity>
struct generic_deleter_base
{
    using chunk_type = chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>;
    using pair_type = std::pair<std::byte*, var::size_t>;

    constexpr static count_t chunk_capacity = ChunkCapacity;

protected:
    chunk_type* m_host_chunk = nullptr;
    var::size_t m_size_in_bytes = 0;

public:
    _FORCE_INLINE_ generic_deleter_base() noexcept : m_host_chunk(), m_size_in_bytes() {}
    _FORCE_INLINE_ generic_deleter_base(chunk_type* host_p, size_t size_in_bytes_p) noexcept : m_host_chunk(host_p), m_size_in_bytes(size_in_bytes_p) {}

    _FORCE_INLINE_ chunk_type* get_host_chunk_ptr() const noexcept
    {
		return this->m_host_chunk;
	}

    _FORCE_INLINE_ size_t get_size_in_bytes() const noexcept
    {
        return this->m_size_in_bytes;
    }
};

template<size_t ChunkCapacity>
struct nondestructive_generic_deleter final : public generic_deleter_base<ChunkCapacity>
{
    using base_type = generic_deleter_base<ChunkCapacity>;
    using chunk_type = typename base_type::chunk_type;
    using pair_type = typename base_type::pair_type;

    _FORCE_INLINE_ nondestructive_generic_deleter() noexcept : base_type() {}
    _FORCE_INLINE_ nondestructive_generic_deleter(chunk_type* host_p, size_t size_in_bytes_p) noexcept : base_type(host_p, size_in_bytes_p) {}

    void operator()(void* ptr_p) const noexcept
    {
        FE_ASSERT(this->m_host_chunk == nullptr, "${%s@0}: ${%s@1} was nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_host_chunk));

        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }


        FE_ASSERT(this->m_size_in_bytes == 0, "Assertion Failed: ${%s@0} cannot be zero.", TO_STRING(this->m_size_in_bytes));
        this->m_host_chunk->_unused_blocks.push(pair_type{ static_cast<std::byte*>(ptr_p), this->m_size_in_bytes });
    }
};

template<typename T, size_t ChunkCapacity, class Alignment>
struct pool_deleter<T, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment> final : public generic_deleter_base<ChunkCapacity>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

    using base_type = generic_deleter_base<ChunkCapacity>;
    using chunk_type = typename base_type::chunk_type;
    using pair_type = typename base_type::pair_type;
    using value_type = typename std::remove_const<typename std::remove_all_extents<typename std::remove_reference<T>::type>::type>::type;

private:
    var::count_t m_element_count = 0;

public:
    _FORCE_INLINE_ pool_deleter() noexcept : base_type() {}
    _FORCE_INLINE_ pool_deleter(chunk_type* host_p, count_t elements_p, size_t size_in_bytes_p) noexcept : base_type(host_p, size_in_bytes_p), m_element_count(elements_p) {}

    void operator()(void* ptr_p) const noexcept
    {
        FE_ASSERT(this->m_host_chunk == nullptr, "${%s@0}: ${%s@1} was nullptr", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_NULLPTR), TO_STRING(this->m_host_chunk));
        FE_ASSERT(this->m_size_in_bytes < this->m_element_count, "Assertion Failed: Detected arguments with illegal values.");

        if (ptr_p == nullptr || this->m_host_chunk == nullptr) { return; }
     
        if constexpr (FE::is_trivial<value_type>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
        {
            value_type* l_ptr = static_cast<value_type*>(ptr_p);
            void* const l_ptr_end = static_cast<value_type*>(ptr_p) + this->m_element_count;

            while (l_ptr != l_ptr_end)
            {
                l_ptr->~value_type();
                ++l_ptr;
            }
        }

        FE_ASSERT(this->m_size_in_bytes == 0, "Assertion Failed: ${%s@0} cannot be zero.", TO_STRING(this->m_size_in_bytes));
        this->m_host_chunk->_unused_blocks.push(pair_type{ static_cast<std::byte*>(ptr_p), this->m_size_in_bytes });
    }

    _FORCE_INLINE_ count_t get_element_count() const noexcept
    {
        return this->m_element_count;
    }
};




template<typename T, POOL_TYPE PoolType, size_t ChunkCapacity, class GlobalAllocator = FE::std_style::scalable_aligned_allocator<chunk<T, PoolType, ChunkCapacity>>, class RegionAllocator = FE::std_style::scalable_aligned_allocator<std::pair<const char* const, chunk<T, PoolType, ChunkCapacity>>>>
class pool;

template<typename T, size_t ChunkCapacity, class GlobalAllocator, class RegionAllocator>
class pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, GlobalAllocator, RegionAllocator>
{
    FE_STATIC_ASSERT(std::is_array<T>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
    FE_STATIC_ASSERT(std::is_const<T>::value == true, "Static Assertion Failed: The T must not be a const type.");

public:
    using chunk_type = chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity>;
    using deleter_type = pool_deleter<T, POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>;

    using global = std::list<chunk_type, GlobalAllocator>;
    using regions = std::unordered_map<const char*, chunk_type, FE::hash<const char*>, algorithm::string::equal_to<char>, RegionAllocator>;

    constexpr static count_t chunk_capacity = ChunkCapacity;

    FE_STATIC_ASSERT((std::is_same<T, typename chunk_type::value_type>::value == false), "Static Assertion Failed: The value_type does not match.");
    FE_STATIC_ASSERT((std::is_same<T*, typename chunk_type::pointer>::value == false), "Static Assertion Failed: The value_type* does not match.");
    FE_STATIC_ASSERT((chunk_type::chunk_capacity != chunk_capacity), "Static Assertion Failed: The chunk_capacity does not match.");

private:
    thread_local static typename global tl_s_global_memory;
    thread_local static typename regions tl_s_memory_regions;

public:
    static std::unique_ptr<T, deleter_type> allocate(memory_region_t region_p) noexcept
    {
        if ((tl_s_memory_regions.empty() == true)) _UNLIKELY_
        {
            create_pages(1, region_p);
        }

        size_t l_target_bucket_index = tl_s_memory_regions.bucket(region_p._name);
        typename regions::local_iterator l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
        typename regions::const_local_iterator l_cend = tl_s_memory_regions.cend(l_target_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: The memory region named ${%s@0} does not exist.", region_p._name);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->second.is_full() == false)
            {
                T* l_value;
                if (l_list_iterator->second._unused_blocks.is_empty() == false)
                {
                    l_value = l_list_iterator->second._unused_blocks.pop();
                    
                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }
                }
                else
                {
                    l_value = l_list_iterator->second._page_iterator;

                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }

                    ++(l_list_iterator->second._page_iterator);
                }
          
                return std::unique_ptr<T, deleter_type>(l_value, deleter_type(&(l_list_iterator->second)));
            }
        }

        create_pages(1, region_p);
        return allocate(region_p);
    }

    static std::unique_ptr<T, deleter_type> allocate() noexcept
    {
        if ((tl_s_global_memory.empty() == true)) _UNLIKELY_
        {
            create_pages(1);
        }

        typename global::iterator l_list_iterator = tl_s_global_memory.begin();
        typename global::const_iterator l_cend = tl_s_global_memory.cend();

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->is_full() == false)
            {
                T* l_value;
                if (l_list_iterator->_unused_blocks.is_empty() == false)
                {
                    l_value = l_list_iterator->_unused_blocks.pop();
                    
                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }
                }
                else
                {
                    l_value = l_list_iterator->_page_iterator;

                    if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                    {
                        new(l_value) T();
                    }
                    
                    ++(l_list_iterator->_page_iterator);
                }

                return std::unique_ptr<T, deleter_type>(l_value, deleter_type(l_list_iterator.operator->()));
            }
        }

        create_pages(1);
        return allocate();
    }


    static void create_pages(size_t chunk_count_p, memory_region_t region_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        tl_s_memory_regions.reserve(chunk_count_p * 2);
        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_memory_regions.insert({ region_p._name, chunk_type{} });
        }
    }

    static void create_pages(size_t chunk_count_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_global_memory.emplace_back(chunk_type{});
        }
    }


    static void shrink_to_fit(memory_region_t region_p) noexcept
    {
        size_t l_target_bucket_index = tl_s_memory_regions.bucket(region_p._name);
        typename regions::local_iterator l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
        typename regions::const_local_iterator l_cend = tl_s_memory_regions.cend(l_target_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: The memory region named ${%s@0} does not exist.", region_p._name);


        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_element_size = l_list_iterator->second._unused_blocks.size();

            if (l_list_iterator->second._page_iterator < l_list_iterator->second._end)
            {
                l_unused_element_size += (l_list_iterator->second._end - l_list_iterator->second._page_iterator);
            }

            if (l_unused_element_size == ChunkCapacity)
            {
                tl_s_memory_regions.erase(l_list_iterator);

                if (tl_s_memory_regions.bucket_size(l_target_bucket_index) == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
            }
        }
    }

    static void shrink_to_fit() noexcept
    {
        typename global::iterator l_list_iterator = tl_s_global_memory.begin();
        typename global::const_iterator l_cend = tl_s_global_memory.cend();
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to shrink_to_fit an empty memory pool");


        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_element_size = l_list_iterator->_unused_blocks.size();

            FE_ASSERT((l_list_iterator->_end - l_list_iterator->_begin) != ChunkCapacity, "The chunk range is invalid.");
          
            if (l_list_iterator->_page_iterator < l_list_iterator->_end)
            {
                l_unused_element_size += (l_list_iterator->_end - l_list_iterator->_page_iterator);
            }

            if (l_unused_element_size == ChunkCapacity)
            {
                tl_s_global_memory.erase(l_list_iterator);

                if (tl_s_global_memory.size() == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_global_memory.begin();
            }
        }
    }
};


template<typename T, size_t ChunkCapacity, class GlobalAllocator, class RegionAllocator>
thread_local typename pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, GlobalAllocator, RegionAllocator>::global pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, GlobalAllocator, RegionAllocator>::tl_s_global_memory;

template<typename T, size_t ChunkCapacity, class GlobalAllocator, class RegionAllocator>
thread_local typename pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, GlobalAllocator, RegionAllocator>::regions pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, GlobalAllocator, RegionAllocator>::tl_s_memory_regions;




template<size_t ChunkCapacity, class GlobalAllocator, class RegionAllocator>
class pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, GlobalAllocator, RegionAllocator>
{
public:
    using chunk_type = chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>;

    template<typename U, class Alignment>
    using deleter_type = pool_deleter<U, POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>;

    using global = std::list<chunk_type, GlobalAllocator>;
    using regions = std::unordered_map<const char*, chunk_type, FE::hash<const char*>, algorithm::string::equal_to<char>, RegionAllocator>;
    using pair_type = std::pair<std::byte*, var::size_t>;

    constexpr static size_t chunk_capacity = ChunkCapacity;

private:
    thread_local static typename global tl_s_global_memory;
    thread_local static typename regions tl_s_memory_regions;

public:
    template<typename U, class Alignment = FE::align_8bytes>
    static std::unique_ptr<U, deleter_type<U, Alignment>> allocate(count_t size_p, memory_region_t region_p) noexcept
    {
        FE_STATIC_ASSERT((Alignment::size % 2) != 0, "Static Assertion Failed: The Alignment::size must be an even number.");
        FE_STATIC_ASSERT(std::is_array<U>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
        FE_STATIC_ASSERT(std::is_const<U>::value == true, "Static Assertion Failed: The T must not be a const type.");
        FE_ASSERT(size_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(size_p));

        size_t l_queried_allocation_size_in_bytes = FE::calculate_aligned_memory_size_in_bytes<U, Alignment>(size_p);


        if ((tl_s_memory_regions.empty() == true)) _UNLIKELY_
        {
            create_pages(1, region_p);
        }

        size_t l_target_bucket_index = tl_s_memory_regions.bucket(region_p._name);
        typename regions::local_iterator l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
        typename regions::const_local_iterator l_cend = tl_s_memory_regions.cend(l_target_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: The memory region named ${%s@0} does not exist.", region_p._name);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->second.is_full() == false)
            {
                void* l_allocation_result = nullptr;
                pair_type l_memblock_info;

                if (l_list_iterator->second._unused_blocks.is_empty() == false)
                {
                    /*
                    std::pair's first contains the address of the memory block.
                    std::pair's second contains the size of the memory block.
                    */

                    __recycle(l_memblock_info, l_list_iterator->second, l_queried_allocation_size_in_bytes);

                    if (l_memblock_info.second < l_queried_allocation_size_in_bytes)
                    {
                        l_allocation_result = l_list_iterator->second._page_iterator;
                        l_list_iterator->second._page_iterator += l_queried_allocation_size_in_bytes;
                    }
                    else
                    {
                        l_allocation_result = l_memblock_info.first;
                    }
                }
                else
                {
                    l_allocation_result = l_list_iterator->second._page_iterator;
                    l_list_iterator->second._page_iterator += l_queried_allocation_size_in_bytes;
                }

                if constexpr (FE::is_trivial<U>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    U* l_iterator = static_cast<U*>(l_allocation_result);
                    for (var::index_t i = 0; i < size_p; ++i)
                    {
                        new(l_iterator) U();
                        ++l_iterator;
                    }
                }

                if (l_list_iterator->second._page_iterator > l_list_iterator->second._end)
                {
                    create_pages(1, region_p);
                    return allocate<U, Alignment>(1, region_p);
                }

                return std::unique_ptr<U, deleter_type<U, Alignment>>{static_cast<U*>(l_allocation_result), deleter_type<U, Alignment>{reinterpret_cast<chunk_type*>(&l_list_iterator->second), size_p, l_queried_allocation_size_in_bytes}};
            }
        }

        create_pages(1, region_p);
        return allocate<U, Alignment>(1, region_p);
    }

    template<typename U, class Alignment = FE::align_8bytes>
    static std::unique_ptr<U, deleter_type<U, Alignment>> allocate(count_t size_p = 1) noexcept
    {
        FE_STATIC_ASSERT((Alignment::size % 2) != 0, "Static Assertion Failed: The Alignment::size must be an even number.");
        FE_STATIC_ASSERT(std::is_array<U>::value == true, "Static Assertion Failed: The T must not be an array[] type.");
        FE_STATIC_ASSERT(std::is_const<U>::value == true, "Static Assertion Failed: The T must not be a const type.");
        FE_ASSERT(size_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(size_p));

        size_t l_queried_allocation_size_in_bytes = FE::calculate_aligned_memory_size_in_bytes<U, Alignment>(size_p);

        if ((tl_s_global_memory.empty() == true)) _UNLIKELY_
        {
            create_pages(1);
        }

        typename global::iterator l_list_iterator = tl_s_global_memory.begin();
        typename global::const_iterator l_cend = tl_s_global_memory.cend();

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if (l_list_iterator->is_full() == false)
            {
                void* l_allocation_result = nullptr;
                pair_type l_memblock_info;

                if (l_list_iterator->_unused_blocks.is_empty() == false)
                {
                    /*
                    std::pair's first contains the address of the memory block.
                    std::pair's second contains the size of the memory block.
                    */

                    __recycle(l_memblock_info, *l_list_iterator, l_queried_allocation_size_in_bytes);

                    if (l_memblock_info.second < l_queried_allocation_size_in_bytes)
                    {
                        l_allocation_result = l_list_iterator->_page_iterator;
                        l_list_iterator->_page_iterator += l_queried_allocation_size_in_bytes;
                    }
                    else
                    {
                        l_allocation_result = l_memblock_info.first;
                    }
                }
                else
                {
                    l_allocation_result = l_list_iterator->_page_iterator;
                    l_list_iterator->_page_iterator += l_queried_allocation_size_in_bytes;
                }


                if constexpr (FE::is_trivial<U>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    U* l_iterator = static_cast<U*>(l_allocation_result);
                    for (var::index_t i = 0; i < size_p; ++i)
                    {
                        new(l_iterator) U();
                        ++l_iterator;
                    }
                }


                if (l_list_iterator->_page_iterator > l_list_iterator->_end)
                {
                    create_pages(1);
                    return allocate<U, Alignment>(1);
                }

                return std::unique_ptr<U, deleter_type<U, Alignment>>{static_cast<U*>(l_allocation_result), deleter_type<U, Alignment>{reinterpret_cast<chunk_type*>(l_list_iterator.operator->()), size_p, l_queried_allocation_size_in_bytes}};
            }                                                                                                                                                  
        }

        create_pages(1);
        return allocate<U, Alignment>();
    }


    static void create_pages(size_t chunk_count_p, memory_region_t region_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        tl_s_memory_regions.reserve(chunk_count_p * 2);
        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_memory_regions.insert({ region_p._name, chunk_type{} });
        }
    }

    static void create_pages(size_t chunk_count_p) noexcept
    {
        FE_ASSERT(chunk_count_p == 0, "${%s@0}: ${%s@1} was 0", TO_STRING(MEMORY_ERROR_1XX::_FATAL_ERROR_INVALID_SIZE), TO_STRING(chunk_count_p));

        for (var::size_t i = 0; i < chunk_count_p; ++i)
        {
            tl_s_global_memory.emplace_back(chunk_type{});
        }
    }


    static void shrink_to_fit(memory_region_t region_p) noexcept
    {
        size_t l_target_bucket_index = tl_s_memory_regions.bucket(region_p._name);

        typename regions::local_iterator l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
        typename regions::const_local_iterator l_cend = tl_s_memory_regions.cend(l_target_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: The memory region named ${%s@0} does not exist.", region_p._name);


        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_memory_size_in_bytes = 0;
            auto l_container = l_list_iterator->second._unused_blocks.get_container();
            for (auto block : l_container)
            {
                l_unused_memory_size_in_bytes += block.second;
            }

            if (l_list_iterator->second._page_iterator < l_list_iterator->second._end)
            {
                l_unused_memory_size_in_bytes += l_list_iterator->second._end - l_list_iterator->second._page_iterator;
            }

            if (l_unused_memory_size_in_bytes == ChunkCapacity)
            {
                tl_s_memory_regions.erase(l_list_iterator);

                if (tl_s_memory_regions.bucket_size(l_target_bucket_index) == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
            }
        }
    }

    static void shrink_to_fit() noexcept
    {
        typename global::iterator  l_list_iterator = tl_s_global_memory.begin();
        typename global::const_iterator l_cend = tl_s_global_memory.cend();
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to shrink_to_fit an empty memory pool");

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            var::size_t l_unused_memory_size_in_bytes = 0;
            auto l_container = l_list_iterator->_unused_blocks.get_container();
            for (auto block : l_container)
            {
                l_unused_memory_size_in_bytes += block.second;
            }

            if (l_list_iterator->_page_iterator < l_list_iterator->_end)
            {
                l_unused_memory_size_in_bytes += l_list_iterator->_end - l_list_iterator->_page_iterator;
            }

            if (l_unused_memory_size_in_bytes == ChunkCapacity)
            {
                tl_s_global_memory.erase(l_list_iterator);

                if (tl_s_global_memory.size() == 0)
                {
                    break;
                }

                l_list_iterator = tl_s_global_memory.begin();
            }
        }
    }


    template <typename T, class Alignment>
    static void deallocate(T* pointer_p, count_t element_count_p) noexcept 
    {
        typename global::iterator  l_list_iterator = tl_s_global_memory.begin();
        typename global::const_iterator l_cend = tl_s_global_memory.cend();
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to return an address to an empty memory pool.");
        
        std::byte* const l_value = reinterpret_cast<std::byte*>(pointer_p);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if ((l_list_iterator->_begin <= l_value) && (l_value < l_list_iterator->_end))
            {
                if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    for (var::count_t i = 0; i < element_count_p; ++i)
                    {
                        pointer_p->~T();
                        ++pointer_p;
                    }
                }
                l_list_iterator->_unused_blocks.push(pair_type{ l_value, FE::calculate_aligned_memory_size_in_bytes<T, Alignment>(element_count_p) });
                return;
            }
        }
    }

    template <typename T, class Alignment>
    static void deallocate(T* pointer_p, count_t element_count_p, memory_region_t region_p) noexcept
    {
        size_t l_target_bucket_index = tl_s_memory_regions.bucket(region_p._name);

        typename regions::local_iterator l_list_iterator = tl_s_memory_regions.begin(l_target_bucket_index);
        typename regions::const_local_iterator l_cend = tl_s_memory_regions.cend(l_target_bucket_index);
        FE_ASSERT(l_list_iterator == l_cend, "Assertion Failure: Unable to return an address to an empty memory pool.");

        std::byte* const l_value = reinterpret_cast<std::byte*>(pointer_p);

        for (; l_list_iterator != l_cend; ++l_list_iterator)
        {
            if ((l_list_iterator->second._begin < l_value) && (l_value < l_list_iterator->second._end))
            {
                if constexpr (FE::is_trivial<T>::value == FE::TYPE_TRIVIALITY::_NOT_TRIVIAL)
                {
                    for (var::count_t i = 0; i < element_count_p; ++i)
                    {
                        pointer_p->~T();
                        ++pointer_p;
                    }
                }
                FE_ASSERT((FE::calculate_aligned_memory_size_in_bytes<T, Alignment>(element_count_p) < element_count_p), "Assertion Failure: Detected an invalid argument value. ${%s@0} was ${%lu@1}.", TO_STRING(element_count_p), &element_count_p);

                l_list_iterator->second._unused_blocks.push(pair_type{ l_value, FE::calculate_aligned_memory_size_in_bytes<T, Alignment>(element_count_p) });
                return;
            }
        }
    }

private:
    static void __merge(FE::fstack<pair_type, ChunkCapacity>& temporary_strorage_p, FE::fpriority_queue<pair_type, ChunkCapacity, from_low_address>& unused_scattered_blocks_p) noexcept
    {
        /*
                        std::pair's first contains the address of the memory block.
                        std::pair's second contains the size of the memory block.
        */

        while (unused_scattered_blocks_p.size() > 1)
        {
            auto l_prev = unused_scattered_blocks_p.top();
            unused_scattered_blocks_p.pop();
            
            auto l_next = unused_scattered_blocks_p.top();

            FE_ASSERT(l_prev.first >= l_next.first, "Assertion Failure: The priority queue has illegal address order. ${%s@0} always has lower address value than ${%s@1}.", TO_STRING(l_prev.first), TO_STRING(l_next.first));
            FE_ASSERT(l_prev.first + l_prev.second > l_next.first, "${%s@0}: Free-ed memory block range collision detected!\nPlease check if the count value passed to generic_pool<>::deallocate<T, Alignment>(T* ptr_p, count_t element_count_p) was incorrect or not.", TO_STRING(FE::MEMORY_ERROR_1XX::_FATAL_ERROR_ACCESS_VIOLATION));

            if ((l_prev.first + l_prev.second) == l_next.first)
            {
                unused_scattered_blocks_p.pop();
                unused_scattered_blocks_p.push({ l_prev.first, l_prev.second + l_next.second });
            }
            else
            {
                temporary_strorage_p.push(l_prev);
            }
        }
       
        while(temporary_strorage_p.is_empty() == false)
		{
			unused_scattered_blocks_p.push(std::move(temporary_strorage_p.pop()));
		}
    }

    static void __recycle(pair_type& in_out_memblock_info_p, chunk_type& memory_p, size_t queried_allocation_size_in_bytes_p) noexcept
    {
        FE_ASSERT(memory_p._unused_blocks.is_empty() == true, "Assertion Failure: Cannot recycle from an empty bin.");

        /*
           std::pair's first contains the address of the memory block.
           std::pair's second contains the size of the memory block.
           */
        thread_local static FE::fstack<pair_type, ChunkCapacity> tl_s_tmp_strorage;

        if (memory_p._unused_blocks.size() > 1)
        {
            __merge(tl_s_tmp_strorage, memory_p._unused_blocks);
        }

        in_out_memblock_info_p = memory_p._unused_blocks.top();
        while (in_out_memblock_info_p.second < queried_allocation_size_in_bytes_p)
        {
            tl_s_tmp_strorage.push(std::move(in_out_memblock_info_p));
            memory_p._unused_blocks.pop();

            if (memory_p._unused_blocks.is_empty() == true)
            {
                break;
            }
            else
            {
                in_out_memblock_info_p = memory_p._unused_blocks.top();
            }
        }

        if (memory_p._unused_blocks.is_empty() == false)
        {
            memory_p._unused_blocks.pop();
        }

        int64 l_remaining_size = (in_out_memblock_info_p.second - queried_allocation_size_in_bytes_p);
        if (l_remaining_size > 0)
        {
            memory_p._unused_blocks.push(pair_type{ (in_out_memblock_info_p.first + queried_allocation_size_in_bytes_p), l_remaining_size });
            in_out_memblock_info_p.second = queried_allocation_size_in_bytes_p;
        }

        while (tl_s_tmp_strorage.is_empty() == false)
        {
            memory_p._unused_blocks.push(std::move(tl_s_tmp_strorage.pop()));
        }

        tl_s_tmp_strorage.pop_all();
    }
};


template<size_t ChunkCapacity, class GlobalAllocator, class RegionAllocator>
thread_local typename pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, GlobalAllocator, RegionAllocator>::global pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, GlobalAllocator, RegionAllocator>::tl_s_global_memory;

template<size_t ChunkCapacity, class GlobalAllocator, class RegionAllocator>
thread_local typename pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, GlobalAllocator, RegionAllocator>::regions pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, GlobalAllocator, RegionAllocator>::tl_s_memory_regions;




template<typename T, size_t ChunkCapacity = 1024, class GlobalAllocator = FE::std_style::scalable_aligned_allocator<chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity>>, class RegionAllocator = FE::std_style::scalable_aligned_allocator<std::pair<const char* const, chunk<T, POOL_TYPE::_BLOCK, ChunkCapacity>>>>
using block_pool = pool<T, POOL_TYPE::_BLOCK, ChunkCapacity, GlobalAllocator, RegionAllocator>;

template<size_t ChunkCapacity = 10240, class GlobalAllocator = FE::std_style::scalable_aligned_allocator<chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>>, class RegionAllocator = FE::std_style::scalable_aligned_allocator<std::pair<const char* const, chunk<void, POOL_TYPE::_GENERIC, ChunkCapacity>>>>
using generic_pool = pool<void, POOL_TYPE::_GENERIC, ChunkCapacity, GlobalAllocator, RegionAllocator>;


template<typename T, size_t ChunkCapacity = 1024>
using block_pool_ptr = std::unique_ptr<T, pool_deleter<T, FE::POOL_TYPE::_BLOCK, ChunkCapacity, FE::align_custom_bytes<sizeof(T)>>>;

template<typename T, size_t ChunkCapacity = 10240, class Alignment = FE::align_8bytes>
using generic_pool_ptr = std::unique_ptr<T, pool_deleter<T, FE::POOL_TYPE::_GENERIC, ChunkCapacity, Alignment>>;


template<uint64 Capacity>
struct capacity final
{
    _MAYBE_UNUSED_ static constexpr inline size_t size = Capacity;
};


END_NAMESPACE
#pragma warning (pop)
#endif