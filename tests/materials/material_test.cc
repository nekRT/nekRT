// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, Liam Pohlmann
// Copyright (c) 2026, nekRT

#include "materials/material.h"

#include <gtest/gtest.h>

#include <numeric>

#include "../tests/constants.h"

namespace nekrt {
namespace {

// Builds a material whose scatter_moments[i] == i, so every value equals its
// flat index.
Material MakeIndexedMaterial(const int n_moments, const int n_groups) {
  Material material;
  material.n_legendre_moments = n_moments;
  material.scatter_moments.resize(n_moments * n_groups * n_groups);
  std::iota(material.scatter_moments.begin(), material.scatter_moments.end(),
            0.0);
  return material;
}

}  // namespace

TEST(MaterialsMaterial, GetScatterMomentSingleGroup) {
  Material material;
  material.n_legendre_moments = 1;
  material.scatter_moments = {3.5};
  EXPECT_NEAR(GetScatterMoment(0, 0, 0, material, 1), 3.5, kExpNearTolerance);
}

TEST(MaterialsMaterial, GetScatterMomentIndexOrder) {
  constexpr int kGroups = 3;
  const Material material = MakeIndexedMaterial(2, kGroups);
  // The "to" group varies fastest.
  EXPECT_NEAR(GetScatterMoment(0, 0, 0, material, kGroups), 0.0,
              kExpNearTolerance);
  EXPECT_NEAR(GetScatterMoment(0, 0, 2, material, kGroups), 2.0,
              kExpNearTolerance);
  // The "from" group has a stride of n_groups.
  EXPECT_NEAR(GetScatterMoment(0, 1, 0, material, kGroups), 3.0,
              kExpNearTolerance);
  // The moment number has a stride of n_groups^2.
  EXPECT_NEAR(GetScatterMoment(1, 0, 0, material, kGroups), 9.0,
              kExpNearTolerance);
  EXPECT_NEAR(GetScatterMoment(1, 2, 1, material, kGroups), 16.0,
              kExpNearTolerance);
}

TEST(MaterialsMaterial, GetScatterMomentAllEntries) {
  constexpr int kGroups = 3;
  constexpr int kMoments = 2;
  const Material material = MakeIndexedMaterial(kMoments, kGroups);
  for (int m = 0; m < kMoments; ++m) {
    for (int from = 0; from < kGroups; ++from) {
      for (int to = 0; to < kGroups; ++to) {
        EXPECT_NEAR(GetScatterMoment(m, from, to, material, kGroups),
                    m * kGroups * kGroups + from * kGroups + to,
                    kExpNearTolerance)
            << "m=" << m << " from=" << from << " to=" << to;
      }
    }
  }
}

TEST(MaterialsMaterial, GetScatterMomentDistinguishesFromAndTo) {
  Material material;
  material.n_legendre_moments = 1;
  material.scatter_moments = {0.1, 0.2, 0.3, 0.4};
  EXPECT_NEAR(GetScatterMoment(0, 0, 1, material, 2), 0.2, kExpNearTolerance);
  EXPECT_NEAR(GetScatterMoment(0, 1, 0, material, 2), 0.3, kExpNearTolerance);
}

}  // namespace nekrt
