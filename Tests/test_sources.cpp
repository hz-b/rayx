#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Beamline/Beamline.h"
#include "Beamline/MatrixSource.h"
#include "Beamline/PointSource.h"

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
    std::cout.precision (17);
    std::string filename = "../../output/";
    filename.append(name);
    filename.append(".csv");
    outputFile.open(filename);
    char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
    outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep<<"Zloc"<<sep<<"Weight"<<sep<<"Xdir"<<sep<<"Ydir"<<sep<<"Zdir" << std::endl;
    // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;
    
    size_t counter = 0;
    int print = 0; // whether to print on std::out (0=no, 1=yes)
    for (std::list<double>::iterator i=outputRays.begin(); i != outputRays.end(); i++){
        if(counter%8 == 0){
            outputFile << counter/VULKANTRACER_RAY_DOUBLE_AMOUNT;
            if(print==1) std::cout << ")" << std::endl;
            if(print==1) std::cout << "(";
        }
        if(counter%8 == 7){
            outputFile << std::endl;
            counter++;
            continue;
        }
        outputFile << sep << *i ;
        if(counter%8 == 3) {
            if(print==1) std::cout << ") ";
        }else if(counter%8 == 4) {
            if(print==1) std::cout << " (";
        }else if(counter %8 != 0){
            if(print==1) std::cout <<", ";
        }
        if(print==1) std::cout << *i;
        counter++;
    }
    if(print==1) std::cout << ")" << std::endl;
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
    Ray r = Ray(x,y,z,xdir,ydir,zdir,weight);
    EXPECT_EQ (r.getxPos(),  x);
    EXPECT_EQ (r.getyPos(),  y);
    EXPECT_EQ (r.getzPos(),  z);
    EXPECT_EQ (r.getxDir(),  xdir);
    EXPECT_EQ (r.getyDir(),  ydir);
    EXPECT_EQ (r.getzDir(),  zdir);
    EXPECT_EQ (r.getWeight(),  weight);
}


TEST(MatrixTest, testParams) {
    int n = 100;
    double width = 0.065;
    double height = 0.04;
    double depth = 0.0;
    double verdiv = 0.001;
    double hordiv = 0.001;
    int id = 3;
    std::vector<double> mis = {0,0,0,0};
    RAY::MatrixSource m = RAY::MatrixSource(id, "Matrix source 1", n, width, height, depth, hordiv, verdiv, mis);
    
    ASSERT_DOUBLE_EQ (m.getId(), id);
    ASSERT_DOUBLE_EQ (m.getNumberOfRays(), n);
    ASSERT_DOUBLE_EQ (m.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ (m.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ (m.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ (m.getHorDivergence(), hordiv);
    ASSERT_DOUBLE_EQ (m.getVerDivergence(), verdiv);
}

TEST (MatrixTest, testGetRays) {
    int n = 100;
    double width = 0.065;
    double height = 0.04;
    double depth = 0.0;
    double verdiv = 0.001;
    double hordiv = 0.001;
    int id = 3;
    RAY::MatrixSource m = RAY::MatrixSource(id, "Matrix source 1", n, width, height, depth, hordiv, verdiv, {0,0,0,0});
    std::vector<RAY::Ray> rays = m.getRays();

    EXPECT_EQ (rays.size(), n);
}

TEST (PointSource, testParams) {
    int number_of_rays = 1000;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    std::vector<double> misalignment = {0,0,0,0};
    RAY::PointSource p = RAY::PointSource(0, "Point source 1", number_of_rays, width, height, depth, hor, ver, 0,0,0,0, misalignment); // 0 = hard edge (uniform)
        
    ASSERT_DOUBLE_EQ (p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ (p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ (p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ (p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ (p.getVerDivergence(), ver);

    std::vector<RAY::Ray> rays = p.getRays();
    EXPECT_EQ (rays.size(), number_of_rays);
}


TEST (LightSource, PointSourceHardEdge) {
    int number_of_rays = 10000;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    std::vector<double> mis = {0,0,0,0};
    RAY::PointSource p = RAY::PointSource(0, "Point source 1", number_of_rays, width, height, depth, hor, ver, 0,0,0,0, mis); // 0 = hard edge (uniform)
        
    ASSERT_DOUBLE_EQ (p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ (p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ (p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ (p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ (p.getVerDivergence(), ver);
    std::vector<RAY::Ray> rays = p.getRays();
    std::list<double> rayList;
    for (RAY::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(100);
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceHE");
}

TEST (LightSource, PointSourceSoftEdge) {
    int number_of_rays = 10000;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    std::vector<double> mis = {0,0,0,0};
    RAY::PointSource p = RAY::PointSource(0, "Point source 1", number_of_rays, width, height, depth, hor, ver, 1,1,1,1, mis); // 1 = soft edge (gaussian)
        
    ASSERT_DOUBLE_EQ (p.getSourceDepth(), depth);
    ASSERT_DOUBLE_EQ (p.getSourceWidth(), width);
    ASSERT_DOUBLE_EQ (p.getSourceHeight(), height);
    ASSERT_DOUBLE_EQ (p.getHorDivergence(), hor);
    ASSERT_DOUBLE_EQ (p.getVerDivergence(), ver);
    std::vector<RAY::Ray> rays = p.getRays();
    std::list<double> rayList;
    for (RAY::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(100);
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceSE");
}


TEST (LightSource, PointSourceHardEdgeMis) {
    int number_of_rays = 10000;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    std::vector<double> mis = {2,3,0.005,0.006}; // x, y, psi, phi
    RAY::PointSource p = RAY::PointSource(0, "Point source 1", number_of_rays, width, height, depth, hor, ver, 0,0,0,0, mis); // 0 = hard edge (uniform)
    
    std::vector<RAY::Ray> rays = p.getRays();
    std::list<double> rayList;
    for (RAY::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(100);
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceHE_mis");
}

TEST (LightSource, PointSourceSoftEdgeMis) {
    int number_of_rays = 10000;
    double width = 0.065;
    double height = 0.04;
    double depth = 1.0;
    double hor = 0.001;
    double ver = 0.001;
    std::vector<double> mis = {2,3,0.005,0.006}; // x,y,psi,phi in rad
    RAY::PointSource p = RAY::PointSource(0, "Point source 1", number_of_rays, width, height, depth, hor, ver, 1,1,1,1, mis); // 1 = soft edge (gaussian)
    
    std::vector<RAY::Ray> rays = p.getRays();
    std::list<double> rayList;
    for (RAY::Ray r : rays) {
        rayList.push_back(r.m_position.x);
        rayList.push_back(r.m_position.y);
        rayList.push_back(r.m_position.z);
        rayList.push_back(r.m_weight);
        rayList.push_back(r.m_direction.x);
        rayList.push_back(r.m_direction.y);
        rayList.push_back(r.m_direction.z);
        rayList.push_back(100);
    }
    std::cout << rayList.size() << std::endl;
    writeRaysToFile(rayList, "pointSourceSE_mis");
}