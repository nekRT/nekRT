// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_REGIONS_REGION_H_
#define NEKRT_REGIONS_REGION_H_

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

}  // namespace nekrt

#endif  // NEKRT_REGIONS_REGION_H_
