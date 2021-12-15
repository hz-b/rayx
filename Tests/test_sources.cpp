#include "setupTests.h"

#if RUN_TEST_SOURCES

#include <fstream>
#include <sstream>

#include "Core.h"
#include "Model/Beamline/Beamline.h"
#include "Model/Beamline/Objects/MatrixSource.h"
#include "Model/Beamline/Objects/PointSource.h"
#include "Presenter/SimulationEnv.h"
#include "Tracer/Ray.h"
#include "Tracer/Vulkan/VulkanTracer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

void writeRaysToFile(std::list<double> outputRays, std::string name) {
    std::cout << "writing to file..." << std::endl;
    std::ofstream outputFile;
    outputFile.precision(17);
    std::cout.precision(17);
    std::string filename = "../../Tests/output/";
    filename.append(name);
    filename.append(".csv");
    outputFile.open(filename);
    char sep = ';';  // file is saved in .csv (comma seperated value), excel
                     // compatibility is manual right now
    outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep << "Zloc"
               << sep << "Weight" << sep << "Xdir" << sep << "Ydir" << sep
               << "Zdir" << sep << "Energy" << sep << "S0" << sep << "S1" << sep
               << "S2" << sep << "S3" << sep << "pathLength" << sep << "order"
               << sep << "lastElement" << sep << "extraParam" << std::endl;

    size_t counter = 0;
    int print = 0;  // whether to print on std::out (0=no, 1=yes)
    for (std::list<double>::iterator i = outputRays.begin();
         i != outputRays.end(); i++) {
        if (counter % RAY_DOUBLE_COUNT == 0) {
            outputFile << counter / RAY_DOUBLE_COUNT;
            if (print == 1) std::cout << ")" << std::endl;
            if (print == 1) std::cout << "(";
        }
        outputFile << sep << *i;
        if (counter % RAY_DOUBLE_COUNT == RAY_DOUBLE_COUNT - 1) {
            outputFile << std::endl;
            counter++;
            continue;
        }
        if (counter % RAY_DOUBLE_COUNT == 3) {
            if (print == 1) std::cout << ") ";
        } else if (counter % RAY_DOUBLE_COUNT == 4) {
            if (print == 1) std::cout << " (";
        } else if (counter % RAY_DOUBLE_COUNT != 0) {
            if (print == 1) std::cout << ", ";
        }
        if (print == 1) std::cout << *i;
        counter++;
    }
    if (print == 1) std::cout << ")" << std::endl;
    outputFile.close();
    std::cout << "done!" << std::endl;
}

TEST(RayTest, test1) {
    // arrange
    // act
    // assert
    double x = 0.2;
    double y = 0.4;
    double z = 0.1;
    double xdir = 0.25;
    double ydir = 0.05;
    double zdir = 0.99;
    double weight = 1.0;
    double energy = 120.0;
    double s0 = 1;
    double s1 = 1;
    double s2 = 0;
    double s3 = 0;
    double pathLength = 15;
    double order = 3;
    double lastElement = 4;
    double extraParam = 7;
    RAYX::Ray r{x, y, z, weight, xdir, ydir, zdir, energy, s0, s1, s2, s3,
                pathLength, order, lastElement, extraParam};
    EXPECT_EQ(r.getxPos(), x);
    EXPECT_EQ(r.getyPos(), y);
    EXPECT_EQ(r.getzPos(), z);
    EXPECT_EQ(r.getxDir(), xdir);
    EXPECT_EQ(r.getyDir(), ydir);
    EXPECT_EQ(r.getzDir(), zdir);
    EXPECT_EQ(r.getWeight(), weight);
    EXPECT_EQ(r.getEnergy(), energy);
    EXPECT_EQ(r.getS0(), s0);
    EXPECT_EQ(r.getS1(), s1);
    EXPECT_EQ(r.getS2(), s2);
    EXPECT_EQ(r.getS3(), s3);
    EXPECT_EQ(r.getPathLength(), pathLength);
    EXPECT_EQ(r.getOrder(), order);
    EXPECT_EQ(r.getLastElement(), lastElement);
    EXPECT_EQ(r.getExtraParam(), extraParam);
}

