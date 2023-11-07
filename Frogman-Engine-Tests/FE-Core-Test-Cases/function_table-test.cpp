#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <FE/framework/reflection/function_table.hpp>
#include <FE/core/function.hxx>




FE::boolean Jesus_loves_you() noexcept
{
	return true;
}
FE::c_style_task<FE::boolean(void)> g_greeting = FE::function<FE::boolean(void)>{ Jesus_loves_you };




// https://learn.microsoft.com/en-us/cpp/build/reference/gf-eliminate-duplicate-strings?view=msvc-170
TEST(function_table, general)
{
	FE::framework::function_table::register_method(FE::framework::function_table::underlying_container::value_type{ "FE::boolean Jesus_loves_you() noexcept", &g_greeting });
	EXPECT_TRUE(FE::framework::function_table::check_presence("FE::boolean Jesus_loves_you() noexcept"));

	FE::framework::function_table::invoke("FE::boolean Jesus_loves_you() noexcept");
	EXPECT_TRUE(g_greeting._task_result);

	auto l_fn = FE::framework::function_table::retrieve("FE::boolean Jesus_loves_you() noexcept");

	g_greeting._task_result = false;
	(*l_fn)();
	EXPECT_TRUE(g_greeting._task_result);
}




void FE_function_table_invocation_overhead_benchmark(benchmark::State& state_p) noexcept
{
	FE_ASSERT(FE::framework::function_table::check_presence("FE::boolean Jesus_loves_you() noexcept") == false, "Could not find the target function.");

	for(auto _ : state_p)
	{
		FE::framework::function_table::invoke("FE::boolean Jesus_loves_you() noexcept");
	}
}

BENCHMARK(FE_function_table_invocation_overhead_benchmark);