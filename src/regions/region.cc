// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "regions/region.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "mesh/attributes.h"
#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {

std::vector<Region> BuildRegions(
    const std::vector<RegionParams>& region_params,
    const std::unordered_map<std::string, int>& material_id_map,
    const std::unordered_map<std::string, int>& source_id_map) {
  std::vector<Region> regions;
  regions.reserve(region_params.size());
  for (const RegionParams& params : region_params) {
    const std::string prefix = "Region '" + params.name + "': ";

    const auto material = material_id_map.find(params.material_name);
    NekRTError(
        material == material_id_map.end(),
        prefix + "material '" + params.material_name + "' is not defined.");
    Region& region = regions.emplace_back();
    region.material_id = material->second;

    // An empty source name means a source-free region.
    if (!params.source_name.empty()) {
      const auto source = source_id_map.find(params.source_name);
      NekRTError(
          source == source_id_map.end(),
          prefix + "source '" + params.source_name + "' is not defined.");
      region.source_id = source->second;
    }
  }
  return regions;
}

mfem::Array<int> CreateElementRegions(
    const std::unordered_map<std::string, int>& region_id_map,
    const mfem::Mesh& mesh) {
  const mfem::Array<int> attribute_to_region = AttributeToIndex(
      mesh.attribute_sets, mesh.attributes, "region", region_id_map);
  mfem::Array<int> element_regions(mesh.GetNE());
  for (int e = 0; e < mesh.GetNE(); e++) {
    const int attribute = mesh.GetAttribute(e);
    element_regions[e] = attribute_to_region[attribute];
    if (element_regions[e] == -1) {
      NekRTError("Element " + std::to_string(e) + " has attribute " +
                 std::to_string(attribute) +
                 ", which is not in any regions entry.");
    }
  }
  return element_regions;
}

}  // namespace nekrt
