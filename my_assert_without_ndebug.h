#ifndef ASSERT_WITHOUT_NDEBUG_H
#define ASSERT_WITHOUT_NDEBUG_H
#include <stdlib.h>
#include <stdio.h>

//! Checks if condition is correct, if it is incorrect outputs to stderr string, that is set in arguments
//!     also outputs to stderr name of file, function and line where mistake was found a
//!     Call the exit function with exit status EXIT_FAILURE
//!
//! @note Doesn't has NDEBUG

#define MY_ASSERT_WTHOUT_NDEBUG(condition)                                                                \
    if (condition == false)                                                                               \
    {                                                                                                     \
        fprintf(stderr, "Assertion failed\n");                                                            \
        fprintf(stderr, "Mistake in file: %s, function: %s, string: %d\n", __FILE__, __func__, __LINE__); \
        exit(EXIT_FAILURE);                                                                               \
    }

#endif // ASSERT_WITHOUT_NDEBUG_H