TEST(RayTest, testDefaultValues) {
    // arrange
    // act
    // assert
    double x = 0.2;
    double y = 0.4;
    double z = 0.1;
    double xdir = 0.25;
    double ydir = 0.05;
    double zdir = 0.99;
    double weight = 1.0;
    double energy = 120.0;
    double s0 = 1;
    double s1 = 1;
    double s2 = 0;
    double s3 = 0;
    RAYX::Ray r{x, y, z, weight, xdir, ydir, zdir, energy, s0, s1, s2, s3, 0, 0, 0, 0};
    EXPECT_EQ(r.getxPos(), x);
    EXPECT_EQ(r.getyPos(), y);
    EXPECT_EQ(r.getzPos(), z);
    EXPECT_EQ(r.getxDir(), xdir);
    EXPECT_EQ(r.getyDir(), ydir);
    EXPECT_EQ(r.getzDir(), zdir);
    EXPECT_EQ(r.getWeight(), weight);
    EXPECT_EQ(r.getEnergy(), energy);
    EXPECT_EQ(r.getS0(), s0);
    EXPECT_EQ(r.getS1(), s1);
    EXPECT_EQ(r.getS2(), s2);
    EXPECT_EQ(r.getS3(), s3);
    EXPECT_EQ(r.getPathLength(), 0);
    EXPECT_EQ(r.getOrder(), 0);
    EXPECT_EQ(r.getLastElement(), 0);
    EXPECT_EQ(r.getExtraParam(), 0);
}

TEST(MatrixTest, testParams) {
    int n = RAYX::SimulationEnv::get().m_numOfRays;
    double width = 0.065;
    double height = 0.04;
    double depth = 0.0;
    double verdiv = 0.001;
    double hordiv = 0.001;
    double photonEnergy = 100;
    double energySpread = 10;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    std::vector<double> mis = {0, 0, 0, 0};
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  true);
    RAYX::MatrixSource m =
        RAYX::MatrixSource("Matrix source 1", dist, width, height, depth,
                           hordiv, verdiv, lin0, lin45, circ, mis);

    // ASSERT_DOUBLE_EQ(m.m_ID, id);
    ASSERT_DOUBLE_EQ(RAYX::SimulationEnv::get().m_numOfRays, n);
    ASSERT_DOUBLE_EQ(m.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(m.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(m.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(m.getHorDivergence(), hordiv);
    ASSERT_DOUBLE_EQ(m.getVerDivergence(), verdiv);
}

TEST(MatrixTest, testGetRays) {
    int n = 1000;
    RAYX::SimulationEnv::get().m_numOfRays = n;
    double width = 0.065;
    double height = 0.04;
    double depth = 0.0;
    double verdiv = 0.001;
    double hordiv = 0.001;
    double photonEnergy = 200;
    double energySpread = 10;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  true);
    RAYX::MatrixSource m =
        RAYX::MatrixSource("Matrix source 1", dist, width, height, depth,
                           hordiv, verdiv, lin0, lin45, circ, {0, 0, 0, 0});
    std::vector<RAYX::Ray> rays = m.getRays();

    ASSERT_EQ(rays.size(), n);
}

TEST(PointSource, testParams) {
    int number_of_rays = 100;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    double photonEnergy = 120;
    double energySpread = 20;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    std::vector<double> misalignment = {0, 0, 0, 0};
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  false);
    RAYX::PointSource p = RAYX::PointSource(
        "Point source 1", dist, width, height, depth, hor, ver, 0, 0, 0, 0,
        lin0, lin45, circ, misalignment);  // 0 = hard edge (uniform)

    ASSERT_DOUBLE_EQ(p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ(p.getVerDivergence(), ver);

    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
}

TEST(LightSource, PointSourceHardEdge) {
    int number_of_rays = 10000;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    double photonEnergy = 120.97;
    double energySpread = 12.1;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    std::vector<double> mis = {0, 0, 0, 0};
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  true);
    RAYX::PointSource p = RAYX::PointSource(
        "Point source 1", dist, width, height, depth, hor, ver, 0, 0, 0, 0,
        lin0, lin45, circ, mis);  // 0 = hard edge (uniform)

    ASSERT_DOUBLE_EQ(p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ(p.getVerDivergence(), ver);
    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(r.m_energy);
        rayList.push_back(r.m_stokes.x);
        rayList.push_back(r.m_stokes.y);
        rayList.push_back(r.m_stokes.z);
        rayList.push_back(r.m_stokes.w);
        rayList.push_back(r.m_pathLength);
        rayList.push_back(r.m_order);
        rayList.push_back(r.m_lastElement);
        rayList.push_back(r.m_extraParam);
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    ASSERT_EQ(rayList.size(), number_of_rays * RAY_DOUBLE_COUNT);
    writeRaysToFile(rayList, "pointSourceHE");
}

TEST(LightSource, PointSourceSoftEdge) {
    int number_of_rays = 10000;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    double photonEnergy = 120;
    double energySpread = 0;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    std::vector<double> mis = {0, 0, 0, 0};
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  true);
    RAYX::PointSource p = RAYX::PointSource(
        "Point source 1", dist, width, height, depth, hor, ver, 1, 1, 1, 1,
        lin0, lin45, circ, mis);  // 1 = soft edge (gaussian)

    ASSERT_DOUBLE_EQ(p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ(p.getVerDivergence(), ver);
    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(r.m_energy);
        rayList.push_back(r.m_stokes.x);
        rayList.push_back(r.m_stokes.y);
        rayList.push_back(r.m_stokes.z);
        rayList.push_back(r.m_stokes.w);
        rayList.push_back(r.m_pathLength);
        rayList.push_back(r.m_order);
        rayList.push_back(r.m_lastElement);
        rayList.push_back(r.m_extraParam);
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    ASSERT_EQ(rayList.size(), number_of_rays * RAY_DOUBLE_COUNT);
    writeRaysToFile(rayList, "pointSourceSE");
}

TEST(LightSource, PointSourceHardEdgeMis) {
    int number_of_rays = 10000;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    double photonEnergy = 151;
    double energySpread = 6;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    std::vector<double> mis = {2, 3, 0.005, 0.006};  // x, y, psi, phi
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  false);
    RAYX::PointSource p = RAYX::PointSource(
        "Point source 1", dist, width, height, depth, hor, ver, 0, 0, 0, 0,
        lin0, lin45, circ, mis);  // 0 = hard edge (uniform)

    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(r.m_energy);
        rayList.push_back(r.m_stokes.x);
        rayList.push_back(r.m_stokes.y);
        rayList.push_back(r.m_stokes.z);
        rayList.push_back(r.m_stokes.w);
        rayList.push_back(r.m_pathLength);
        rayList.push_back(r.m_order);
        rayList.push_back(r.m_lastElement);
        rayList.push_back(r.m_extraParam);
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    ASSERT_EQ(rayList.size(), number_of_rays * RAY_DOUBLE_COUNT);
    writeRaysToFile(rayList, "pointSourceHE_mis");
}

