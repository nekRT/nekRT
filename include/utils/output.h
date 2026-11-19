// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_UTILS_OUTPUT_H_
#define NEKRT_UTILS_OUTPUT_H_

#include <string>

#include "utils/input_parameters.h"

namespace nekrt {

void PrintHeader();

void PrintProblem(const std::string input, const InputParameters& input_params);

/**
 * @brief Print information about the mesh
 *
 * @param n_nodes Number of nodes in mesh
 * @param n_elements Number of elements in mesh
 * @param dimension Mesh dimension
 */
void PrintMeshInfo(const size_t n_nodes, const size_t n_elements,
                   const unsigned int dimension);

void PrintSourceIterationColumns();

}  // namespace nekrt

#endif  // NEKRT_UTILS_OUTPUT_H_
