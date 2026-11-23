#include "materials/material_bank.h"

#include <gtest/gtest.h>

namespace nekrt {
TEST(MaterialBankTest, BuildMaterialsCopiesCrossSections) {
  MaterialParams params;
  params.name = "fuel";
  params.total_xs = {0.75, 1.5};
  params.scatter_moments = {0.1, 0.2, 0.3, 0.4};

  MaterialBank material_bank({params});

  ASSERT_EQ(material_bank.materials().size(), 1);
  EXPECT_EQ(material_bank.GetByID(0).total_xs, params.total_xs);
  EXPECT_EQ(material_bank.GetByID(0).scatter_moments, params.scatter_moments);
}

// n_legendre_moments is the number of G x G blocks in scatter_moments.
TEST(MaterialBankTest, BuildMaterialsCountsLegendreMoments) {
  MaterialParams one_group;  // 1 group, 3 moments.
  one_group.total_xs = {1.0};
  one_group.scatter_moments = {0.5, 0.1, 0.01};
  MaterialParams two_groups;  // 2 groups, 1 moment.
  two_groups.total_xs = {1.0, 2.0};
  two_groups.scatter_moments = {0.1, 0.2, 0.3, 0.4};

  MaterialBank material_bank({one_group, two_groups});

  ASSERT_EQ(material_bank.materials().size(), 2);
  EXPECT_EQ(material_bank.GetByID(0).n_legendre_moments, 3);
  EXPECT_EQ(material_bank.GetByID(1).n_legendre_moments, 1);
}
}  // namespace nekrt
