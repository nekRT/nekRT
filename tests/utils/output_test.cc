// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "utils/output.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <mfem.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "utils/build_info.h"

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
  ASSERT_EQ(lines.size(), 5U);  // banner, blank, titles, names, underlines
  EXPECT_EQ(lines[0] + "\n", FormatBanner("SOURCE ITERATION"));
  const std::size_t separator = lines[4].find('|');
  ASSERT_NE(separator, std::string::npos);
  for (std::size_t i = 2; i < lines.size(); ++i) {
    EXPECT_EQ(lines[i].find('|'), separator) << lines[i];
    EXPECT_NE(lines[i].back(), ' ') << lines[i];
  }
  EXPECT_EQ(lines[4].size(), static_cast<std::size_t>(kLineWidth));
  EXPECT_NE(lines[2].find("Power Iteration"), std::string::npos);
  EXPECT_NE(lines[3].find("k-eff Error"), std::string::npos);
  EXPECT_NE(lines[3].find("Relative Err."), std::string::npos);
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

}  // namespace nekrt
