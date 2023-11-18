#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <FE/core/pool.hxx>
#include <FE/core/pool_allocator.hxx>
#include <FE/core/algorithm/string.hxx>
#include <tbb/memory_pool.h>
#include <FE/miscellaneous/undefine_max_min.h>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <FE/miscellaneous/define_max_min.h>

#include <string>




TEST(pool, block_allocation)
{
	FE::scoped_pool_resource<FE::block_pool<FE::var::int64, 32>> l_pool(1);

	FE::memory_region_t l_memory_region = "Restaurant";
	{

		FE::scoped_namespace_pool_resource<FE::block_pool<std::string>>l_pool_namespace(l_memory_region.data(), 1);

		FE::block_pool_ptr<std::string> l_smart_ptr1 = FE::block_pool<std::string>::allocate(l_memory_region.data());
		l_smart_ptr1->reserve(20);
		l_smart_ptr1->assign("Aglio e olio!");

		FE::block_pool_ptr<std::string> l_smart_ptr2 = FE::block_pool<std::string>::allocate(l_memory_region.data());
		l_smart_ptr2->reserve(20);
		l_smart_ptr2->assign("Pizza!");

		FE::block_pool_ptr<std::string> l_smart_ptr3 = FE::block_pool<std::string>::allocate(l_memory_region.data());
		l_smart_ptr3->reserve(20);
		l_smart_ptr3->assign("Vongole!");

		FE::block_pool_ptr<std::string> l_smart_ptr4 = FE::block_pool<std::string>::allocate(l_memory_region.data());
		l_smart_ptr4->reserve(20);
		l_smart_ptr4->assign("Carbonara!");

		FE::block_pool_ptr<std::string> l_smart_ptr5 = FE::block_pool<std::string>::allocate(l_memory_region.data());
		l_smart_ptr5->reserve(20);
		l_smart_ptr5->assign("Arrabbiata!");

	}

	{
		FE::block_pool_ptr<FE::var::int64, 32> l_smart_ptr[10];

		for (FE::var::uint32 i = 0; i < 10; ++i)
		{
			l_smart_ptr[i] = FE::block_pool<FE::var::int64, 32>::allocate();
		}
	}
}


TEST(pool, generic_block_allocation)
{
	FE::scoped_pool_resource<FE::generic_pool<1 KB>> l_pool_lifecycle(1);

	{
		FE::generic_pool_ptr<std::vector<FE::var::int32>, 1 KB> l_smart_ptr = FE::generic_pool<1 KB>::allocate<std::vector<FE::var::int32>>();
	}

	{
		FE::generic_pool_ptr<std::string, 1 KB> l_smart_ptr = FE::generic_pool<1 KB>::allocate<std::string>();
	}

	{
		FE::generic_pool_ptr<FE::var::int64, 1 KB> l_smart_ptr[8];

		for (FE::var::uint32 i = 0; i < 8; ++i)
		{
			l_smart_ptr[i] = FE::generic_pool<1 KB>::allocate<FE::var::int64>();
		}

		FE::generic_pool_ptr<FE::var::int64, 1 KB> l_another_smart_ptr;
		l_another_smart_ptr = std::move(l_smart_ptr[0]);
	}

	{
		FE::generic_pool_ptr<FE::var::int64, 1 KB> l_smart_ptr[4];

		for (FE::var::uint32 i = 0; i < 4; ++i)
		{
			l_smart_ptr[i] = FE::generic_pool<1 KB>::allocate<FE::var::int64>();
		}

		FE::generic_pool_ptr<FE::var::int64, 1 KB> l_another_smart_ptr = std::move(l_smart_ptr[0]);
	}


	FE::memory_region_t l_memory_region = "Restaurant";
	{
	
		FE::scoped_namespace_pool_resource<FE::generic_pool<1 KB>>l_pool_namespace(l_memory_region.data(), 1);

		FE::generic_pool_ptr<std::string, 1 KB> l_smart_ptr1 = FE::generic_pool<1 KB>::allocate<std::string>(l_memory_region.data(), 1);
		l_smart_ptr1->reserve(20);
		l_smart_ptr1->assign("Aglio e olio!");

		FE::generic_pool_ptr<std::string, 1 KB> l_smart_ptr2 = FE::generic_pool<1 KB>::allocate<std::string>(l_memory_region.data(), 1);
		l_smart_ptr2->reserve(20);
		l_smart_ptr2->assign("Pizza!");

		FE::generic_pool_ptr<std::string, 1 KB> l_smart_ptr3 = FE::generic_pool<1 KB>::allocate<std::string>(l_memory_region.data(), 1);
		l_smart_ptr3->reserve(20);
		l_smart_ptr3->assign("Vongole!");

		FE::generic_pool_ptr<std::string, 1 KB> l_smart_ptr4 = FE::generic_pool<1 KB>::allocate<std::string>(l_memory_region.data(), 1);
		l_smart_ptr4->reserve(20);
		l_smart_ptr4->assign("Carbonara!");

		FE::generic_pool_ptr<std::string, 1 KB> l_smart_ptr5 = FE::generic_pool<1 KB>::allocate<std::string>(l_memory_region.data(), 1);
		l_smart_ptr5->reserve(20);
		l_smart_ptr5->assign("Arrabbiata!");
	}
}




