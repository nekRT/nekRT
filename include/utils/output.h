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

/**
 * @brief Print the column headings of the source iteration table.
 *
 */
void PrintSourceIterationColumns();

}  // namespace nekrt

#endif  // NEKRT_UTILS_OUTPUT_H_
