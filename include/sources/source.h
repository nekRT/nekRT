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
  SourceType type;

  /// @brief Isotropic strengths of source in n/cm3/s/str.
  std::vector<double> strengths;
};

std::vector<Source> BuildSources(const std::vector<SourceParams> source_params);
}  // namespace nekrt

#endif  // NEKRT_SOURCES_SOURCE_H_
