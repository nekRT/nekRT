// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "regions/region.h"

#include "utils/input_parameters.h"

namespace nekrt {
std::vector<Region> BuildRegions(
    const std::vector<RegionParams> region_params,
    const std::unordered_map<std::string, int>& material_id_map,
    const std::unordered_map<std::string, int>& source_id_map) {
  std::vector<Region> regions;
  regions.reserve(region_params.size());
  for (const auto& params : region_params) {
    Region region;
    region.material_id = material_id_map.at(params.material_name);
    if (source_id_map.contains(params.source_name))
      region.source_id = source_id_map.at(params.source_name);
    regions.push_back(std::move(region));
  }
  return regions;
}
}  // namespace nekrt
