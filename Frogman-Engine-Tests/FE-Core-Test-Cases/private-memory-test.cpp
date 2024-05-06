﻿#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/memory.hxx>
#include <FE/core/algorithm/string.hxx>
using namespace FE;




TEST(memmove, string_insertion)
{
	std::unique_ptr<char[]> l_string(new char[64] {"Freddy's Pizza\0"});
	let character l_string2[] = " Fazbear";
	let length_t l_string2_len = algorithm::string::length(l_string2);

	let index_t l_target_location = algorithm::string::find_the_first(l_string.get(), "'")->_begin;
	auto l_rest = algorithm::string::find_the_first(l_string.get(), "'s Pizza");
	let length_t l_rest_length = l_rest->_end - l_rest->_begin;

	FE::memmove(l_string.get() + l_target_location + l_string2_len, l_string.get() + l_target_location, l_rest_length);

	FE::memcpy(l_string.get() + l_target_location, l_string2, l_string2_len);

	EXPECT_TRUE(algorithm::string::compare(l_string.get(), "Freddy Fazbear's Pizza"));
}

TEST(memmove, General)
{
	std::unique_ptr<var::int64[]> l_array(new var::int64[]{ 1, 0, 1, 2, 3, 4, 5, 1, 1, 1, 1, 1});

	FE::memmove(l_array.get() + 3, l_array.get() + 1, sizeof(int64) * 6);

	EXPECT_EQ(l_array.get()[0], 1);
	EXPECT_EQ(l_array.get()[1], 0);
	EXPECT_EQ(l_array.get()[2], 1);
	EXPECT_EQ(l_array.get()[3], 0);
	EXPECT_EQ(l_array.get()[4], 1);
	EXPECT_EQ(l_array.get()[5], 2);
	EXPECT_EQ(l_array.get()[6], 3);
	EXPECT_EQ(l_array.get()[7], 4);
	EXPECT_EQ(l_array.get()[8], 5);
}

TEST(memcpy, _)
{
	constexpr auto l_length = 10;
	std::unique_ptr<int[]> l_source(new int [l_length]{1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
	std::unique_ptr<int[]> l_destination(new int [l_length]{0});

	FE::memcpy(l_destination.get(), l_source.get(), l_length * sizeof(int));

	for (let var::int32 i = 0; i < l_length; ++i)
	{
		EXPECT_EQ(l_destination.get()[i], 1);
	}
}

TEST(memset, _)
{
	constexpr auto l_length = 10;
	var::byte l_arr[l_length];
	
	FE::memset(l_arr, 100, l_length * sizeof(var::byte));
		
	for (let var::int32 i = 0; i < l_length; ++i)
	{
		EXPECT_EQ(l_arr[i], 100);
	}
}




#define _MAGICAL_SIZE_ 102400

void FE_aligned_memcpy_benchmark(benchmark::State& state_p) noexcept
{
	static alignas(64) std::byte l_dest[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_dest);
	static alignas(64) std::byte l_source[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_source);

	for (auto _ : state_p)
	{
		FE::memcpy<FE::ADDRESS::_ALIGNED, FE::ADDRESS::_ALIGNED>(l_dest, l_source, _MAGICAL_SIZE_);
		FE::memcpy<FE::ADDRESS::_ALIGNED, FE::ADDRESS::_ALIGNED>(l_source, l_dest, _MAGICAL_SIZE_);
	}
}
BENCHMARK(FE_aligned_memcpy_benchmark);

void std_memcpy_benchmark(benchmark::State& state_p) noexcept
{
	static alignas(64) std::byte l_dest[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_dest);
	static alignas(64) std::byte l_source[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_source);

	for (auto _ : state_p)
	{
		std::memcpy(l_dest, l_source, _MAGICAL_SIZE_);
		std::memcpy(l_source, l_dest, _MAGICAL_SIZE_);
	}
}
BENCHMARK(std_memcpy_benchmark);


void FE_aligned_memmove_benchmark(benchmark::State& state_p) noexcept
{
	static alignas(64) std::byte l_mem[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_mem);

	for (auto _ : state_p)
	{
		FE::memmove<FE::ADDRESS::_ALIGNED>(l_mem + 64, l_mem + 128, 10240);
	}
}
BENCHMARK(FE_aligned_memmove_benchmark);

void std_memmove_benchmark(benchmark::State& state_p) noexcept
{
	static alignas(64) std::byte l_mem[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_mem);

	for (auto _ : state_p)
	{
		std::memmove(l_mem + 64, l_mem + 128, 10240);
	}
}
BENCHMARK(std_memmove_benchmark);


void FE_aligned_memset_benchmark(benchmark::State& state_p) noexcept
{
	static alignas(64) std::byte l_dest[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_dest);

	for (auto _ : state_p)
	{
		FE::memset<FE::ADDRESS::_ALIGNED>(l_dest, _NULL_, _MAGICAL_SIZE_);
		FE::memset<FE::ADDRESS::_ALIGNED>(l_dest, 1, _MAGICAL_SIZE_);
	}
}
BENCHMARK(FE_aligned_memset_benchmark);

void std_memset_benchmark(benchmark::State& state_p) noexcept
{
	static alignas(64) std::byte l_dest[_MAGICAL_SIZE_];
	benchmark::DoNotOptimize(l_dest);

	for (auto _ : state_p)
	{
		std::memset(l_dest, _NULL_, _MAGICAL_SIZE_);
		std::memset(l_dest, 1, _MAGICAL_SIZE_);
	}
}
BENCHMARK(std_memset_benchmark);

#undef _MAGICAL_SIZE_ 