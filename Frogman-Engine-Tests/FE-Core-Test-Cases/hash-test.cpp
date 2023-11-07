#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/hash.hpp>




TEST(hash, all)
{
	EXPECT_EQ(FE::hash<const char*>{}("string"), FE::hash<const char*>{}("string"));

	EXPECT_EQ(FE::hash<const wchar_t*>{}(L"wide string"), FE::hash<const wchar_t*>{}(L"wide string"));

	std::unique_ptr<int*> l_double_ptr( new int* {new int});
	EXPECT_EQ( FE::hash<int*>{}(*l_double_ptr), FE::hash<int*>{}(*l_double_ptr) );

	std::vector<int> l_vec = { 1, 2, 3 };
	EXPECT_EQ(FE::hash<std::vector<int>>{}(l_vec), FE::hash<std::vector<int>>{}(l_vec));

}


void std_hash_benchmark(benchmark::State& state_p)
{
	std::hash<std::string> l_hasher;
	static std::string l_string = "_NODISCARD_ _CONSTEXPR20_ _FORCE_INLINE_ count<char> count_chars(const char* string_p, const char target_p) noexcept";

	for (auto _ : state_p)
	{
		auto l_result = l_hasher(l_string);
		(void)l_result;
	}
}

BENCHMARK(std_hash_benchmark);


void city_hash_benchmark(benchmark::State& state_p)
{
	FE::hash<const char*> l_hasher;
	auto l_content = "_NODISCARD_ _CONSTEXPR20_ _FORCE_INLINE_ count<char> count_chars(const char* string_p, const char target_p) noexcept";

	for (auto _ : state_p)
	{
		auto l_result = l_hasher(l_content);
		(void)l_result;
	}
}

BENCHMARK(city_hash_benchmark);