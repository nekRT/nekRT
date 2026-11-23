// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_MATERIALS_MATERIAL_BANK_H_
#define NEKRT_MATERIALS_MATERIAL_BANK_H_

#include <vector>

#include "materials/material.h"
#include "utils/input_parameters.h"
namespace nekrt {
class MaterialBank {
 public:
  /**
   * @brief Construct a new Material Bank object
   *
   * @param material_params Material parameters
   */
  MaterialBank(const std::vector<MaterialParams>& material_params);

  /**
   * @brief Get the material object by ID
   *
   * @param id Material ID (index)
   * @return const Material&
   */
  const Material& Get(const int id) const { return materials_.at(id); }

  /**
   * @brief Get the number of energy groups in the simulation
   *
   * @return int
   */
  int n_groups() const { return n_groups_; }

 private:
  /// @brief Materials in simulation
  const std::vector<Material> materials_;

  /// @brief Number of energy groups
  const int n_groups_;

  /**
   * @brief Builds the vector of materials used in the simulation.
   *
   * @param material_params User-set material parameters, already checked by
   *   CheckMaterialParams().
   * @return Materials in the same order as `material_params`, so the index of a
   *   material is its ID.
   * @todo For now, this function only handles when cross sections are
   * explicitly given. Later, we will need to have a branch to handle NJOY or
   * ENDF8 cross section generation.
   */
  std::vector<Material> BuildMaterials(
      const std::vector<MaterialParams>& material_params);
};
}  // namespace nekrt

#endif  // NEKRT_MATERIALS_MATERIAL_BANK_H_
