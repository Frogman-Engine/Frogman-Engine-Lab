#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <FE/core/containers/hash_map.hxx>




TEST(hash_map, constructors)
{
	//{
	//	std::vector<FE::pair<const char*, int>> l_input_vec = { {"first", 1}, {"second", 2}, {"third", 3} };
	//	FE::hash_map<const char*, int> l_hash_map(l_input_vec.begin(), l_input_vec.end(), 3);
	//}

	//{
	//	FE::hash_map<const char*, int> l_hash_map = { {"first", 1}, {"second", 2}, {"third", 3} };
	//}

	{
		std::vector<FE::pair<const char*, int>> l_input_vec = { {"first", 1}, {"second", 2}, {"third", 3} };
		FE::hash_map<const char*, int> l_hash_map = FE::container::make_range(l_input_vec, 0, 3);
	}
}