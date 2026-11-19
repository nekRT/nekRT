// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_REGIONS_REGION_H_
#define NEKRT_REGIONS_REGION_H_

#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/input_parameters.h"

namespace nekrt {

/**
 * @brief A region holds collective information about what exists in a single
 * element.
 *
 */
struct Region {
  /// @brief Material ID
  int material_id = -1;

  /// @brief Source ID
  int source_id = -1;
};

/**
 * @brief Construct the Region objects used in the simulation.
 *
 * @param region_params User-set region parameters
 * @param material_id_map Material ID map
 * @param source_id_map Source ID map
 * @return std::vector<Region>
 */
std::vector<Region> BuildRegions(
    const std::vector<RegionParams>& region_params,
    const std::unordered_map<std::string, int>& material_id_map,
    const std::unordered_map<std::string, int>& source_id_map);

std::unordered_map<std::string, int> CreateRegionIDMap(
    const std::vector<RegionParams>& region_params);

/**
 * @brief Builds a table from mesh element attribute to region index.
 *
 * Each region is looked up in the mesh as the physical group `region:<name>`.
 *
 * @param region_id_map Region name to region index.
 * @param mesh Mesh loaded from a GMSH file.
 * @return Table indexed by element attribute. Attributes that no region claims
 *   hold -1.
 * @throws Error If a region is missing from the mesh, lists an attribute the
 *   mesh does not use, or shares an attribute with another region.
 */
mfem::Array<int> AttributeToRegionIndex(
    const std::unordered_map<std::string, int>& region_id_map,
    const mfem::Mesh& mesh);

/**
 * @brief Creates an array mapping the element number to the region ID that it
 * contains. For an element number `e`, we can get the Region struct by called
 * `element_regions[e]`.
 *
 * @param region_id_map Map from a region name to its ID
 * @param mesh Mesh
 * @return mfem::Array<int>
 */
mfem::Array<int> CreateElementRegions(
    const std::unordered_map<std::string, int>& region_id_map,
    const mfem::Mesh& mesh);

}  // namespace nekrt

#endif  // NEKRT_REGIONS_REGION_H_
