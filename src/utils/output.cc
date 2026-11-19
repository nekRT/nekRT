// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "utils/output.h"

#include <fmt/core.h>
#include <fmt/ostream.h>

namespace nekrt {
void PrintHeader() {
  fmt::print(
      "                         @==@                         \n"
      "                        @====@                        \n"
      "                        ++++++                        \n"
      "                       @++++++@                       \n"
      "                      @++++++++@                      \n"
      "                      %********%                      \n"
      "                     @**********@                     \n"
      "                     @**********@                     \n"
      "                    @############@                    \n"
      "                    @############@                    \n"
      "                   @##############@                   \n"
      "       @%%@        %%%%%%%%%%%%%%%%        @%%@       \n"
      "    %%.    @%%%%%%@                @%%%%%%@     %%    \n"
      "            @%%%%@                  @%%%%@            \n"
      "             @%%@                    @%%@             \n");

  fmt::print(
      "                        ▄▄        ▄▄▄▄▄▄    ▄▄▄▄▄▄▄▄ \n"
      "                        ██        ██▀▀▀▀██  ▀▀▀██▀▀▀ \n"
      "    ██▄████▄   ▄████▄   ██ ▄██▀   ██    ██     ██    \n"
      "    ██▀   ██  ██▄▄▄▄██  ██▄██     ███████      ██    \n"
      "    ██    ██  ██▀▀▀▀▀▀  ██▀██▄    ██  ▀██▄     ██    \n"
      "    ██    ██  ▀██▄▄▄▄█  ██  ▀█▄   ██    ██     ██    \n"
      "    ▀▀    ▀▀    ▀▀▀▀▀   ▀▀   ▀▀▀  ▀▀    ▀▀▀    ▀▀    \n\n");

  fmt::print(
      "    Description | Spectral Elements for Radiation Transport \n"
      "      Copyright | 2026, nekRT\n"
      "        License | "
      "https://github.com/nekRT/nekRT/blob/main/LICENSE\n\n");
}

void PrintProblem(const std::string input,
                  const InputParameters& input_params) {
  fmt::print("        Problem | {}\n", input_params.problem_params.name);
  fmt::print("     Input file | {}\n", input);
  fmt::print("      Mesh file | {}\n\n", input_params.mesh_params.filename);
}

void PrintMeshInfo(const size_t n_nodes, const size_t n_elements,
                   const unsigned int dimension) {
  fmt::print(
      "Mesh info:\n"
      "       Number of nodes: {}\n"
      "    Number of elements: {}\n"
      "             Dimension: {}\n\n",
      n_nodes, n_elements, dimension);
}

void PrintSourceIterationColumns() {
  fmt::print(
      "   k-eff Iter.       k-eff       k-eff Error  | Scatter Iter.   "
      "Flux Error     Relative Err.\n"
      "  =============  =============  ============= | =============  "
      "=============  =============\n");
}
}  // namespace nekrt
