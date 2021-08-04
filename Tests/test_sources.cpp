#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Beamline/Beamline.h"
#include "Beamline/Objects/MatrixSource.h"
#include "Beamline/Objects/PointSource.h"

#include "Core.h"
#include "Ray.h"
#include "VulkanTracer.h"
#include <fstream>
#include <sstream>


void writeRaysToFile(std::list<double> outputRays, std::string name)
{
    std::cout << "writing to file..." << std::endl;
    std::ofstream outputFile;
    outputFile.precision(17);
    std::cout.precision(17);
    std::string filename = "../../Tests/output/";
    filename.append(name);
    filename.append(".csv");
    outputFile.open(filename);
    char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
    outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep << "Zloc" << sep << "Weight" << sep << "Xdir" << sep << "Ydir" << sep << "Zdir" << sep << "Energy" << std::endl;
    // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;

    size_t counter = 0;
    int print = 0; // whether to print on std::out (0=no, 1=yes)
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end(); i++) {
        if (counter % 8 == 0) {
            outputFile << counter / VULKANTRACER_RAY_DOUBLE_AMOUNT;
            if (print == 1) std::cout << ")" << std::endl;
            if (print == 1) std::cout << "(";
        }
        outputFile << sep << *i;
        if (counter % 8 == 7) {
            outputFile << std::endl;
            counter++;
            continue;
        }
        if (counter % 8 == 3) {
            if (print == 1) std::cout << ") ";
        }        
else if (counter % 8 == 4) {
            if (print == 1) std::cout << " (";
        }        
else if (counter % 8 != 0) {
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
    //arrange
    //act
    //assert
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
    Ray r = Ray(x, y, z, xdir, ydir, zdir, energy, weight);
    EXPECT_EQ(r.getxPos(), x);
    EXPECT_EQ(r.getyPos(), y);
    EXPECT_EQ(r.getzPos(), z);
    EXPECT_EQ(r.getxDir(), xdir);
    EXPECT_EQ(r.getyDir(), ydir);
    EXPECT_EQ(r.getzDir(), zdir);
    EXPECT_EQ(r.getWeight(), weight);
    EXPECT_EQ(r.getEnergy(), energy);
}


TEST(MatrixTest, testParams) {
    int n = 100;
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
    int id = 3;
    int spreadType = 0;
    std::vector<double> mis = { 0,0,0,0 };
    RAYX::MatrixSource m = RAYX::MatrixSource(id, "Matrix source 1", n, spreadType, width, height, depth, hordiv, verdiv, photonEnergy, energySpread, lin0, lin45, circ, mis);

    ASSERT_DOUBLE_EQ(m.getId(), id);
    ASSERT_DOUBLE_EQ(m.getNumberOfRays(), n);
    ASSERT_DOUBLE_EQ(m.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(m.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(m.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(m.getHorDivergence(), hordiv);
    ASSERT_DOUBLE_EQ(m.getVerDivergence(), verdiv);
    ASSERT_DOUBLE_EQ(m.getEnergySpread(), energySpread);
    ASSERT_DOUBLE_EQ(m.getPhotonEnergy(), photonEnergy);
    ASSERT_EQ(m.getSpreadType(), spreadType);
}

TEST(MatrixTest, testGetRays) {
    int n = 100;
    double width = 0.065;
    double height = 0.04;
    double depth = 0.0;
    double verdiv = 0.001;
    double hordiv = 0.001;
    int id = 3;
    double photonEnergy = 200;
    double energySpread = 10;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    int spreadType = 0;
    RAYX::MatrixSource m = RAYX::MatrixSource(id, "Matrix source 1", n, spreadType, width, height, depth, hordiv, verdiv, photonEnergy, energySpread, lin0, lin45, circ, { 0,0,0,0 });
    std::vector<RAYX::Ray> rays = m.getRays();

    ASSERT_EQ(rays.size(), n);
}

TEST(PointSource, testParams) {
    int number_of_rays = 1000;
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
    int spreadType = 1;
    std::vector<double> misalignment = { 0,0,0,0 };
    RAYX::PointSource p = RAYX::PointSource(0, "Point source 1", number_of_rays, spreadType, width, height, depth, hor, ver, 0, 0, 0, 0, photonEnergy, energySpread, lin0, lin45, circ, misalignment); // 0 = hard edge (uniform)

    ASSERT_DOUBLE_EQ(p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ(p.getVerDivergence(), ver);
    ASSERT_DOUBLE_EQ(p.getPhotonEnergy(), photonEnergy);
    ASSERT_DOUBLE_EQ(p.getEnergySpread(), energySpread);
    ASSERT_EQ(p.getSpreadType(), spreadType);

    std::vector<RAYX::Ray> rays = p.getRays();
    ASSERT_EQ(rays.size(), number_of_rays);
}


TEST(LightSource, PointSourceHardEdge) {
    int number_of_rays = 10000;
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
    int spreadType = 0;
    std::vector<double> mis = { 0,0,0,0 };
    RAYX::PointSource p = RAYX::PointSource(0, "Point source 1", number_of_rays, spreadType, width, height, depth, hor, ver, 0, 0, 0, 0, photonEnergy, energySpread, lin0, lin45, circ, mis); // 0 = hard edge (uniform)

    ASSERT_DOUBLE_EQ(p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ(p.getVerDivergence(), ver);
    std::vector<RAYX::Ray> rays = p.getRays();
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
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceHE");
}

TEST(LightSource, PointSourceSoftEdge) {
    int number_of_rays = 10000;
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
    int spreadType = 0;
    std::vector<double> mis = { 0,0,0,0 };
    RAYX::PointSource p = RAYX::PointSource(0, "Point source 1", number_of_rays, spreadType, width, height, depth, hor, ver, 1, 1, 1, 1, photonEnergy, energySpread, lin0, lin45, circ, mis); // 1 = soft edge (gaussian)

    ASSERT_DOUBLE_EQ(p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ(p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ(p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ(p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ(p.getVerDivergence(), ver);
    std::vector<RAYX::Ray> rays = p.getRays();
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
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceSE");
}


TEST(LightSource, PointSourceHardEdgeMis) {
    int number_of_rays = 10000;
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
    int spreadType = 1;
    std::vector<double> mis = { 2,3,0.005,0.006 }; // x, y, psi, phi
    RAYX::PointSource p = RAYX::PointSource(0, "Point source 1", number_of_rays, spreadType, width, height, depth, hor, ver, 0, 0, 0, 0, photonEnergy, energySpread, lin0, lin45, circ, mis); // 0 = hard edge (uniform)

    std::vector<RAYX::Ray> rays = p.getRays();
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
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceHE_mis");
}

TEST(LightSource, PointSourceSoftEdgeMis) {
    int number_of_rays = 10000;
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
    int spreadType = 1;
    std::vector<double> mis = { 2,3,0.005,0.006 }; // x,y,psi,phi in rad
    RAYX::PointSource p = RAYX::PointSource(0, "Point source 1", number_of_rays, spreadType, width, height, depth, hor, ver, 1, 1, 1, 1, photonEnergy, energySpread, lin0, lin45, circ, mis); // 1 = soft edge (gaussian)

    std::vector<RAYX::Ray> rays = p.getRays();
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
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceSE_mis");
}

TEST(LightSource, MatrixSource20000) {
    int number_of_rays = 20000;
    double photonEnergy = 120;
    double energySpread = 20;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    int spreadType = 1;
    RAYX::MatrixSource p = RAYX::MatrixSource(0, "Matrix20", number_of_rays, spreadType, 0.065, 0.04, 0.0, 0.001, 0.001, photonEnergy, energySpread, lin0, lin45, circ, { 0,0,0,0 });

    std::vector<RAYX::Ray> rays = p.getRays();
    std::list<double> rayList;
    for (RAYX::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        ASSERT_TRUE(r.m_energy >= photonEnergy - (energySpread / 2));
        ASSERT_TRUE(r.m_energy <= photonEnergy + (energySpread / 2));
        rayList.push_back(r.m_energy);
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "matrixsource20000");
}

TEST(LightSource, PointSource20000) {
    int number_of_rays = 20000;
    double photonEnergy = 120;
    double energySpread = 20;
    double lin0 = 1;
    double lin45 = 0;
    double circ = 0;
    int spreadType = 1;
    RAYX::PointSource p = RAYX::PointSource(0, "spec1_first_rzp4", number_of_rays, 1, 0.005, 0.005, 0, 0.02, 0.06, 1, 1, 0, 0, 640, 120, lin0, lin45, circ, { 0,0,0,0 });

    std::vector<RAYX::Ray> rays = p.getRays();
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
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointsource20000");
}
