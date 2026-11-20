// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_SPATIAL_SPECTRAL_ELEMENT_SPACE_H_
#define NEKRT_SPATIAL_SPECTRAL_ELEMENT_SPACE_H_

#include <mfem.hpp>

#include "utils/input_parameters.h"

namespace nekrt {
class SpectralElementSpace {
 public:
  SpectralElementSpace(mfem::Mesh& mesh, const SpatialParams& spatial_params);

  int order() const { return order_; }
  mfem::FiniteElementSpace& fe_space() { return fe_space_; }
  const mfem::IntegrationRule& volume_rule() const { return *volume_rule_; }
  const mfem::IntegrationRule& face_rule() const { return *face_rule_; }

 private:
  const int order_;
  mfem::IntegrationRules gll_rules_;
  const mfem::H1_FECollection fe_collection_;
  mfem::FiniteElementSpace fe_space_;
  const mfem::IntegrationRule* volume_rule_;
  const mfem::IntegrationRule* face_rule_;
};

}  // namespace nekrt

#endif  // NEKRT_SPATIAL_SPECTRAL_ELEMENT_SPACE_H_
