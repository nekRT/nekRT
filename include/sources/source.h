// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

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
