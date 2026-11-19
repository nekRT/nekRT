// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "utils/output.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <mfem.hpp>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "utils/build_info.h"
#include "utils/input_parameters.h"

namespace nekrt {
namespace {

// Offset of the '|' separator in a FormatField() line.
constexpr std::size_t kSeparatorColumn = kLabelWidth + 1;

// Split formatted output into lines, without the newlines.
std::vector<std::string> Lines(const std::string& text) {
  std::vector<std::string> lines;
  std::istringstream stream(text);
  std::string line;
  while (std::getline(stream, line)) {
    lines.push_back(line);
  }
  return lines;
}

InputParameters LathropParams() {
  InputParameters params;
  params.problem_params.name = "lathrop";
  params.mesh_params.filename = "lathrop.msh";
  return params;
}

}  // namespace

TEST(UtilsOutput, FormatFieldAlignsSeparator) {
  const std::string line = FormatField("Label", "value");
  EXPECT_EQ(line.find('|'), kSeparatorColumn);
  EXPECT_EQ(line, std::string(kLabelWidth - 5, ' ') + "Label | value\n");

  EXPECT_EQ(FormatField("", "value").find('|'), kSeparatorColumn);
}

TEST(UtilsOutput, FormatBannerWidth) {
  const std::string banner = FormatBanner("MESH");
  ASSERT_FALSE(banner.empty());
  EXPECT_EQ(banner.back(), '\n');
  EXPECT_EQ(banner.size() - 1, static_cast<std::size_t>(kLineWidth));
  EXPECT_NE(banner.find(">  MESH  <"), std::string::npos);
}

TEST(UtilsOutput, FormatHeaderLinesArePlainAscii) {
  const std::string header = FormatHeader({"2026-01-01 00:00:00", "host"});
  std::istringstream stream(header);
  std::string line;
  while (std::getline(stream, line)) {
    EXPECT_LE(line.size(), static_cast<std::size_t>(kLineWidth)) << line;
    if (!line.empty()) {
      EXPECT_NE(line.back(), ' ') << "trailing whitespace: " << line;
    }
    for (const char c : line) {
      EXPECT_LT(static_cast<unsigned char>(c), 0x80) << line;
    }
  }
}

TEST(UtilsOutput, FormatHeaderCentersLogo) {
  const std::string header = FormatHeader({"2026-01-01 00:00:00", "host"});
  // The top of the logo, "@==@", should sit in the middle of the line.
  const std::size_t peak = header.find("@==@");
  ASSERT_NE(peak, std::string::npos);
  const std::size_t line_start = header.rfind('\n', peak) + 1;
  const std::size_t center = peak - line_start + 2;
  EXPECT_EQ(center, static_cast<std::size_t>(kLineWidth / 2));
}

TEST(UtilsOutput, FormatHeaderContainsBuildAndRunInfo) {
  const std::string header = FormatHeader({"2026-01-01 00:00:00", "host"});
  EXPECT_NE(header.find(FormatField("Version", kVersion)), std::string::npos);
  EXPECT_NE(header.find(FormatField("Git SHA1", kGitSha)), std::string::npos);
  EXPECT_NE(header.find(FormatField("MFEM Version", MFEM_VERSION_STRING)),
            std::string::npos);
  EXPECT_NE(header.find(FormatField("Date/Time", "2026-01-01 00:00:00")),
            std::string::npos);
  EXPECT_NE(header.find(FormatField("Hostname", "host")), std::string::npos);
}

TEST(UtilsOutput, FormatMeshInfo) {
  const std::string section = FormatMeshInfo(10, 4, 2);
  EXPECT_EQ(section.rfind(FormatBanner("MESH"), 0), 0U);
  EXPECT_NE(section.find(FormatField("Dimension", "2")), std::string::npos);
  EXPECT_NE(section.find(FormatField("Nodes", "10")), std::string::npos);
  EXPECT_NE(section.find(FormatField("Elements", "4")), std::string::npos);
}

TEST(UtilsOutput, SourceIterationColumnsWithEigenvalue) {
  const std::vector<std::string> lines =
      Lines(FormatSourceIterationColumns(true));
  // banner, blank, titles, blank, names, underlines
  ASSERT_EQ(lines.size(), 6U);
  EXPECT_EQ(lines[0] + "\n", FormatBanner("SOURCE ITERATION"));
  EXPECT_TRUE(lines[1].empty());
  EXPECT_TRUE(lines[3].empty());
  const std::size_t separator = lines[5].find('|');
  ASSERT_NE(separator, std::string::npos);
  for (const std::size_t i : {2U, 4U, 5U}) {
    EXPECT_EQ(lines[i].find('|'), separator) << lines[i];
    EXPECT_NE(lines[i].back(), ' ') << lines[i];
  }
  EXPECT_EQ(lines[5].size(), static_cast<std::size_t>(kLineWidth));
  EXPECT_NE(lines[2].find("Power Iteration"), std::string::npos);
  EXPECT_NE(lines[2].find("Scattering Iteration"), std::string::npos);
  EXPECT_NE(lines[4].find("k-eff Error"), std::string::npos);
  EXPECT_NE(lines[4].find("Relative Err."), std::string::npos);
}

TEST(UtilsOutput, SourceIterationColumnsFixedSource) {
  const std::string columns = FormatSourceIterationColumns(false);
  EXPECT_EQ(columns.find("k-eff"), std::string::npos);
  EXPECT_EQ(columns.find('|'), std::string::npos);
  EXPECT_NE(columns.find("Scattering Iteration"), std::string::npos);
}

TEST(UtilsOutput, SourceIterationRowAlignsWithColumns) {
  const ScatterIterationRow scatter{12, 1.5e-5, 2.25e-7};
  const EigenvalueIterationRow eigenvalue{3, 1.0234567891, -4.0e-6};

  // Every cell is right-aligned, so a row ends where the underlines end.
  const std::string fixed_underline =
      Lines(FormatSourceIterationColumns(false)).back();
  const std::string fixed_row = FormatSourceIterationRow(scatter);
  EXPECT_EQ(fixed_row.size() - 1, fixed_underline.size());
  EXPECT_NE(fixed_row.find("1.500000e-05"), std::string::npos);
  EXPECT_NE(fixed_row.find("2.250000e-07"), std::string::npos);

  const std::string full_underline =
      Lines(FormatSourceIterationColumns(true)).back();
  const std::string full_row = FormatSourceIterationRow(eigenvalue, scatter);
  EXPECT_EQ(full_row.size() - 1, full_underline.size());
  EXPECT_EQ(full_row.find('|'), full_underline.find('|'));
  EXPECT_NE(full_row.find("1.02345679"), std::string::npos);
  EXPECT_NE(full_row.find("-4.000000e-06"), std::string::npos);
}

TEST(UtilsOutput, FormatProblem) {
  const std::string section = FormatProblem("lathrop.yaml", LathropParams());
  EXPECT_EQ(section.rfind(FormatBanner("PROBLEM"), 0), 0U);
  EXPECT_NE(section.find(FormatField("Problem", "lathrop")), std::string::npos);
  EXPECT_NE(section.find(FormatField("Input File", "lathrop.yaml")),
            std::string::npos);
  EXPECT_NE(section.find(FormatField("Mesh File", "lathrop.msh")),
            std::string::npos);
  EXPECT_TRUE(section.ends_with("\n\n"));
}

TEST(UtilsOutput, GetRunInfoHasExpectedShape) {
  const RunInfo run_info = GetRunInfo();
  EXPECT_FALSE(run_info.hostname.empty());
  // The values differ on every run, so only check their form.
  EXPECT_TRUE(std::regex_match(
      run_info.date_time, std::regex(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})")))
      << run_info.date_time;
}

TEST(UtilsOutput, PrintHeader) {
  testing::internal::CaptureStdout();
  PrintHeader();
  const std::string printed = testing::internal::GetCapturedStdout();

  // The date and time change, so compare the shape instead of the whole text.
  EXPECT_NE(printed.find("Spectral Elements for Radiation Transport"),
            std::string::npos);
  EXPECT_NE(printed.find(FormatField("Version", kVersion)), std::string::npos);
  EXPECT_TRUE(printed.ends_with('\n'));
  EXPECT_EQ(Lines(printed).size(),
            Lines(FormatHeader({"2026-01-01 00:00:00", "host"})).size());
}

TEST(UtilsOutput, PrintProblem) {
  const InputParameters params = LathropParams();
  testing::internal::CaptureStdout();
  PrintProblem("lathrop.yaml", params);
  EXPECT_EQ(testing::internal::GetCapturedStdout(),
            FormatProblem("lathrop.yaml", params));
}

TEST(UtilsOutput, PrintMeshInfo) {
  testing::internal::CaptureStdout();
  PrintMeshInfo(10, 4, 2);
  EXPECT_EQ(testing::internal::GetCapturedStdout(), FormatMeshInfo(10, 4, 2));
}

TEST(UtilsOutput, PrintSourceIterationColumns) {
  for (const bool include_eigenvalue : {false, true}) {
    testing::internal::CaptureStdout();
    PrintSourceIterationColumns(include_eigenvalue);
    EXPECT_EQ(testing::internal::GetCapturedStdout(),
              FormatSourceIterationColumns(include_eigenvalue))
        << "include_eigenvalue = " << include_eigenvalue;
  }
}

TEST(UtilsOutput, PrintSourceIterationRow) {
  const ScatterIterationRow scatter{12, 1.5e-5, 2.25e-7};
  const EigenvalueIterationRow eigenvalue{3, 1.0234567891, -4.0e-6};

  testing::internal::CaptureStdout();
  PrintSourceIterationRow(scatter);
  EXPECT_EQ(testing::internal::GetCapturedStdout(),
            FormatSourceIterationRow(scatter));

  testing::internal::CaptureStdout();
  PrintSourceIterationRow(eigenvalue, scatter);
  EXPECT_EQ(testing::internal::GetCapturedStdout(),
            FormatSourceIterationRow(eigenvalue, scatter));
}

}  // namespace nekrt
