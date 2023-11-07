#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <FE/core/containers/index_map.hxx>
#include <FE/core/string.hxx>


TEST(index_map, general)
{
	FE::index_map<FE::string> l_index_map(FE::reserve{20});
	
	EXPECT_EQ(l_index_map.capacity(), 20);
	
	auto l_hi_world_key = l_index_map.emplace("Hi, world.");
	_MAYBE_UNUSED_ auto l_bye_world_key = l_index_map.emplace("Bye, world.");
	auto l_pizza_key = l_index_map.emplace("Pizza");
	auto l_aglio_e_olio_key = l_index_map.emplace("Aglio e olio");

	EXPECT_EQ(l_index_map.size(), 4);

	EXPECT_TRUE(l_index_map.contains(l_aglio_e_olio_key, "Aglio e olio"));

	l_index_map.erase(l_hi_world_key);
	EXPECT_EQ(l_index_map.size(), 3);

	EXPECT_TRUE(l_index_map.find(l_pizza_key) == "Pizza");

	l_index_map.extend(10);
	EXPECT_EQ(l_index_map.capacity(), 30);

	l_index_map.shrink_to_fit();
	EXPECT_EQ(l_index_map.capacity(), 4);

	l_index_map.clear();
	EXPECT_EQ(l_index_map.size(), 0);

	FE::index_map<FE::string> l_another_index_map(FE::reserve{ 10 });
	l_another_index_map.swap(l_index_map);
}




#include <FE/core/allocator_adaptor.hxx>


void std_vector_with_tbb(benchmark::State& state_p) noexcept
{
	for (auto _ : state_p)
	{
		std::vector<std::byte, FE::std_style::scalable_aligned_allocator<std::byte>> l_vector;
		l_vector.reserve(1024);
	}
}

BENCHMARK(std_vector_with_tbb);


void typical_std_vector(benchmark::State& state_p) noexcept
{
	for (auto _ : state_p)
	{
		std::vector<std::byte> l_vector;
		l_vector.reserve(1024);
	}
}

BENCHMARK(typical_std_vector);