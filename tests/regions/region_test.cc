// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "regions/region.h"

#include <gtest/gtest.h>

#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {
namespace {

// The edge cases of the attribute lookup are tested on AttributeToIndex() in
// tests/mesh/attributes_test.cc.

// Two-element mesh whose elements have attributes 1 and 2, like a GMSH mesh
// with two physical surfaces.
mfem::Mesh TwoElementMesh() {
  mfem::Mesh mesh =
      mfem::Mesh::MakeCartesian2D(2, 1, mfem::Element::QUADRILATERAL);
  mesh.SetAttribute(0, 1);
  mesh.SetAttribute(1, 2);
  mesh.SetAttributes();
  return mesh;
}

void AddGroup(mfem::Mesh& mesh, const std::string& name, int attribute) {
  mesh.attribute_sets.CreateAttributeSet(name);
  mesh.attribute_sets.AddToAttributeSet(name, attribute);
}

const std::unordered_map<std::string, int> kMaterialIds = {{"fuel", 0},
                                                           {"water", 1}};
const std::unordered_map<std::string, int> kSourceIds = {{"core", 0}};

TEST(BuildRegionsTest, ResolvesMaterialAndSourceIds) {
  const std::vector<RegionParams> params = {{"fuel_region", "fuel", "core"},
                                            {"water_region", "water", "core"}};

  const std::vector<Region> regions =
      BuildRegions(params, kMaterialIds, kSourceIds);

  ASSERT_EQ(regions.size(), 2);
  EXPECT_EQ(regions[0].material_id, 0);
  EXPECT_EQ(regions[0].source_id, 0);
  EXPECT_EQ(regions[1].material_id, 1);
  EXPECT_EQ(regions[1].source_id, 0);
}

TEST(BuildRegionsTest, OmittedSourceIsMinusOne) {
  const std::vector<RegionParams> params = {{"water_region", "water", ""}};

  const std::vector<Region> regions =
      BuildRegions(params, kMaterialIds, kSourceIds);

  ASSERT_EQ(regions.size(), 1);
  EXPECT_EQ(regions[0].material_id, 1);
  EXPECT_EQ(regions[0].source_id, -1);
}

TEST(BuildRegionsTest, UnknownMaterialThrows) {
  const std::vector<RegionParams> params = {{"r", "steel", ""}};
  EXPECT_THROW(BuildRegions(params, kMaterialIds, kSourceIds), Error);
}

// A misspelled source must not silently give a source-free region.
TEST(BuildRegionsTest, UnknownSourceThrows) {
  const std::vector<RegionParams> params = {{"r", "fuel", "cor"}};
  EXPECT_THROW(BuildRegions(params, kMaterialIds, kSourceIds), Error);
}

TEST(CreateElementRegionsTest, MapsEachElement) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:moderator", 2);
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0},
                                                           {"moderator", 1}};

  const mfem::Array<int> element_regions =
      CreateElementRegions(region_ids, mesh);

  ASSERT_EQ(element_regions.Size(), 2);
  EXPECT_EQ(element_regions[0], 0);
  EXPECT_EQ(element_regions[1], 1);
}

TEST(CreateElementRegionsTest, UnlistedAttributeThrows) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:moderator", 2);
  // Only one of the two physical groups is listed in the input.
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0}};

  EXPECT_THROW(CreateElementRegions(region_ids, mesh), Error);
}

}  // namespace
}  // namespace nekrt
