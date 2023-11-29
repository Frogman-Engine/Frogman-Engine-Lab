#include <gtest/gtest.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/miscellaneous/google_test_extension.h>
#include <FE/core/heap_memory.hpp>
#include <FE/core/hash.hpp>
#include <vector>
#include <deque>
#include <list>
#include <unordered_map>




TEST(allocator_adaptor_in_conjunction_with_heap_memory_util_tracker, instantiation)
{
	std::vector<int, FE::scalable_aligned_allocator<int>> l_scalable_aligned_vector;
	::std::list<int, FE::scalable_aligned_allocator<int>> l_scalable_aligned_list;
	::std::deque<int, FE::scalable_aligned_allocator<int>> l_scalable_aligned_deque;
	::std::unordered_map<int, int, FE::hash<int>, std::equal_to<int>, FE::scalable_aligned_allocator<std::pair<const int, int>>> l_scalable_aligned_hash_map;

	::std::vector<int, FE::cache_aligned_allocator<int>> l_cache_aligned_vector;
	::std::list<int, FE::cache_aligned_allocator<int>> l_cache_aligned_list;
	::std::deque<int, FE::cache_aligned_allocator<int>> l_cache_aligned_deque;
	::std::unordered_map<int, int, FE::hash<int>, std::equal_to<int>, FE::cache_aligned_allocator<std::pair<const int, int>>> l_cache_aligned_hash_map;
}
