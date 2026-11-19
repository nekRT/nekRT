// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "sources/source.h"

namespace nekrt {

std::vector<Source> BuildSources(
    const std::vector<SourceParams>& source_params) {
  std::vector<Source> sources;
  sources.reserve(source_params.size());

  for (const auto& params : source_params) {
    Source source;
    source.strengths = params.strengths;
    source.type = params.type;
    sources.push_back(std::move(source));
  }
  return sources;
}

std::unordered_map<std::string, int> CreateSourceIDMap(
    const std::vector<SourceParams>& source_params) {
  std::unordered_map<std::string, int> source_id_map;

  int count = 0;
  for (const auto& params : source_params) {
    source_id_map[params.name] = count;
    count++;
  }
  return source_id_map;
}
}  // namespace nekrt
