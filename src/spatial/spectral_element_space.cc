// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "spatial/spectral_element_space.h"

namespace nekrt {
SpectralElementSpace::SpectralElementSpace(mfem::Mesh& mesh,
                                           const SpatialParams& spatial_params)
    : order_(spatial_params.order),
      gll_rules_(0, mfem::Quadrature1D::GaussLobatto),
      fe_collection_(order_, mesh.Dimension(), mfem::BasisType::GaussLobatto),
      fe_space_(&mesh, &fe_collection_),
      volume_rule_(
          &gll_rules_.Get(mesh.GetTypicalElementGeometry(), 2 * order_ - 1)),
      face_rule_(
          &gll_rules_.Get(mesh.GetTypicalFaceGeometry(), 2 * order_ - 1)) {}

}  // namespace nekrt
