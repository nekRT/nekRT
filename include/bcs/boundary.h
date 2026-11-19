// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_BCS_BOUNDARY_H_
#define NEKRT_BCS_BOUNDARY_H_

#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/enums.h"
#include "utils/input_parameters.h"

namespace nekrt {

/**
 * @brief A boundary holds the boundary condition applied to a group of boundary
 * elements.
 *
 */
struct Boundary {
  /// @brief Boundary condition type.
  BoundaryType type = BoundaryType::Vacuum;
};

/**
 * @brief Constructs the Boundary objects used in the simulation.
 *
 * @param boundary_params User-set boundary parameters.
 * @return Boundaries in the same order as `boundary_params`, so the index of a
 *   boundary is its ID.
 */
std::vector<Boundary> BuildBoundaries(
    const std::vector<BoundaryParams>& boundary_params);

/**
 * @brief Creates an array mapping each boundary element to the ID of its
 * boundary.
 *
 * Each boundary is looked up in the mesh as the physical group
 * `boundary:<name>`. For a boundary element `b`, its Boundary is
 * `boundaries[element_boundaries[b]]`.
 *
 * @param boundary_id_map Boundary name to boundary index (see
 *   CreateNameIndexMap()).
 * @param mesh Mesh loaded from a GMSH file.
 * @return Boundary index of each boundary element, of size `mesh.GetNBE()`.
 * @throws Error If AttributeToIndex() fails, or a boundary element has an
 *   attribute that no boundary claims.
 */
mfem::Array<int> CreateElementBoundaries(
    const std::unordered_map<std::string, int>& boundary_id_map,
    const mfem::Mesh& mesh);

}  // namespace nekrt

#endif  // NEKRT_BCS_BOUNDARY_H_
