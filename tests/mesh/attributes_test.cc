// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "mesh/attributes.h"

#include <gtest/gtest.h>

#include <mfem.hpp>
#include <string>
#include <unordered_map>

#include "utils/errors.h"

namespace nekrt {
namespace {

// Two-element mesh whose elements have attributes 1 and 3, like a GMSH mesh
// with two physical surfaces. Attribute 2 is skipped so that a gap in the
// attribute numbers is covered.
mfem::Mesh TwoElementMesh() {
  mfem::Mesh mesh =
      mfem::Mesh::MakeCartesian2D(2, 1, mfem::Element::QUADRILATERAL);
  mesh.SetAttribute(0, 1);
  mesh.SetAttribute(1, 3);
  mesh.SetAttributes();
  return mesh;
}

void AddGroup(mfem::Mesh& mesh, const std::string& name, int attribute) {
  if (!mesh.attribute_sets.AttributeSetExists(name)) {
    mesh.attribute_sets.CreateAttributeSet(name);
  }
  mesh.attribute_sets.AddToAttributeSet(name, attribute);
}

mfem::Array<int> RegionTable(
    const mfem::Mesh& mesh,
    const std::unordered_map<std::string, int>& name_to_index) {
  return AttributeToIndex(mesh.attribute_sets, mesh.attributes, "region",
                          name_to_index);
}

// Returns the message of the Error thrown by RegionTable(), or fails.
std::string ErrorMessage(
    const mfem::Mesh& mesh,
    const std::unordered_map<std::string, int>& name_to_index) {
  try {
    RegionTable(mesh, name_to_index);
  } catch (const Error& e) {
    return e.what();
  }
  ADD_FAILURE() << "expected an Error";
  return "";
}

TEST(AttributeToIndexTest, MapsEachAttributeToItsIndex) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:moderator", 3);

  const mfem::Array<int> table =
      RegionTable(mesh, {{"fuel", 0}, {"moderator", 1}});

  ASSERT_EQ(table.Size(), 4);
  EXPECT_EQ(table[0], -1);  // Attribute 0 is never used.
  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[2], -1);  // Not used by this mesh.
  EXPECT_EQ(table[3], 1);
}

TEST(AttributeToIndexTest, GroupWithSeveralAttributes) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:fuel", 3);

  const mfem::Array<int> table = RegionTable(mesh, {{"fuel", 0}});

  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[3], 0);
}

TEST(AttributeToIndexTest, UnclaimedAttributeStaysMinusOne) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);

  const mfem::Array<int> table = RegionTable(mesh, {{"fuel", 0}});

  EXPECT_EQ(table[1], 0);
  EXPECT_EQ(table[3], -1);
}

TEST(AttributeToIndexTest, MissingGroupThrows) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);

  const std::string what = ErrorMessage(mesh, {{"fuel", 0}, {"moderator", 1}});

  EXPECT_NE(what.find("'region:moderator' not found"), std::string::npos)
      << what;
}

TEST(AttributeToIndexTest, SharedAttributeThrows) {
  mfem::Mesh mesh = TwoElementMesh();
  AddGroup(mesh, "region:fuel", 1);
  AddGroup(mesh, "region:moderator", 1);

  const std::string what = ErrorMessage(mesh, {{"fuel", 0}, {"moderator", 1}});

  EXPECT_NE(what.find("shares attribute 1 with another region"),
            std::string::npos)
      << what;
}

TEST(AttributeToIndexTest, AttributeNotInMeshThrows) {
  for (const int attribute : {2, 7}) {  // In the gap, and above the maximum.
    mfem::Mesh mesh = TwoElementMesh();
    AddGroup(mesh, "region:fuel", attribute);

    const std::string what = ErrorMessage(mesh, {{"fuel", 0}});

    EXPECT_NE(what.find("attribute " + std::to_string(attribute) +
                        ", which the mesh does not use"),
              std::string::npos)
        << what;
  }
}

}  // namespace
}  // namespace nekrt
