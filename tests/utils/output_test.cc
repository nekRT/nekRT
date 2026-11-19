// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, nekRT

#include "utils/output.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <mfem.hpp>
#include <sstream>
#include <string>

#include "utils/build_info.h"

namespace nekrt {
namespace {

// Offset of the '|' separator in a FormatField() line.
constexpr std::size_t kSeparatorColumn = kLabelWidth + 1;

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

}  // namespace nekrt
