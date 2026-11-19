// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "mesh/attributes.h"

#include <string>
#include <string_view>
#include <unordered_map>

#include "utils/errors.h"

namespace nekrt {

mfem::Array<int> AttributeToIndex(
    const mfem::AttributeSets& attribute_sets,
    const mfem::Array<int>& attributes, std::string_view prefix,
    const std::unordered_map<std::string, int>& name_to_index) {
  // Array::Max() is undefined for an empty array.
  mfem::Array<int> attribute_to_index(
      attributes.Size() > 0 ? attributes.Max() + 1 : 0);
  attribute_to_index = -1;

  for (const auto& [name, index] : name_to_index) {
    const std::string group_name = std::string(prefix) + ":" + name;
    NekRTError(!attribute_sets.AttributeSetExists(group_name),
               "Physical group '" + group_name + "' not found in the mesh.");
    for (const int attribute : attribute_sets.GetAttributeSet(group_name)) {
      NekRTError(attributes.Find(attribute) == -1,
                 "Physical group '" + group_name + "' has attribute " +
                     std::to_string(attribute) +
                     ", which the mesh does not use.");
      NekRTError(attribute_to_index[attribute] != -1,
                 "Physical group '" + group_name + "' shares attribute " +
                     std::to_string(attribute) + " with another " +
                     std::string(prefix) + ".");
      attribute_to_index[attribute] = index;
    }
  }
  return attribute_to_index;
}

}  // namespace nekrt
