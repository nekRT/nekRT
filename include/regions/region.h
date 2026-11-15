// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_REGION_REGION_H_
#define NEKRT_REGION_REGION_H_

namespace nekrt {

/**
 * @brief A region holds collective information about what exists in a single
 * element.
 *
 */
struct Region {
  /// @brief Material ID
  int material_id = -1;

  /// @brief Source ID
  int source_id = -1;
};

}  // namespace nekrt

#endif  // NEKRT_REGION_REGION_H_