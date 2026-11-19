// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "utils/output.h"

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <unistd.h>

#include <array>
#include <cstddef>
#include <ctime>
#include <mfem.hpp>
#include <string>
#include <string_view>

#include "utils/build_info.h"

namespace nekrt {
namespace {

// The logo and wordmark are drawn centered in a frame kArtWidth columns wide,
// and are kept pure ASCII so they display correctly in any log file or locale.
constexpr int kArtWidth = 54;

constexpr std::string_view kLogo =
    "\n\n"
    "                         @==@\n"
    "                        @====@\n"
    "                        ++++++\n"
    "                       @++++++@\n"
    "                      @++++++++@\n"
    "                      %********%\n"
    "                     @**********@\n"
    "                     @**********@\n"
    "                    @############@\n"
    "                    @############@\n"
    "                   @##############@\n"
    "       @%%@        %%%%%%%%%%%%%%%%        @%%@\n"
    "    %%.    @%%%%%%@                @%%%%%%@     %%\n"
    "            @%%%%@                  @%%%%@\n"
    "             @%%@                    @%%@\n";

constexpr std::string_view kWordmark =
    "                       ##      #######   ########\n"
    "                       ##      ##    ##     ##\n"
    "       ## ####   ####  ##  ##  ##    ##     ##\n"
    "       ###  ## ##   ## ## ##   #######      ##\n"
    "       ##   ## ####### ####    ##  ##       ##\n"
    "       ##   ## ##      ## ##   ##   ##      ##\n"
    "       ##   ##  #####  ##  ##  ##    ##     ##\n\n";

// Indent every non-empty line of `block` by `n` spaces. Empty lines stay
// empty, so no trailing whitespace is added.
std::string Indent(std::string_view block, const int n) {
  const std::string padding(static_cast<std::size_t>(n), ' ');
  std::string indented;
  bool at_line_start = true;
  for (const char c : block) {
    if (at_line_start && c != '\n') {
      indented += padding;
    }
    indented += c;
    at_line_start = (c == '\n');
  }
  return indented;
}

std::string CurrentDateTime() {
  const std::time_t now = std::time(nullptr);
  std::tm local_time{};
  if (localtime_r(&now, &local_time) == nullptr) {
    return "unknown";
  }
  return fmt::format("{:%Y-%m-%d %H:%M:%S}", local_time);
}

std::string Hostname() {
  std::array<char, 256> buffer{};
  if (gethostname(buffer.data(), buffer.size() - 1) != 0) {
    return "unknown";
  }
  return {buffer.data()};
}

}  // namespace

RunInfo GetRunInfo() { return {CurrentDateTime(), Hostname()}; }

std::string FormatField(std::string_view label, std::string_view value) {
  return fmt::format("{:>{}} | {}\n", label, kLabelWidth, value);
}

std::string FormatBanner(std::string_view title) {
  return fmt::format(" {:=^{}}\n", fmt::format(">  {}  <", title),
                     kLineWidth - 1);
}

std::string FormatHeader(const RunInfo& run_info) {
  std::string header;
  // Center the art on the full line width.
  constexpr int kArtIndent = (kLineWidth - kArtWidth) / 2;
  header += Indent(kLogo, kArtIndent);
  header += '\n';
  header += Indent(kWordmark, kArtIndent);
  header += '\n';
  header += FormatField("", "Spectral Elements for Radiation Transport");
  header += FormatField("Copyright", "2026, nekRT");
  header += FormatField("License",
                        "https://github.com/nekRT/nekRT/blob/main/LICENSE");
  header += FormatField("Version", kVersion);
  header += FormatField("Git SHA1", kGitSha);
  header += FormatField("Build Type", kBuildType);
  header += FormatField("Compiler", kCompiler);
  header += FormatField("MFEM Version", MFEM_VERSION_STRING);
  header += FormatField("Date/Time", run_info.date_time);
  header += FormatField("Hostname", run_info.hostname);
  header += '\n';
  return header;
}

std::string FormatProblem(std::string_view input,
                          const InputParameters& input_params) {
  std::string section = FormatBanner("PROBLEM");
  section += '\n';
  section += FormatField("Problem", input_params.problem_params.name);
  section += FormatField("Input File", input);
  section += FormatField("Mesh File", input_params.mesh_params.filename);
  section += '\n';
  return section;
}

std::string FormatMeshInfo(const std::size_t n_nodes,
                           const std::size_t n_elements,
                           const unsigned int dimension) {
  std::string section = FormatBanner("MESH");
  section += '\n';
  section += FormatField("Dimension", fmt::to_string(dimension));
  section += FormatField("Nodes", fmt::to_string(n_nodes));
  section += FormatField("Elements", fmt::to_string(n_elements));
  section += '\n';
  return section;
}

// NOTE(parallel): Only rank 0 should print once MPI is added.
void PrintHeader() { fmt::print("{}", FormatHeader(GetRunInfo())); }

void PrintProblem(std::string_view input, const InputParameters& input_params) {
  fmt::print("{}", FormatProblem(input, input_params));
}

void PrintMeshInfo(const std::size_t n_nodes, const std::size_t n_elements,
                   const unsigned int dimension) {
  fmt::print("{}", FormatMeshInfo(n_nodes, n_elements, dimension));
}

void PrintSourceIterationColumns() {
  fmt::print(
      "   k-eff Iter.       k-eff       k-eff Error  | Scatter Iter.   "
      "Flux Error     Relative Err.\n"
      "  =============  =============  ============= | =============  "
      "=============  =============\n");
}

}  // namespace nekrt
