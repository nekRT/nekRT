// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_SOURCES_SOURCE_H_
#define NEKRT_SOURCES_SOURCE_H_

#include <unordered_map>
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
  SourceType type;

  /// @brief Isotropic strengths of source in n/cm3/s/str.
  std::vector<double> strengths;
};

/**
 * @brief Build the vector of sources for the simulation.
 *
 * @param source_params Source parameters from the user.
 * @return std::vector<Source>
 */
std::vector<Source> BuildSources(
    const std::vector<SourceParams>& source_params);

/**
 * @brief Create a Source ID map to map source names to their ID (location in
 * global source vector)
 *
 * @param source_params User source parameters
 * @return std::unordered_map<std::string, int>
 */
std::unordered_map<std::string, int> CreateSourceIDMap(
    const std::vector<SourceParams>& source_params);

}  // namespace nekrt

#endif  // NEKRT_SOURCES_SOURCE_H_
