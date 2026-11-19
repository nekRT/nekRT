// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include <iostream>
#include <mfem.hpp>
#include <string>

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

  nekrt::InputParameters input_params;
  const std::string input_filename = argv[1];
  try {
    input_params = nekrt::ReadInputFile(input_filename);
  } catch (const nekrt::Error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  nekrt::PrintProblem(input_filename, input_params);

  // create materials
  std::vector<nekrt::Material> materials =
      nekrt::BuildMaterials(input_params.materials);

  std::unordered_map<std::string, int> material_id_map =
      nekrt::CreateMaterialIDMap(input_params.materials);

  // create sources
  std::vector<nekrt::Source> sources =
      nekrt::BuildSources(input_params.sources);

  std::unordered_map<std::string, int> source_id_map =
      nekrt::CreateSourceIDMap(input_params.sources);

  std::vector<nekrt::Region> regions =
      nekrt::BuildRegions(input_params.regions, material_id_map, source_id_map);
  std::unordered_map<std::string, int> region_id_map =
      nekrt::CreateRegionIDMap(input_params.regions);

  // load mesh
  mfem::Mesh mesh(input_params.mesh_params.filename);

  // The eigenvalue columns are for an eigenvalue run mode, which RunMode
  // does not have yet.
  nekrt::PrintSourceIterationColumns(/*include_eigenvalue=*/false);

  return 0;
}
