#include <FE/framework/reflection/function_table.hpp>
#define _FUNCTION_TABLE_DEFUALT_SIZE_ 1024
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.

tbb::memory_pool<FE::cache_aligned_allocator<std::pair<const FE::framework::method_signature_t, FE::task_base*>>> FE::framework::function_table::s_memory_pool;

FE::framework::function_table::underlying_container FE::framework::function_table::s_task_map{ tbb::memory_pool_allocator<std::pair<const method_signature_t, FE::task_base*>>{s_memory_pool} };

std::mutex FE::framework::function_table::s_mutex;