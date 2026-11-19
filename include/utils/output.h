// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_UTILS_OUTPUT_H_
#define NEKRT_UTILS_OUTPUT_H_

#include <cstddef>
#include <string>
#include <string_view>

#include "utils/input_parameters.h"

namespace nekrt {

/// Width of the right-aligned label column in `Label | value` lines.
inline constexpr int kLabelWidth = 16;

/// Maximum width of a line of nekRT output.
inline constexpr int kLineWidth = 80;

/// Information about the current run, printed in the header.
struct RunInfo {
  std::string date_time;  ///< Local start time, `YYYY-MM-DD HH:MM:SS`.
  std::string hostname;   ///< Host the run is on.
};

/**
 * @brief Collect the start time and hostname of the current run.
 *
 * @return Run information. A field that can't be determined is "unknown".
 */
RunInfo GetRunInfo();

/**
 * @brief Format one `Label | value` line with a right-aligned label.
 *
 * @param label Label, right-aligned to kLabelWidth. May be empty.
 * @param value Value printed after the separator.
 * @return Formatted line, ending in a newline.
 */
std::string FormatField(std::string_view label, std::string_view value);

/**
 * @brief Format a section banner, e.g. ` ====>  MESH  <====`, kLineWidth wide.
 *
 * @param title Section title.
 * @return Formatted banner, ending in a newline.
 */
std::string FormatBanner(std::string_view title);

/**
 * @brief Format the startup header: logo, wordmark, and build and run info.
 *
 * @param run_info Information about the current run.
 * @return Formatted header.
 */
std::string FormatHeader(const RunInfo& run_info);

/**
 * @brief Format the problem section: problem name, input file, and mesh file.
 *
 * @param input Path to the input file.
 * @param input_params Parameters read from the input file.
 * @return Formatted section.
 */
std::string FormatProblem(std::string_view input,
                          const InputParameters& input_params);

/**
 * @brief Format the mesh section.
 *
 * @param n_nodes Number of nodes in mesh
 * @param n_elements Number of elements in mesh
 * @param dimension Mesh dimension
 * @return Formatted section.
 */
std::string FormatMeshInfo(std::size_t n_nodes, std::size_t n_elements,
                           unsigned int dimension);

/**
 * @brief Print the startup header (see FormatHeader()) for the current run.
 *
 */
void PrintHeader();

/**
 * @brief Print the problem section (see FormatProblem()).
 *
 * @param input Path to the input file.
 * @param input_params Parameters read from the input file.
 */
void PrintProblem(std::string_view input, const InputParameters& input_params);

/**
 * @brief Print the mesh section (see FormatMeshInfo()).
 *
 * @param n_nodes Number of nodes in mesh
 * @param n_elements Number of elements in mesh
 * @param dimension Mesh dimension
 */
void PrintMeshInfo(std::size_t n_nodes, std::size_t n_elements,
                   unsigned int dimension);

/// One scattering (inner) iteration, printed as a source iteration table row.
struct ScatterIterationRow {
  unsigned int iteration = 0;   ///< Scattering iteration number.
  double flux_error = 0.0;      ///< Change in scalar flux since last iteration.
  double relative_error = 0.0;  ///< flux_error relative to the scalar flux.
};

/// One power (outer) iteration, printed in the eigenvalue columns of a row.
struct EigenvalueIterationRow {
  unsigned int iteration = 0;  ///< Power iteration number.
  double k_eff = 0.0;          ///< Current estimate of k-eff.
  double k_eff_error = 0.0;    ///< Change in k-eff since last iteration.
};

/**
 * @brief Format the source iteration section banner and table column headings.
 *
 * The scattering columns are always shown. The eigenvalue (power iteration)
 * columns are shown to their left when `include_eigenvalue` is true. With
 * both, every line is kLineWidth wide.
 *
 * @param include_eigenvalue Whether to include the eigenvalue columns.
 * @return Formatted banner and headings.
 */
std::string FormatSourceIterationColumns(bool include_eigenvalue);

/**
 * @brief Format a row of the fixed-source iteration table, aligned with
 * FormatSourceIterationColumns(false).
 *
 * @param scatter Scattering iteration to print.
 * @return Formatted row, ending in a newline.
 */
std::string FormatSourceIterationRow(const ScatterIterationRow& scatter);

/**
 * @brief Format a row of the eigenvalue iteration table, aligned with
 * FormatSourceIterationColumns(true).
 *
 * @param eigenvalue Power iteration to print.
 * @param scatter Scattering iteration to print.
 * @return Formatted row, ending in a newline.
 */
std::string FormatSourceIterationRow(const EigenvalueIterationRow& eigenvalue,
                                     const ScatterIterationRow& scatter);

/**
 * @brief Print the source iteration banner and table column headings (see
 * FormatSourceIterationColumns()).
 *
 * @param include_eigenvalue Whether to include the eigenvalue columns.
 */
void PrintSourceIterationColumns(bool include_eigenvalue);

/**
 * @brief Print a row of the fixed-source iteration table.
 *
 * @param scatter Scattering iteration to print.
 */
void PrintSourceIterationRow(const ScatterIterationRow& scatter);

/**
 * @brief Print a row of the eigenvalue iteration table.
 *
 * @param eigenvalue Power iteration to print.
 * @param scatter Scattering iteration to print.
 */
void PrintSourceIterationRow(const EigenvalueIterationRow& eigenvalue,
                             const ScatterIterationRow& scatter);

}  // namespace nekrt

#endif  // NEKRT_UTILS_OUTPUT_H_
