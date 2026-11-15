// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include <vector>

namespace nekrt {

struct Material {
  /// @brief Material ID
  int id;

  /// @brief Group-wise total cross sections in 1/cm
  std::vector<double> total_xs;

  /// @brief Legendre scattering moments stored in a flattened array.
  std::vector<double> scatter_moments;

  /**
   * @brief Number of Legendre moments in scattering matrix.
   *
   */
  int n_legendre_moments;
};

/**
 * @brief Get the Scatter moment from the flattened vector of moments within a
 * material.
 *
 * @param moment_number Legendre moment number.
 * @param from_energy_group Scatter from energy group.
 * @param to_energy_group Scatter into energy group.
 * @param material Material.
 * @param n_energy_groups Number of energy groups in simulation.
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

}  // namespace nekrt
