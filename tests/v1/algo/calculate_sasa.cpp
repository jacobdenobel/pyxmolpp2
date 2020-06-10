#include <gtest/gtest.h>

#include "xmol/v1/algo/sasa.h"
#include "xmol/v1/io/PdbInputFile.h"
#include <chrono>
#include <numeric>

using ::testing::Test;
using namespace xmol::v1::geom;
using namespace xmol::v1::io;
using namespace xmol::v1::algo;
using namespace xmol::v1::future;

class calculate_sasa_Tests : public Test {
public:
};

TEST_F(calculate_sasa_Tests, test1) {
  std::vector<double> radii = {1.0, 1.0};
  std::vector<XYZ> coords = {XYZ(0, 0, 0), XYZ(0, 0, 1)};
  auto result = calc_sasa(coords,  Span(radii.data(),radii.size()), 0.0);
  EXPECT_EQ(result.size(), coords.size());
  EXPECT_DOUBLE_EQ(result[0], result[1]);
}

TEST_F(calculate_sasa_Tests, calc_sasa_on_pdb) {
  auto files = {
      "pdb/rcsb/1PGB.pdb",
      "pdb/rcsb/5BMG.pdb",
      "pdb/rcsb/5BMH.pdb",
  };
  for (auto& file : files) {
    auto frame = PdbInputFile(file).frames()[0];
    auto atoms = frame.atoms();
    auto _coords = frame.coords();
    std::vector<XYZ> coords(_coords.begin(),_coords.end());
    std::vector<double> radii(atoms.size(), 1.0);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto sasa = calc_sasa(coords, Span(radii.data(), radii.size()), 0.0, 20);
    auto t2 = std::chrono::high_resolution_clock::now();
    double total_sasa = std::accumulate(sasa.begin(), sasa.end(), 0.0);
    std::cout << "SASA= " << total_sasa << " ("
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms)" << std::endl;
  }
}
