#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <FE/core/containers/hash_multimap.hxx>




TEST(hash_multimap, constructors)
{
	{
		std::vector<FE::pair<const char*, int>> l_input_vec = { {"first", 1}, {"second", 2}, {"third", 3} };
		FE::hash_multimap<const char*, int> l_hash_map(l_input_vec.begin(), l_input_vec.end(), 3);
	}

	{
		FE::hash_multimap<const char*, int> l_hash_map = { {"first", 1}, {"second", 2}, {"third", 3} };
	}

	{
		std::vector<FE::pair<const char*, int>> l_input_vec = { {"first", 1}, {"second", 2}, {"third", 3} };
		FE::hash_multimap<const char*, int> l_hash_map = FE::container::make_range(l_input_vec, 0, 3);
	}

	{
		FE::hash_multimap<const char*, int> l_hash_map;
		l_hash_map = { {"first", 1}, {"second", 2}, {"third", 3} };
	}
}




//TEST(hash_multimap, Iterators)
//{
//	{
//		FE::hash_multimap<const char*, int> l_hash_map;
//		l_hash_map = { {"one", 1}, { "two", 2 }, { "three", 3 }, { "four", 4 }, { "five", 5 } , { "six", 6 }, { "seven", 7 }, { "eight", 8 }, { "nine", 9 }, { "ten", 10 } };
//
//		FE::hash_multimap<const char*, int>::iterator l_iterator = l_hash_map.begin();
//		FE::hash_multimap<const char*, int>::iterator l_end = l_hash_map.end();
//		while (l_iterator != l_end)
//		{
//			std::cout << l_iterator->_first << '\n';
//			++l_iterator;
//		}
//	}
//
//}