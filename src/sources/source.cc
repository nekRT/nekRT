// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "sources/source.h"

#include <vector>

#include "utils/input_parameters.h"

namespace nekrt {

std::vector<Source> BuildSources(
    const std::vector<SourceParams>& source_params) {
  std::vector<Source> sources;
  sources.reserve(source_params.size());
  for (const SourceParams& params : source_params) {
    sources.push_back({.type = params.type, .strengths = params.strengths});
  }
  return sources;
}

}  // namespace nekrt
