// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_SOURCES_SOURCE_H_
#define NEKRT_SOURCES_SOURCE_H_

#include "utils/enums.h"

namespace nekrt {

/**
 * @brief Struct containing information about a source in the domain
 *
 */
struct Source {
  /// @brief Source ID.
  int id;

  /// @brief Type of source.
  SourceType type;
};
}  // namespace nekrt

#endif  // NEKRT_SOURCES_SOURCE_H_
