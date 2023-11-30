#ifndef _FE_CORE_DEBUG_H_
#define _FE_CORE_DEBUG_H_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <iostream>
#include <FE/miscellaneous/configuration.h>
#include <FE/core/macros/attributes.h>




#ifdef FE_STATIC_ASSERT
#error FE_STATIC_ASSERT is a reserved Frogman Engine macro keyword.
#endif 
#define FE_STATIC_ASSERT(expression, message) static_assert(!(expression), message)


#ifdef _ENABLE_FE_CHECK_ONCE_
namespace FE::internal
{
    struct check_once
    {
        check_once(const bool did_it_fail_p) noexcept
        {
            if (did_it_fail_p == false) _LIKELY_
            {
                return;
            }

            abort();
        }
    };
}
#ifdef _FN_LEVEL_GLOBAL_
#error _FN_LEVEL_GLOBAL_ is a reserved Frogman Engine macro keyword.
#endif 
// A directive for checking the expression once at function global level.
// If the same function gets called inside a loop or recursively, the expression_p can only be evaluated once.
#define _FN_LEVEL_GLOBAL_ static

#ifdef _FN_LEVEL_THREAD_LOCAL_
#error _FN_LEVEL_THREAD_LOCAL_ is a reserved Frogman Engine macro keyword.
#endif 
// A directive for checking the expression once per thread.
// Verification can be performed once per thread.
#define _FN_LEVEL_THREAD_LOCAL_ thread_local static 

#ifdef FE_CHECK_ONCE
#error FE_CHECK_ONCE is a reserved Frogman Engine macro keyword.
#endif 
// It invokes abort() if the expression_p is true
#define FE_CHECK_ONCE(scope_p, expression_p, verification_case_name_p) { scope_p ::FE::internal::check_once scope_p##verification_case_name_p(expression_p); }


#else
#ifdef FE_CHECK_ONCE
#error FE_CHECK_ONCE is a reserved Frogman Engine macro keyword.
#endif 
// It invokes abort() if the expression_p is true
#define FE_CHECK_ONCE(scope_p, expression_p, verification_case_name_p)
#endif




#ifdef ABORT_IF
#error ABORT_IF is a reserved Frogman Engine macro keyword.
#endif 
#ifdef _ENABLE_ABORT_IF_
    #if defined(_RELEASE_) || defined(_RELWITHDEBINFO_)
         // It invokes abort() if the expression_p is true
        #define ABORT_IF(expression, message) if (expression) _UNLIKELY_ { ::std::abort(); }
    #else
        // It invokes abort() if the expression_p is true
        #define ABORT_IF(expression, message) if (expression) _UNLIKELY_ { std::cerr << "Expected the expression \"" << #expression << "\" to be false.\nAbort has been called from\ndirectory: " << __FILE__ << "\nfunction: " << __func__ << "()\nline number: " << __LINE__ << "\nmessage: " << #message; ::std::abort(); }
    #endif
#else
    // It invokes abort() if the expression_p is true
    #define ABORT_IF(expression, message)
#endif


#endif