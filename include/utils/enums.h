// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_UTILS_ENUMS_H_
#define NEKRT_UTILS_ENUMS_H_

namespace nekrt {
/**
 * @brief Available run modes.
 *
 */
enum class RunMode { FixedSource };

/**
 * @brief Available output file formats.
 *
 */
enum class OutputFormat { VTKHDF };

/**
 * @brief Available values to be exported from the simulation.
 *
 */
enum class ExportValues { ScalarFlux, AngularFlux };

/**
 * @brief Available boundary condition types.
 *
 */
enum class BoundaryType { Vacuum, Reflective };

/**
 * @brief Available independent source types.
 *
 */
enum class SourceType { Volumetric };

/**
 * @brief Available angular treatment models.
 *
 */
enum class AngularModel { DiscreteOrdinates };

/**
 * @brief Available spherical quadrature types.
 *
 */
enum class SphericalQuadType { TensorProduct };

/**
 * @brief Available generic quadrature sets.
 *
 */
enum class QuadSetType { GaussLegendre, Trapezoid, GaussLobattoLegendre };
}  // namespace nekrt

#endif  // NEKRT_UTILS_ENUMS_H_
