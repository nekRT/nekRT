// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "bcs/boundary.h"

#include <gtest/gtest.h>

#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/enums.h"
#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {
namespace {

// Two-element mesh with six boundary elements. The first three boundary
// elements have attribute 1 and the rest have attribute 2, like a GMSH mesh
// with two physical curves.
mfem::Mesh TwoAttributeMesh() {
  mfem::Mesh mesh =
      mfem::Mesh::MakeCartesian2D(2, 1, mfem::Element::QUADRILATERAL);
  for (int b = 0; b < mesh.GetNBE(); b++) {
    mesh.SetBdrAttribute(b, b < 3 ? 1 : 2);
  }
  mesh.SetAttributes();
  return mesh;
}

void AddGroup(mfem::Mesh& mesh, const std::string& name, int attribute) {
  mesh.bdr_attribute_sets.CreateAttributeSet(name);
  mesh.bdr_attribute_sets.AddToAttributeSet(name, attribute);
}

TEST(BuildBoundariesTest, KeepsOrderAndType) {
  const std::vector<BoundaryParams> params = {
      {"outer_wall", BoundaryType::Vacuum},
      {"reflective_wall", BoundaryType::Reflective}};

  const std::vector<Boundary> boundaries = BuildBoundaries(params);

  ASSERT_EQ(boundaries.size(), 2);
  EXPECT_EQ(boundaries[0].type, BoundaryType::Vacuum);
  EXPECT_EQ(boundaries[1].type, BoundaryType::Reflective);
}

TEST(CreateBoundaryIDMapTest, MapsNameToIndex) {
  const std::vector<BoundaryParams> params = {
      {"outer_wall", BoundaryType::Vacuum},
      {"reflective_wall", BoundaryType::Reflective}};

  const auto id_map = CreateBoundaryIDMap(params);

  ASSERT_EQ(id_map.size(), 2);
  EXPECT_EQ(id_map.at("outer_wall"), 0);
  EXPECT_EQ(id_map.at("reflective_wall"), 1);
}

TEST(AttributeToBoundaryIndexTest, MapsEachAttributeToItsBoundary) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 1);
  AddGroup(mesh, "boundary:reflective_wall", 2);
  const std::unordered_map<std::string, int> boundary_ids = {
      {"outer_wall", 0}, {"reflective_wall", 1}};

  const mfem::Array<int> table = AttributeToBoundaryIndex(boundary_ids, mesh);

  ASSERT_EQ(table.Size(), 3);
  EXPECT_EQ(table[0], -1);  // Attribute 0 is never used.
  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[2], 1);
}

TEST(AttributeToBoundaryIndexTest, UnclaimedAttributeStaysMinusOne) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 1);
  const std::unordered_map<std::string, int> boundary_ids = {{"outer_wall", 0}};

  const mfem::Array<int> table = AttributeToBoundaryIndex(boundary_ids, mesh);

  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[2], -1);
}

TEST(AttributeToBoundaryIndexTest, MissingGroupThrows) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 1);
  const std::unordered_map<std::string, int> boundary_ids = {
      {"outer_wall", 0}, {"reflective_wall", 1}};

  EXPECT_THROW(AttributeToBoundaryIndex(boundary_ids, mesh), Error);
}

TEST(AttributeToBoundaryIndexTest, SharedAttributeThrows) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 1);
  AddGroup(mesh, "boundary:reflective_wall", 1);
  const std::unordered_map<std::string, int> boundary_ids = {
      {"outer_wall", 0}, {"reflective_wall", 1}};

  EXPECT_THROW(AttributeToBoundaryIndex(boundary_ids, mesh), Error);
}

TEST(AttributeToBoundaryIndexTest, AttributeNotInMeshThrows) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 7);
  const std::unordered_map<std::string, int> boundary_ids = {{"outer_wall", 0}};

  EXPECT_THROW(AttributeToBoundaryIndex(boundary_ids, mesh), Error);
}

TEST(CreateElementBoundariesTest, MapsEachBoundaryElement) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 1);
  AddGroup(mesh, "boundary:reflective_wall", 2);
  const std::unordered_map<std::string, int> boundary_ids = {
      {"outer_wall", 0}, {"reflective_wall", 1}};

  const mfem::Array<int> element_boundaries =
      CreateElementBoundaries(boundary_ids, mesh);

  ASSERT_EQ(element_boundaries.Size(), mesh.GetNBE());
  for (int b = 0; b < mesh.GetNBE(); b++) {
    EXPECT_EQ(element_boundaries[b], b < 3 ? 0 : 1) << "boundary element " << b;
  }
}

TEST(CreateElementBoundariesTest, UnlistedAttributeThrows) {
  mfem::Mesh mesh = TwoAttributeMesh();
  AddGroup(mesh, "boundary:outer_wall", 1);
  AddGroup(mesh, "boundary:reflective_wall", 2);
  // Only one of the two physical groups is listed in the input.
  const std::unordered_map<std::string, int> boundary_ids = {{"outer_wall", 0}};

  EXPECT_THROW(CreateElementBoundaries(boundary_ids, mesh), Error);
}

}  // namespace
}  // namespace nekrt
