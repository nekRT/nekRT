// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "materials/material.h"

#include "regions/region.h"

namespace nekrt {
mfem::Array<int> CreateElementMaterials(const mfem::Array<int>& element_regions,
                                        const std::vector<Region>& regions) {
  mfem::Array<int> element_materials(element_regions.Size());
  element_materials = -1;
  for (auto i = 0; i < element_regions.Size(); i++) {
    auto region_id = element_regions[i];
    element_materials[i] = regions[region_id].material_id;
  }
  return element_materials;
}
}  // namespace nekrt
