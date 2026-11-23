// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include <fmt/core.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <mfem.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "bcs/boundary.h"
#include "materials/material.h"
#include "materials/material_bank.h"
#include "quadrature/quadrature.h"
#include "regions/region.h"
#include "sources/source.h"
#include "spatial/spectral_element_space.h"
#include "utils/errors.h"
#include "utils/input_parameters.h"
#include "utils/legendre_polynomials.h"
#include "utils/output.h"

using namespace mfem;

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
    nekrt::MaterialBank material_bank(input_params.materials);
    const std::vector<nekrt::Source> independent_sources =
        nekrt::BuildSources(input_params.sources);
    const std::vector<nekrt::Region> regions = nekrt::BuildRegions(
        input_params.regions, nekrt::CreateNameIndexMap(input_params.materials),
        nekrt::CreateNameIndexMap(input_params.sources));

    // MFEM aborts instead of throwing when the mesh file is missing.
    const std::string& mesh_filename = input_params.mesh_params.filename;
    nekrt::NekRTError(!std::filesystem::exists(mesh_filename),
                      "Mesh file '" + mesh_filename + "' not found.");
    Mesh mesh(mesh_filename);

    // Map each element to its region and each boundary element to its
    // boundary condition.
    const Array<int> element_regions = nekrt::CreateElementRegions(
        nekrt::CreateNameIndexMap(input_params.regions), mesh);
    const Array<int> element_materials =
        nekrt::CreateElementMaterials(element_regions, regions);
    const Array<int> element_boundaries = nekrt::CreateElementBoundaries(
        nekrt::CreateNameIndexMap(input_params.boundaries), mesh);

    // create spherical quadrature set
    nekrt::QuadSet2D angular_quad_set =
        nekrt::SphericalQuadSet(input_params.angular_params);

    // The eigenvalue columns are for an eigenvalue run mode, which RunMode
    // does not have yet.
    nekrt::PrintSourceIterationColumns(/*include_eigenvalue=*/false);

    nekrt::SpectralElementSpace se_space(mesh, input_params.spatial_params);
    std::vector<nekrt::SpectralElementSpace> se_spaces;
    se_spaces.reserve(material_bank.n_groups());

    std::vector<GridFunction> sources;
    sources.reserve(material_bank.n_groups());

    for (auto g = 0; g < material_bank.n_groups(); g++) {
      se_spaces.push_back(
          nekrt::SpectralElementSpace(mesh, input_params.spatial_params));
      GridFunction new_source(&se_spaces.back().fe_space());
      new_source = 0.0;
      sources.push_back(std::move(new_source));
    }

    for (auto g_prime = 0; g_prime < material_bank.n_groups(); g_prime++) {
      for (auto n_prime = 0; n_prime < angular_quad_set.n_nodes; n_prime++) {
        fmt::print("n = {}\n", n_prime);

        auto& se_space = se_spaces[g_prime];
        auto& source = sources[g_prime];

        double quadrature_weight = angular_quad_set.weights[n_prime];
        mfem::Vector ordinate_cartesian = nekrt::PolarToRectangular(
            angular_quad_set.first_axis_nodes[n_prime],
            angular_quad_set.second_axis_nodes[n_prime]);
        VectorConstantCoefficient ordinate(ordinate_cartesian);

        PWConstCoefficient Sigma_t(nekrt::GetTotalXSFromRegions(
            element_regions, regions, material_bank.materials(), g_prime));

        BilinearForm A(&se_space.fe_space());
        A.AddDomainIntegrator(
            new MassIntegrator(Sigma_t, &se_space.volume_rule()));
        A.AddDomainIntegrator(
            new ConservativeConvectionIntegrator(ordinate, 1.0));
        A.Assemble(0);
        A.Finalize(0);

        GridFunctionCoefficient source_grid_function(&source);
        LinearForm rhs(&se_space.fe_space());
        rhs.AddDomainIntegrator(new DomainLFIntegrator(
            source_grid_function, &se_space.volume_rule()));
        rhs.Assemble();

        GridFunction psi(&se_space.fe_space());
        psi = 0.0;

        Array<int> boundary_dofs;
        se_space.fe_space().GetBoundaryTrueDofs(boundary_dofs);

        mfem::GMRESSolver gmres;
        gmres.SetOperator(A);
        gmres.Mult(rhs, psi);

        // update source -- this is a running sum through the iterations
        // we never store the angular flux unless it is specifically desired for
        // the output
        for (auto g = 0; g < material_bank.n_groups(); g++) {
          for (auto n = 0; n < angular_quad_set.n_nodes; n++) {
            auto scatter_direction = nekrt::PolarToRectangular(
                angular_quad_set.first_axis_nodes[n],
                angular_quad_set.second_axis_nodes[n]);
            double scattering_cosine = ordinate_cartesian * scatter_direction;
            for (auto i = 0; i < material_bank.n_materials(); i++) {
              auto element_material = material_bank.GetByID(i);
              for (auto j = 0; j < element_materials.Size(); j++) {
                if (element_materials[j] == i) {
                  for (auto m = 0; m < element_material.n_legendre_moments;
                       m++) {
                    sources[g * angular_quad_set.n_nodes + n] +=
                        nekrt::LegendreExpansionCoefficient(m,
                                                            scattering_cosine) *
                        nekrt::GetScatterMoment(m, g_prime, g, element_material,
                                                material_bank.n_groups()) *
                        quadrature_weight * psi;
                  }
                }
              }
            }
          }
        }
      }
    }

  } catch (const nekrt::Error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
