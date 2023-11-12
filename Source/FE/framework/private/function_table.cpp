#include <FE/framework/reflection/function_table.hpp>
#define _FUNCTION_TABLE_DEFUALT_SIZE_ 1024
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.

thread_local FE::framework::function_table::underlying_container FE::framework::function_table::tl_s_task_map;