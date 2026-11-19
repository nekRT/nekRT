// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include <filesystem>
#include <iostream>
#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "bcs/boundary.h"
#include "materials/material.h"
#include "regions/region.h"
#include "sources/source.h"
#include "utils/errors.h"
#include "utils/input_parameters.h"
#include "utils/output.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file_name>\n";
    return 1;
  }

  nekrt::PrintHeader();

  const std::string input_filename = argv[1];
  try {
    const nekrt::InputParameters input_params =
        nekrt::ReadInputFile(input_filename);

    nekrt::PrintProblem(input_filename, input_params);

    // Build the problem data. Each vector is in input order, so the index of
    // an entry is its ID.
    const std::vector<nekrt::Boundary> boundaries =
        nekrt::BuildBoundaries(input_params.boundaries);
    const std::vector<nekrt::Material> materials =
        nekrt::BuildMaterials(input_params.materials);
    const std::vector<nekrt::Source> sources =
        nekrt::BuildSources(input_params.sources);
    const std::vector<nekrt::Region> regions = nekrt::BuildRegions(
        input_params.regions, nekrt::CreateNameIndexMap(input_params.materials),
        nekrt::CreateNameIndexMap(input_params.sources));

    // MFEM aborts instead of throwing when the mesh file is missing.
    const std::string& mesh_filename = input_params.mesh_params.filename;
    nekrt::NekRTError(!std::filesystem::exists(mesh_filename),
                      "Mesh file '" + mesh_filename + "' not found.");
    const mfem::Mesh mesh(mesh_filename);

    // Map each element to its region and each boundary element to its
    // boundary condition.
    const mfem::Array<int> element_regions = nekrt::CreateElementRegions(
        nekrt::CreateNameIndexMap(input_params.regions), mesh);
    const mfem::Array<int> element_boundaries = nekrt::CreateElementBoundaries(
        nekrt::CreateNameIndexMap(input_params.boundaries), mesh);

    // The eigenvalue columns are for an eigenvalue run mode, which RunMode
    // does not have yet.
    nekrt::PrintSourceIterationColumns(/*include_eigenvalue=*/false);
  } catch (const nekrt::Error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
