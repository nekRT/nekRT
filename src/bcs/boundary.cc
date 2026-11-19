// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "bcs/boundary.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {

std::vector<Boundary> BuildBoundaries(
    const std::vector<BoundaryParams>& boundary_params) {
  std::vector<Boundary> boundaries;
  boundaries.reserve(boundary_params.size());
  for (const auto& params : boundary_params) {
    Boundary boundary;
    boundary.type = params.type;
    boundaries.push_back(std::move(boundary));
  }
  return boundaries;
}

std::unordered_map<std::string, int> CreateBoundaryIDMap(
    const std::vector<BoundaryParams>& boundary_params) {
  std::unordered_map<std::string, int> boundary_id_map;

  int count = 0;
  for (const auto& params : boundary_params) {
    boundary_id_map[params.name] = count;
    count++;
  }
  return boundary_id_map;
}

mfem::Array<int> AttributeToBoundaryIndex(
    const std::unordered_map<std::string, int>& boundary_id_map,
    const mfem::Mesh& mesh) {
  mfem::Array<int> attribute_to_boundary(mesh.bdr_attributes.Max() + 1);
  attribute_to_boundary = -1;

  for (const auto& [name, id] : boundary_id_map) {
    const std::string group_name = "boundary:" + name;
    if (!mesh.bdr_attribute_sets.AttributeSetExists(group_name)) {
      NekRTError(group_name + " not found in mesh.");
    }
    const auto& attribute_set =
        mesh.bdr_attribute_sets.GetAttributeSet(group_name);
    for (const int attribute : attribute_set) {
      if (attribute < 0 || attribute >= attribute_to_boundary.Size()) {
        NekRTError(group_name + " has attribute " + std::to_string(attribute) +
                   ", which no boundary element in the mesh uses.");
      }
      if (attribute_to_boundary[attribute] != -1) {
        NekRTError(group_name + " shares attribute " +
                   std::to_string(attribute) + " with another boundary.");
      }
      attribute_to_boundary[attribute] = id;
    }
  }
  return attribute_to_boundary;
}

mfem::Array<int> CreateElementBoundaries(
    const std::unordered_map<std::string, int>& boundary_id_map,
    const mfem::Mesh& mesh) {
  const auto attribute_to_boundary =
      AttributeToBoundaryIndex(boundary_id_map, mesh);
  mfem::Array<int> element_boundaries(mesh.GetNBE());
  element_boundaries = -1;
  for (int b = 0; b < mesh.GetNBE(); b++) {
    const int attribute = mesh.GetBdrAttribute(b);
    element_boundaries[b] = attribute_to_boundary[attribute];
    if (element_boundaries[b] == -1) {
      NekRTError("Boundary element " + std::to_string(b) + " has attribute " +
                 std::to_string(attribute) +
                 ", which is not in any boundary_conditions entry.");
    }
  }
  return element_boundaries;
}

}  // namespace nekrt
