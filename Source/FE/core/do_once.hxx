﻿#ifndef _FE_CORE_DO_ONCE_HXX_
#define _FE_CORE_DO_ONCE_HXX_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include "prerequisite_symbols.h"
#include <functional>




BEGIN_NAMESPACE(FE)


template <typename F>
struct do_once
{
	do_once(F lambda_p) noexcept
	{
		lambda_p();
	}
};

END_NAMESPACE
#endif