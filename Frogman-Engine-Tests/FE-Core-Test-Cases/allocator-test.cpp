#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/miscellaneous/google_test_extension.h>
#include <FE/core/allocator.hxx>




struct test {};

TEST(allocator, scalable_aligned_allocator)
{
	//EXPECT_EQ(FE::heap_memory_tracker<test>::query_type_data()._thread_local_total_bytes_by_type, 0);

	{
		test* l_ptc = FE::scalable_aligned_allocator<test>::allocate(100);
		//EXPECT_EQ(FE::heap_memory_tracker<test>::query_type_data()._thread_local_total_bytes_by_type, sizeof(test) * 100);
		l_ptc = FE::scalable_aligned_allocator<test>::reallocate(l_ptc, 100, 10);
		FE::scalable_aligned_allocator<test>::deallocate(l_ptc, 10);
	}

	//EXPECT_EQ(FE::heap_memory_tracker<test>::query_type_data()._thread_local_total_bytes_by_type, 0);
}




TEST(allocator, cache_aligned_allocator)
{
	//EXPECT_EQ(FE::heap_memory_tracker<test>::query_type_data()._thread_local_total_bytes_by_type, 0);

	{
		test* l_ptc = FE::cache_aligned_allocator<test>::allocate(100);
		//EXPECT_EQ(FE::heap_memory_tracker<test>::query_type_data()._thread_local_total_bytes_by_type, sizeof(test) * 100);
		l_ptc = FE::cache_aligned_allocator<test>::reallocate(l_ptc, 100, 10);
		FE::cache_aligned_allocator<test>::deallocate(l_ptc, 10);
	}

	//EXPECT_EQ(FE::heap_memory_tracker<test>::query_type_data()._thread_local_total_bytes_by_type, 0);

}




void c_malloc_free_performance_benchmark(benchmark::State& state_p) noexcept
{
	for (auto _ : state_p)
	{
		void* l_pointer = malloc(1024);
		benchmark::DoNotOptimize(l_pointer);
		free(l_pointer);
	}
}
BENCHMARK(c_malloc_free_performance_benchmark);


void cpp_new_delete_performance_benchmark(benchmark::State& state_p) noexcept
{
	for (auto _ : state_p)
	{
		std::byte* l_pointer = new std::byte[1024];
		benchmark::DoNotOptimize(l_pointer);
		delete[] l_pointer;
	}
}
BENCHMARK(cpp_new_delete_performance_benchmark);


void tbb_scalable_aligned_malloc_free_benchmark(benchmark::State& state_p) noexcept
{
	for (auto _ : state_p)
	{
		void* l_pointer = scalable_aligned_malloc(1024, 64);
		benchmark::DoNotOptimize(l_pointer);
		scalable_aligned_free(l_pointer);
	}
}
BENCHMARK(tbb_scalable_aligned_malloc_free_benchmark);