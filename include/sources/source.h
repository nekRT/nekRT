// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_SOURCES_SOURCE_H_
#define NEKRT_SOURCES_SOURCE_H_

#include <vector>

#include "utils/enums.h"
#include "utils/input_parameters.h"

namespace nekrt {

/**
 * @brief Struct containing information about a source in the domain.
 *
 */
struct Source {
  /// @brief Type of source.
  SourceType type = SourceType::Volumetric;

  /// @brief Isotropic source strength per group in n/cm^3/s/str, ordered
  /// lowest energy to highest.
  std::vector<double> strengths;
};

/**
 * @brief Builds the vector of sources used in the simulation.
 *
 * @param source_params User-set source parameters.
 * @return Sources in the same order as `source_params`, so the index of a
 *   source is its ID.
 */
std::vector<Source> BuildSources(
    const std::vector<SourceParams>& source_params);

}  // namespace nekrt

#endif  // NEKRT_SOURCES_SOURCE_H_
