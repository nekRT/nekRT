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
    material.total_xs = params.total_xs;
    material.scatter_moments = params.scatter_moments;
    material.n_legendre_moments = params.n_legendre_moments;
    materials.push_back(std::move(material));
  }
  return materials;
}
}  // namespace nekrt
