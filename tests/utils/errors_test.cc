// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "utils/errors.h"

#include <gtest/gtest.h>

#include <string>

namespace nekrt {
namespace {

TEST(Errors, NekRTErrorFalseDoesNotThrow) {
  EXPECT_NO_THROW(NekRTError(false, "unused"));
}

TEST(Errors, NekRTErrorTrueThrows) {
  EXPECT_THROW(NekRTError(true, "bad input"), Error);
}

TEST(Errors, NekRTErrorMessageHasTextAndLocation) {
  try {
    NekRTError(true, "bad input");
    FAIL() << "expected an Error";
  } catch (const Error& e) {
    const std::string what = e.what();
    EXPECT_NE(what.find("bad input"), std::string::npos);
    EXPECT_NE(what.find("errors_test.cc"), std::string::npos);
  }
}

TEST(Errors, AssertTrueDoesNotThrow) {
  EXPECT_NO_THROW(NEKRT_ASSERT(1 + 1 == 2, "arithmetic"));
}

#ifndef NDEBUG
TEST(Errors, AssertFalseThrowsInDebug) {
  try {
    NEKRT_ASSERT(1 + 1 == 3, "arithmetic");
    FAIL() << "expected an Error";
  } catch (const Error& e) {
    const std::string what = e.what();
    EXPECT_NE(what.find("1 + 1 == 3"), std::string::npos);
    EXPECT_NE(what.find("arithmetic"), std::string::npos);
  }
}
#else
TEST(Errors, AssertIsCompiledOutWithNdebug) {
  int evaluations = 0;
  EXPECT_NO_THROW(NEKRT_ASSERT(++evaluations < 0, "unused"));
  EXPECT_EQ(evaluations, 0);
}
#endif

}  // namespace
}  // namespace nekrt
