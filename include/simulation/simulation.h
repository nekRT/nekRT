// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_SIMULATION_SIMULATION_H_
#define NEKRT_SIMULATION_SIMULATION_H_

#include <vector>

#include "materials/material.h"
#include "region.h"
#include "sources/source.h"
#include "utils/enums.h"

namespace nekrt {

/**
 * @brief Struct to hold simulation-wide information.
 *
 */
struct Simulation {
  /**
   * @brief Materials in simulation
   *
   */
  std::vector<Material> materials;

  /**
   * @brief Sources in simulation.
   *
   */
  std::vector<Source> sources;

  /**
   * @brief Regions in simulation.
   *
   */
  std::vector<Region> regions;

  /// @brief Boundaries in simulation.
  std::vector<BoundaryType> boundaries;

  /// @brief Number of energy groups in simulation.
  int n_energy_groups;
};

}  // namespace nekrt

#endif  // NEKRT_SIMULATION_SIMULATION_H_
