// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#ifndef NEKRT_MESH_ATTRIBUTES_H_
#define NEKRT_MESH_ATTRIBUTES_H_

#include <mfem.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace nekrt {

/**
 * @brief Builds a table from mesh attribute to the index of the named entry
 * (region or boundary) that claims it.
 *
 * GMSH physical groups are read by MFEM into named attribute sets. Each entry
 * of `name_to_index` is looked up as the set `<prefix>:<name>`, and every
 * attribute in that set is mapped to the entry's index.
 *
 * @param attribute_sets Named attribute sets of the mesh, either
 *   `mesh.attribute_sets` or `mesh.bdr_attribute_sets`.
 * @param attributes Attributes used by the mesh, either `mesh.attributes` or
 *   `mesh.bdr_attributes` (matching `attribute_sets`).
 * @param prefix Physical group prefix, for example `"region"` or `"boundary"`.
 * @param name_to_index Entry name (without the prefix) to entry index.
 * @return Table indexed by attribute, of size `max(attributes) + 1`.
 *   Attributes that no entry claims hold -1.
 * @throws Error If an entry has no attribute set in the mesh, its set lists an
 *   attribute that is not in `attributes`, or two entries share an attribute.
 */
mfem::Array<int> AttributeToIndex(
    const mfem::AttributeSets& attribute_sets,
    const mfem::Array<int>& attributes, std::string_view prefix,
    const std::unordered_map<std::string, int>& name_to_index);

}  // namespace nekrt

#endif  // NEKRT_MESH_ATTRIBUTES_H_
