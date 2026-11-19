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
#include <vector>

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

// Source iteration table layout. Each group of columns is
// kTableGroupWidth wide, so a row with both groups is
// " " + group + " | " + group = kLineWidth wide.
struct TableColumn {
  std::string_view name;
  int width;
};

struct TableGroup {
  std::string_view title;
  std::array<TableColumn, 3> columns;
};

constexpr std::string_view kColumnGap = "  ";
constexpr std::string_view kGroupSeparator = " | ";
constexpr int kIterationWidth = 8;
constexpr int kValueWidth = 13;

constexpr TableGroup kEigenvalueGroup{"Power Iteration",
                                      {{{"Iter.", kIterationWidth},
                                        {"k-eff", kValueWidth},
                                        {"k-eff Error", kValueWidth}}}};

constexpr TableGroup kScatterGroup{"Scattering Iteration",
                                   {{{"Iter.", kIterationWidth},
                                     {"Flux Error", kValueWidth},
                                     {"Relative Err.", kValueWidth}}}};

constexpr int GroupWidth(const TableGroup& group) {
  int width = static_cast<int>(kColumnGap.size()) *
              static_cast<int>(group.columns.size() - 1);
  for (const TableColumn& column : group.columns) {
    width += column.width;
  }
  return width;
}

constexpr int kTableGroupWidth = GroupWidth(kScatterGroup);
static_assert(GroupWidth(kEigenvalueGroup) == kTableGroupWidth);
static_assert(1 + 2 * kTableGroupWidth +
                  static_cast<int>(kGroupSeparator.size()) ==
              kLineWidth);

// The three heading lines of one column group, each kTableGroupWidth wide.
struct GroupHeader {
  std::string title;
  std::string names;
  std::string underline;
};

GroupHeader FormatGroupHeader(const TableGroup& group) {
  GroupHeader header;
  header.title = fmt::format("{:^{}}", group.title, kTableGroupWidth);
  for (std::size_t i = 0; i < group.columns.size(); ++i) {
    const TableColumn& column = group.columns[i];
    if (i > 0) {
      header.names += kColumnGap;
      header.underline += kColumnGap;
    }
    header.names += fmt::format("{:>{}}", column.name, column.width);
    header.underline +=
        std::string(static_cast<std::size_t>(column.width), '=');
  }
  return header;
}

std::string FormatEigenvalueCells(const EigenvalueIterationRow& row) {
  const auto& columns = kEigenvalueGroup.columns;
  return fmt::format("{:>{}}{}{:>{}.8f}{}{:>{}.6e}", row.iteration,
                     columns[0].width, kColumnGap, row.k_eff, columns[1].width,
                     kColumnGap, row.k_eff_error, columns[2].width);
}

std::string FormatScatterCells(const ScatterIterationRow& row) {
  const auto& columns = kScatterGroup.columns;
  return fmt::format("{:>{}}{}{:>{}.6e}{}{:>{}.6e}", row.iteration,
                     columns[0].width, kColumnGap, row.flux_error,
                     columns[1].width, kColumnGap, row.relative_error,
                     columns[2].width);
}

// Join the column groups of one line with kGroupSeparator, indent by one
// space to match FormatBanner(), and strip trailing whitespace.
std::string JoinGroups(const std::vector<std::string>& groups) {
  std::string line = " ";
  for (std::size_t i = 0; i < groups.size(); ++i) {
    if (i > 0) {
      line += kGroupSeparator;
    }
    line += groups[i];
  }
  line.erase(line.find_last_not_of(' ') + 1);
  line += '\n';
  return line;
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

std::string FormatSourceIterationColumns(const bool include_eigenvalue) {
  std::string section = FormatBanner("SOURCE ITERATION");
  section += '\n';
  std::vector<GroupHeader> groups;
  if (include_eigenvalue) {
    groups.push_back(FormatGroupHeader(kEigenvalueGroup));
  }
  groups.push_back(FormatGroupHeader(kScatterGroup));

  std::vector<std::string> titles;
  std::vector<std::string> names;
  std::vector<std::string> underlines;
  for (const GroupHeader& group : groups) {
    titles.push_back(group.title);
    names.push_back(group.names);
    underlines.push_back(group.underline);
  }
  section += JoinGroups(titles);
  section += '\n';
  section += JoinGroups(names);
  section += JoinGroups(underlines);
  return section;
}

std::string FormatSourceIterationRow(const ScatterIterationRow& scatter) {
  return JoinGroups({FormatScatterCells(scatter)});
}

std::string FormatSourceIterationRow(const EigenvalueIterationRow& eigenvalue,
                                     const ScatterIterationRow& scatter) {
  return JoinGroups(
      {FormatEigenvalueCells(eigenvalue), FormatScatterCells(scatter)});
}

void PrintSourceIterationColumns(const bool include_eigenvalue) {
  fmt::print("{}", FormatSourceIterationColumns(include_eigenvalue));
}

void PrintSourceIterationRow(const ScatterIterationRow& scatter) {
  fmt::print("{}", FormatSourceIterationRow(scatter));
}

void PrintSourceIterationRow(const EigenvalueIterationRow& eigenvalue,
                             const ScatterIterationRow& scatter) {
  fmt::print("{}", FormatSourceIterationRow(eigenvalue, scatter));
}

}  // namespace nekrt
