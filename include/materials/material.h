// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_MATERIALS_MATERIAL_H_
#define NEKRT_MATERIALS_MATERIAL_H_

#include <vector>

#include "utils/input_parameters.h"

namespace nekrt {

/**
 * @brief Struct to hold material information such as cross sections.
 *
 */
struct Material {
  /// @brief Group-wise total cross sections in 1/cm
  std::vector<double> total_xs;

  /// @brief Legendre scattering moments stored in a flattened array. The order
  /// is stored s.t. the values are indexed with [moment number][from energy
  /// group][to energy group]. Values should indexed using GetScatterMoment().
  /// Legendre moments are in 1/cm.
  std::vector<double> scatter_moments;

  /// @brief Number of Legendre moments in scattering matrix.
  int n_legendre_moments = 0;
};

/**
 * @brief Get the Scatter moment from the flattened vector of moments within a
 * material. Values are stored as a vector in the Material object with 3D
 * indexing [moment number][from energy group][to energy group].
 *
 * @param moment_number Legendre moment number
 * @param from_energy_group Scatter from energy group
 * @param to_energy_group Scatter into energy group
 * @param material Material
 * @param n_energy_groups Number of energy groups in simulation
 * @return double
 */
inline double GetScatterMoment(const int moment_number,
                               const int from_energy_group,
                               const int to_energy_group,
                               const Material& material,
                               const int n_energy_groups) {
  return material
      .scatter_moments[moment_number * n_energy_groups * n_energy_groups +
                       from_energy_group * n_energy_groups + to_energy_group];
}

/**
 * @brief Builds the vector of materials used in the simulation.
 *
 * @param material_params User-set material parameters, already checked by
 *   CheckMaterialParams().
 * @return Materials in the same order as `material_params`, so the index of a
 *   material is its ID.
 * @todo For now, this function only handles when cross sections are explicitly
 * given. Later, we will need to have a branch to handle NJOY or ENDF8 cross
 * section generation.
 */
std::vector<Material> BuildMaterials(
    const std::vector<MaterialParams>& material_params);

}  // namespace nekrt

#endif  // NEKRT_MATERIALS_MATERIAL_H_
