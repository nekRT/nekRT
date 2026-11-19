// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "materials/material.h"

#include <cstddef>
#include <vector>

#include "utils/errors.h"
#include "utils/input_parameters.h"

namespace nekrt {

std::vector<Material> BuildMaterials(
    const std::vector<MaterialParams>& material_params) {
  std::vector<Material> materials;
  materials.reserve(material_params.size());

  for (const MaterialParams& params : material_params) {
    const std::size_t n_groups = params.total_xs.size();
    NEKRT_ASSERT(n_groups > 0, "Material '" + params.name + "' has no groups.");
    Material& material = materials.emplace_back();
    material.total_xs = params.total_xs;
    material.scatter_moments = params.scatter_moments;
    material.n_legendre_moments =
        static_cast<int>(params.scatter_moments.size() / (n_groups * n_groups));
  }
  return materials;
}

}  // namespace nekrt
