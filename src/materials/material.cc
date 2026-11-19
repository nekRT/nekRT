// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "materials/material.h"

namespace nekrt {
std::vector<Material> BuildMaterials(
    const std::vector<MaterialParams>& material_params) {
  std::vector<Material> materials;
  materials.reserve(material_params.size());

  for (const auto& params : material_params) {
    Material material;
    int n_groups = params.total_xs.size() material.total_xs = params.total_xs;
    material.scatter_moments = params.scatter_moments;
    material.n_legendre_moments =
        params.scatter_moments.size() / n_groups / n_groups;
    materials.push_back(std::move(material));
  }
  return materials;
}
}  // namespace nekrt
