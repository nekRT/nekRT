// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include <fmt/core.h>

#include <iostream>
#include <string>

#include "utils/errors.h"
#include "utils/input_parameters.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file_name>\n";
    return 1;
  }

  const std::string input_filename = argv[1];
  try {
    fmt::print("Loading {} input file...\n", input_filename);
    const nekrt::InputParameters input_params =
        nekrt::ReadInputFile(input_filename);
    fmt::print("Done. Problem: {}\n", input_params.problem_params.name);
  } catch (const nekrt::Error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
