﻿#ifndef _FE_CORE_DEBUG_H_
#define _FE_CORE_DEBUG_H_
#include <FE/miscellaneous/configuration.h>
#include <iostream>
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.


#if _ENABLE_ABORT_IF_ == true
// It aborts if the expression is true
#define ABORT_IF(expression, message) if (expression) { std::cerr << #expression << " is a false expression.\nAbort has been called from\ndirectory: " << __FILE__ << "\nfunction: " << __func__ << "()\nline number: " << __LINE__ << "\nmessage: " << #message; ::std::abort(); }
#else
// It aborts if the expression is true
#define ABORT_IF(expression, message)
#endif


#if _ENABLE_ASSERT_ == true || _ENABLE_LOG_ == true || _ENABLE_EXIT_ == true
#define IF_DEBUG_MODE(true_p, false_p) true_p
#define ENABLE_IF_DEBUG_MODE(code_p) code_p
#else
#define IF_DEBUG_MODE(true_p, false_p) false_p
#define ENABLE_IF_DEBUG_MODE(code_p)
#endif

#endif
