// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_REGIONS_REGION_H_
#define NEKRT_REGIONS_REGION_H_

#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "materials/material.h"
#include "utils/input_parameters.h"

namespace nekrt {

struct Material;

/**
 * @brief A region holds collective information about what exists in a single
 * element.
 *
 */
struct Region {
  /// @brief Index of the region's material in the material vector.
  int material_id = -1;

  /// @brief Index of the region's source in the source vector, or -1 for a
  /// source-free region.
  int source_id = -1;
};

/**
 * @brief Constructs the Region objects used in the simulation.
 *
 * @param region_params User-set region parameters.
 * @param material_id_map Material name to material index (see
 *   CreateNameIndexMap()).
 * @param source_id_map Source name to source index (see CreateNameIndexMap()).
 * @return Regions in the same order as `region_params`, so the index of a
 *   region is its ID.
 * @throws Error If a region names a material or source that is not in the
 *   maps.
 */
std::vector<Region> BuildRegions(
    const std::vector<RegionParams>& region_params,
    const std::unordered_map<std::string, int>& material_id_map,
    const std::unordered_map<std::string, int>& source_id_map);

/**
 * @brief Creates an array mapping each element to the ID of its region.
 *
 * Each region is looked up in the mesh as the physical group `region:<name>`.
 * For an element `e`, its Region is `regions[element_regions[e]]`.
 *
 * @param region_id_map Region name to region index (see CreateNameIndexMap()).
 * @param mesh Mesh loaded from a GMSH file.
 * @return Region index of each element, of size `mesh.GetNE()`.
 * @throws Error If AttributeToIndex() fails, or an element has an attribute
 *   that no region claims.
 */
mfem::Array<int> CreateElementRegions(
    const std::unordered_map<std::string, int>& region_id_map,
    const mfem::Mesh& mesh);

mfem::Vector GetTotalXSFromRegions(const mfem::Array<int>& element_regions,
                                   const std::vector<Region>& regions,
                                   const std::vector<Material>& materials,
                                   const int energy_group);

}  // namespace nekrt

#endif  // NEKRT_REGIONS_REGION_H_
