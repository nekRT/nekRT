// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "sources/source.h"

#include <gtest/gtest.h>

#include <vector>

#include "utils/enums.h"
#include "utils/input_parameters.h"

namespace nekrt {
namespace {

TEST(BuildSourcesTest, KeepsOrderTypeAndStrengths) {
  const std::vector<SourceParams> params = {
      {"core", SourceType::Volumetric, {0.25, 0.0}},
      {"edge", SourceType::Volumetric, {1.0, 2.0}}};

  const std::vector<Source> sources = BuildSources(params);

  ASSERT_EQ(sources.size(), 2);
  EXPECT_EQ(sources[0].type, SourceType::Volumetric);
  EXPECT_EQ(sources[0].strengths, (std::vector<double>{0.25, 0.0}));
  EXPECT_EQ(sources[1].strengths, (std::vector<double>{1.0, 2.0}));
}

}  // namespace
}  // namespace nekrt
