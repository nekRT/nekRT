// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_QUADRATURE_QUAD_SET_PARAMS_H_
#define NEKRT_QUADRATURE_QUAD_SET_PARAMS_H_

#include <cstddef>

#include "utils/enums.h"

namespace nekrt {
/**
 * @brief Parameters of a one-dimensional quadrature set.
 *
 */
struct QuadSetParams {
  /// @brief Type of quadrature set
  QuadSetType type = QuadSetType::GaussLegendre;

  /// @brief Number of points in set (`n_directions` in the YAML)
  std::size_t n_points = 0;

  /// @brief Minimum value (integrate from value). Set by the reader from the
  /// set's role, not read from the input file.
  double min = 0.0;

  /// @brief Maximum value (integrate to value). Set by the reader from the
  /// set's role, not read from the input file.
  double max = 0.0;
};
}  // namespace nekrt

#endif  // NEKRT_QUADRATURE_QUAD_SET_PARAMS_H_
