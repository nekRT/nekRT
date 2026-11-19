// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "regions/region.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {
std::vector<Region> BuildRegions(
    const std::vector<RegionParams>& region_params,
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

std::unordered_map<std::string, int> CreateRegionIDMap(
    const std::vector<RegionParams>& region_params) {
  std::unordered_map<std::string, int> region_id_map;

  int count = 0;
  for (const auto& params : region_params) {
    region_id_map[params.name] = count;
    count++;
  }
  return region_id_map;
}

mfem::Array<int> AttributeToRegionIndex(
    const std::unordered_map<std::string, int>& region_id_map,
    const mfem::Mesh& mesh) {
  mfem::Array<int> attribute_to_region(mesh.attributes.Max() + 1);
  attribute_to_region = -1;

  for (const auto& [name, id] : region_id_map) {
    const std::string group_name = "region:" + name;
    if (!mesh.attribute_sets.AttributeSetExists(group_name)) {
      NekRTError(group_name + " not found in mesh.");
    }
    const auto& attribute_set = mesh.attribute_sets.GetAttributeSet(group_name);
    for (const int attribute : attribute_set) {
      if (attribute < 0 || attribute >= attribute_to_region.Size()) {
        NekRTError(group_name + " has attribute " + std::to_string(attribute) +
                   ", which no element in the mesh uses.");
      }
      if (attribute_to_region[attribute] != -1) {
        NekRTError(group_name + " shares attribute " +
                   std::to_string(attribute) + " with another region.");
      }
      attribute_to_region[attribute] = id;
    }
  }
  return attribute_to_region;
}
}  // namespace nekrt
