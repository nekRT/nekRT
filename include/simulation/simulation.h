// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include <vector>

#include "materials/material.h"
#include "sources/source.h"

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
   * @brief Sources in simulation
   *
   */
  std::vector<Source> sources;
};

}  // namespace nekrt