TEST(LightSource, PointSourceSoftEdgeMis) {
    int number_of_rays = 10000;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    double photonEnergy = 130;
    double energySpread = 10;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    std::vector<double> mis = {2, 3, 0.005, 0.006};  // x,y,psi,phi in rad
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  false);
    RAYX::PointSource p = RAYX::PointSource(
        "Point source 1", dist, width, height, depth, hor, ver, 1, 1, 1, 1,
        lin0, lin45, circ, mis);  // 1 = soft edge (gaussian)

    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(r.m_energy);
        rayList.push_back(r.m_stokes.x);
        rayList.push_back(r.m_stokes.y);
        rayList.push_back(r.m_stokes.z);
        rayList.push_back(r.m_stokes.w);
        rayList.push_back(r.m_pathLength);
        rayList.push_back(r.m_order);
        rayList.push_back(r.m_lastElement);
        rayList.push_back(r.m_extraParam);
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    ASSERT_EQ(rayList.size(), number_of_rays * RAY_DOUBLE_COUNT);
    writeRaysToFile(rayList, "pointSourceSE_mis");
}

TEST(LightSource, MatrixSource20000) {
    int number_of_rays = 20000;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double photonEnergy = 120;
    double energySpread = 20;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(photonEnergy, energySpread),
                                  false);
    RAYX::MatrixSource p =
        RAYX::MatrixSource("Matrix20", dist, 0.065, 0.04, 0.0, 0.001, 0.001,
                           lin0, lin45, circ, {0, 0, 0, 0});

    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(r.m_energy);
        rayList.push_back(r.m_stokes.x);
        rayList.push_back(r.m_stokes.y);
        rayList.push_back(r.m_stokes.z);
        rayList.push_back(r.m_stokes.w);
        rayList.push_back(r.m_pathLength);
        rayList.push_back(r.m_order);
        rayList.push_back(r.m_lastElement);
        rayList.push_back(r.m_extraParam);
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    ASSERT_EQ(rayList.size(), number_of_rays * RAY_DOUBLE_COUNT);
    writeRaysToFile(rayList, "matrixsource20000");
}

TEST(LightSource, PointSource20000) {
    int number_of_rays = 20000;
    RAYX::SimulationEnv::get().m_numOfRays = number_of_rays;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    RAYX::EnergyDistribution dist(RAYX::EnergyRange(640, 120), false);
    RAYX::PointSource p =
        RAYX::PointSource("spec1_first_rzp4", dist, 0.005, 0.005, 0, 0.02, 0.06,
                          1, 1, 0, 0, lin0, lin45, circ, {0, 0, 0, 0});

    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(r.m_energy);
        rayList.push_back(r.m_stokes.x);
        rayList.push_back(r.m_stokes.y);
        rayList.push_back(r.m_stokes.z);
        rayList.push_back(r.m_stokes.w);
        rayList.push_back(r.m_pathLength);
        rayList.push_back(r.m_order);
        rayList.push_back(r.m_lastElement);
        rayList.push_back(r.m_extraParam);
    }
    std::cout << rayList.size() << std::endl;
    ASSERT_EQ(rayList.size(), number_of_rays * RAY_DOUBLE_COUNT);
    writeRaysToFile(rayList, "pointsource20000");
}

#endif