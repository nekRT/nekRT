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
  /// @brief Boundary condition type
  BoundaryType type = BoundaryType::Vacuum;
};

/**
 * @brief Construct the Boundary objects used in the simulation.
 *
 * @param boundary_params User-set boundary parameters
 * @return std::vector<Boundary> In the same order as `boundary_params`, so the
 *   index of a boundary is its ID.
 */
std::vector<Boundary> BuildBoundaries(
    const std::vector<BoundaryParams>& boundary_params);

/**
 * @brief Creates a map from the boundary name to its ID.
 *
 * @param boundary_params User-set boundary parameters
 * @return Map from a boundary name to its index in `boundary_params`.
 */
std::unordered_map<std::string, int> CreateBoundaryIDMap(
    const std::vector<BoundaryParams>& boundary_params);

/**
 * @brief Builds a table from mesh boundary attribute to boundary index.
 *
 * Each boundary is looked up in the mesh as the physical group
 * `boundary:<name>`.
 *
 * @param boundary_id_map Boundary name to boundary index.
 * @param mesh Mesh loaded from a GMSH file.
 * @return Table indexed by boundary attribute. Attributes that no boundary
 *   claims hold -1.
 * @throws Error If a boundary is missing from the mesh, lists an attribute the
 *   mesh does not use, or shares an attribute with another boundary.
 */
mfem::Array<int> AttributeToBoundaryIndex(
    const std::unordered_map<std::string, int>& boundary_id_map,
    const mfem::Mesh& mesh);

/**
 * @brief Creates an array mapping the boundary element number to the boundary
 * ID that it belongs to. For a boundary element number `b`, we can get the
 * Boundary struct by calling `boundaries[element_boundaries[b]]`.
 *
 * @param boundary_id_map Map from a boundary name to its ID
 * @param mesh Mesh
 * @return mfem::Array<int>
 * @throws Error If AttributeToBoundaryIndex() fails, or a boundary element
 *   belongs to a physical group that is not in `boundary_id_map`.
 */
mfem::Array<int> CreateElementBoundaries(
    const std::unordered_map<std::string, int>& boundary_id_map,
    const mfem::Mesh& mesh);

}  // namespace nekrt

#endif  // NEKRT_BCS_BOUNDARY_H_
