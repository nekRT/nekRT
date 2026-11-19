// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "regions/region.h"

#include <gtest/gtest.h>

#include <mfem.hpp>
#include <string>
#include <unordered_map>

#include "utils/errors.h"

namespace nekrt {
namespace {

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

TEST(AttributeToRegionIndexTest, MapsEachAttributeToItsRegion) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:moderator", 2);
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0},
                                                           {"moderator", 1}};

  const mfem::Array<int> table = AttributeToRegionIndex(region_ids, mesh);

  ASSERT_EQ(table.Size(), 3);
  EXPECT_EQ(table[0], -1);  // Attribute 0 is never used.
  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[2], 1);
}

TEST(AttributeToRegionIndexTest, UnclaimedAttributeStaysMinusOne) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0}};

  const mfem::Array<int> table = AttributeToRegionIndex(region_ids, mesh);

  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[2], -1);
}

TEST(AttributeToRegionIndexTest, MissingGroupThrows) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0},
                                                           {"moderator", 1}};

  EXPECT_THROW(AttributeToRegionIndex(region_ids, mesh), Error);
}

TEST(AttributeToRegionIndexTest, SharedAttributeThrows) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:moderator", 1);
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0},
                                                           {"moderator", 1}};

  EXPECT_THROW(AttributeToRegionIndex(region_ids, mesh), Error);
}

TEST(AttributeToRegionIndexTest, AttributeNotInMeshThrows) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 7);
  const std::unordered_map<std::string, int> region_ids = {{"fuel", 0}};

  EXPECT_THROW(AttributeToRegionIndex(region_ids, mesh), Error);
}

}  // namespace
}  // namespace nekrt
