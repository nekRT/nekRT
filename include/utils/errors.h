// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_UTILS_ERRORS_H_
#define NEKRT_UTILS_ERRORS_H_

#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>

// NOTE(parallel): an Error thrown on one MPI rank can leave the other ranks
// waiting in a collective call. Revisit this when MPI is added.

namespace nekrt {

/**
 * @brief The exceptions thrown in nekRT. Includes commonly-used
 * NekRTError() and NEKRT_ASSERT()
 *
 */
class Error : public std::runtime_error {
 public:
  /** @param message The full message, including the source location. */
  explicit Error(const std::string& message) : std::runtime_error(message) {}
};

/**
 * Throws an Error if `condition` is true.
 *
 * Use this to check user input and other conditions that can fail in a
 * correct program. It is always active. Format the message with
 * `fmt::format` if it needs values. Do not call it in hot loops.
 *
 * @param condition The error condition: the Error is thrown when it is true.
 * @param message Describes what went wrong and, if possible, how to fix it.
 * @param location The call site, filled in by the compiler.
 * @throws Error If `condition` is true. `what()` is
 *   `<file>:<line>: <message>`.
 */
void NekRTError(
    bool condition, std::string_view message,
    std::source_location location = std::source_location::current());

/**
 * Throws an Error when called.
 *
 * Use this to check user input and other conditions that can fail in a
 * correct program. It is always active. Format the message with
 * `fmt::format` if it needs values. Do not call it in hot loops.
 *
 * @param message Describes what went wrong and, if possible, how to fix it.
 * @param location The call site, filled in by the compiler.
 * @throws Error Always. `what()` is `<file>:<line>: <message>`.
 */
[[noreturn]] void NekRTError(
    std::string_view message,
    std::source_location location = std::source_location::current());

namespace detail {

/** Throws the Error for a failed NEKRT_ASSERT(). */
[[noreturn]] void AssertFailed(const char* condition, std::string_view message,
                               const char* file, int line);

}  // namespace detail
}  // namespace nekrt

/**
 * @def NEKRT_ASSERT(condition, message)
 * Throws an Error if `condition` is false.
 *
 * Use this for internal invariants (programmer mistakes), not for user input.
 * It is active only when NDEBUG is not defined. When NDEBUG is defined,
 * `condition` and `message` are not evaluated.
 */
#ifndef NDEBUG
#define NEKRT_ASSERT(condition, message)                             \
  do {                                                               \
    if (!(condition)) {                                              \
      ::nekrt::detail::AssertFailed(#condition, (message), __FILE__, \
                                    __LINE__);                       \
    }                                                                \
  } while (false)
#else
#define NEKRT_ASSERT(condition, message) \
  do {                                   \
    (void)sizeof(condition);             \
    (void)sizeof(message);               \
  } while (false)
#endif

#endif  // NEKRT_UTILS_ERRORS_H_
