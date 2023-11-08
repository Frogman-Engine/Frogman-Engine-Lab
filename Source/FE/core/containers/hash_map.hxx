#ifndef _FE_CORE_HASH_MAP_HXX_
#define _FE_CORE_HASH_MAP_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/core/allocator_adaptor.hxx>
#include <FE/core/hash.hpp>
#include <vector>
#include <array>
#include <deque>
#include <list>




BEGIN_NAMESPACE(FE)


template<class Key, class T, class Hasher = FE::hash<Key>, class KeyEqual = std::equal_to<Key>, class Container = std::array<std::pair<const Key, T>, 64>, class Allocator = FE::std_style::scalable_aligned_allocator<Container>>
class hash_map
{
	//std::vector<Container, Allocator> m_buckets;
};


END_NAMESPACE
#endif