// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "bcs/boundary.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "mesh/attributes.h"
#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {

std::vector<Boundary> BuildBoundaries(
    const std::vector<BoundaryParams>& boundary_params) {
  std::vector<Boundary> boundaries;
  boundaries.reserve(boundary_params.size());
  for (const BoundaryParams& params : boundary_params) {
    boundaries.push_back({.type = params.type});
  }
  return boundaries;
}

mfem::Array<int> CreateElementBoundaries(
    const std::unordered_map<std::string, int>& boundary_id_map,
    const mfem::Mesh& mesh) {
  const mfem::Array<int> attribute_to_boundary =
      AttributeToIndex(mesh.bdr_attribute_sets, mesh.bdr_attributes, "boundary",
                       boundary_id_map);
  mfem::Array<int> element_boundaries(mesh.GetNBE());
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