TEST(new_delete_pool_allocator, all)
{
	FE::scoped_pool_resource<FE::new_delete_pool_allocator<std::string, 1 KB>> l_pool_lifecycle(1);

	{
		auto l_ptr = FE::new_delete_pool_allocator<std::string, 1 KB>::allocate(1);

		l_ptr = FE::new_delete_pool_allocator<std::string, 1 KB>::reallocate(l_ptr, 1, 2);

		FE::new_delete_pool_allocator<std::string, 1 KB>::deallocate(l_ptr, 2);
	}

	{
		FE::std_style::new_delete_pool_allocator<std::string, FE::capacity<1 KB>> l_allocator;

		auto l_ptr = l_allocator.allocate(1);

		l_ptr = l_allocator.reallocate(l_ptr, 1, 2);

		l_allocator.deallocate(l_ptr, 2);
	}
}




TEST(pool_allocator, all)
{
	{
		std::vector<std::string, FE::std_style::pool_allocator<std::string, FE::capacity<4 KB>>> l_vector;
		l_vector.get_allocator().create_pages(1);

		l_vector.reserve(64);
		l_vector.shrink_to_fit();

		l_vector.get_allocator().shrink_to_fit();
	}
}


TEST(namespace_pool_allocator, all)
{
	FE::namespace_pool_allocator<int, FE::capacity<4 KB>> l_allocator("list node pool");
	l_allocator.create_pages(2);
	{
		std::list<int, FE::namespace_pool_allocator<int, FE::capacity<4 KB>>> l_list{ l_allocator };

		for (int i = 0; i < 10; ++i)
		{
			l_list.push_back(1);
		}
	}
	l_allocator.shrink_to_fit();
}


void memory_pooled_std_list_iteration(benchmark::State& state_p) noexcept
{
	FE::namespace_pool_allocator<int, FE::capacity<4 KB>> l_allocator("list node pool"); // std::list nodes are allocated under the memory pool namespace named "list node pool". This provides higher cache hit ratio. 
	benchmark::DoNotOptimize(l_allocator);

	l_allocator.create_pages(2);

	std::list<int, FE::namespace_pool_allocator<int, FE::capacity<4 KB>>> l_list{ l_allocator };
	benchmark::DoNotOptimize(l_list);

	for (int i = 0; i < 10; ++i)
	{
		l_list.push_back(1);
	}

	for (auto _ : state_p)
	{
		for (auto& element : l_list)
		{
			benchmark::DoNotOptimize(element);
		}
	}

	l_allocator.shrink_to_fit();
}
BENCHMARK(memory_pooled_std_list_iteration);


void std_list_iteration(benchmark::State& state_p) noexcept
{
	std::list<int> l_list;
	benchmark::DoNotOptimize(l_list);

	for (int i = 0; i < 10; ++i)
	{
		l_list.push_back(1);
	}

	for (auto _ : state_p)
	{
		for (auto& element : l_list)
		{
			benchmark::DoNotOptimize(element);
		}
	}
}
BENCHMARK(std_list_iteration);




#define _MAX_ITERATION_ 5000
void boost_pool_allocator_extreme_test(benchmark::State& state_p) noexcept
{
	static std::string* l_s_strings[_MAX_ITERATION_];
	boost::pool_allocator<std::string> l_allocator;

	for (auto _ : state_p)
	{
		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			l_s_strings[i] = boost::pool_allocator<std::string>::allocate(1);
		}

		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			boost::pool_allocator<std::string>::deallocate(l_s_strings[i], 1);
		}
	}
}
BENCHMARK(boost_pool_allocator_extreme_test);


void boost_object_pool_allocator_extreme_test(benchmark::State& state_p) noexcept
{
	static std::string* l_s_strings[_MAX_ITERATION_];
	benchmark::DoNotOptimize(l_s_strings);

	boost::object_pool<std::string> l_allocator;
	benchmark::DoNotOptimize(l_allocator);

	for (auto _ : state_p)
	{
		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			l_s_strings[i] = l_allocator.malloc();
		}

		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			l_allocator.free(l_s_strings[i]);
		}
	}
}
BENCHMARK(boost_object_pool_allocator_extreme_test);


void FE_pool_allocator_extreme_test(benchmark::State& state_p) noexcept
{
	static std::string* l_s_strings[_MAX_ITERATION_];
	benchmark::DoNotOptimize(l_s_strings);

	FE::scoped_pool_resource<FE::pool_allocator<std::string>> l_manager(2);
	benchmark::DoNotOptimize(l_manager);

	for (auto _ : state_p)
	{
		for(FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			l_s_strings[i] = FE::pool_allocator<std::string>::allocate(1);
		}

		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			FE::pool_allocator<std::string>::deallocate(l_s_strings[i], 1);
		}
	}
}
BENCHMARK(FE_pool_allocator_extreme_test);


void tbb_pool_allocator_extreme_test(benchmark::State& state_p) noexcept
{
	static std::string* l_s_strings[_MAX_ITERATION_];
	benchmark::DoNotOptimize(l_s_strings);

	tbb::memory_pool<tbb::scalable_allocator<std::string>> l_memory_pool;
	benchmark::DoNotOptimize(l_memory_pool);

	tbb::memory_pool_allocator<std::string> l_allocator(l_memory_pool);
	benchmark::DoNotOptimize(l_allocator);

	for (auto _ : state_p)
	{
		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			l_s_strings[i] = l_allocator.allocate(1);
		}

		for (FE::var::uint32 i = 0; i < _MAX_ITERATION_; ++i)
		{
			l_allocator.deallocate(l_s_strings[i], 1);
		}
	}
}
BENCHMARK(tbb_pool_allocator_extreme_test);
#undef _MAX_ITERATION_