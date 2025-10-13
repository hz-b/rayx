#include "setupTests.h"

namespace {
const auto beamlineFilename = "METRIX_U41_G1_H1_318eV_PS_MLearn_v114";
}  // namespace

TEST_F(TestSuite, RayAttrMask) {
    EXPECT_EQ(RayAttrMask::None, static_cast<RayAttrMask>(0));

    // operator |
    EXPECT_EQ(RayAttrMask::PositionX | RayAttrMask::PositionY | RayAttrMask::PositionZ, RayAttrMask::Position);

    // operator &
    EXPECT_EQ(RayAttrMask::Position & RayAttrMask::PositionY, RayAttrMask::PositionY);

    // operator ^
    EXPECT_EQ(RayAttrMask::Position ^ RayAttrMask::PositionY, RayAttrMask::PositionX | RayAttrMask::PositionZ);

    // operator ~
    EXPECT_EQ(~RayAttrMask::All, RayAttrMask::None);

    // operator !
    EXPECT_TRUE(!RayAttrMask::None);
    EXPECT_FALSE(!RayAttrMask::Position);

    // operator |=
    {
        auto mask = RayAttrMask::PositionX;
        mask |= RayAttrMask::PositionY;
        EXPECT_EQ(mask, RayAttrMask::PositionX | RayAttrMask::PositionY);
    }

    // operator &=
    {
        auto mask = RayAttrMask::Position;
        mask &= RayAttrMask::PositionY;
        EXPECT_EQ(mask, RayAttrMask::PositionY);
    }

    // operator ^=
    {
        auto mask = RayAttrMask::Position;
        mask ^= RayAttrMask::PositionY;
        EXPECT_EQ(mask, RayAttrMask::PositionX | RayAttrMask::PositionZ);
    }

    // contains
    EXPECT_TRUE(contains(RayAttrMask::Position, RayAttrMask::PositionX));
    EXPECT_FALSE(contains(RayAttrMask::Position, RayAttrMask::Energy));

    // exclude
    EXPECT_EQ(exclude(RayAttrMask::Position, RayAttrMask::PositionX), RayAttrMask::PositionY | RayAttrMask::PositionZ);

    // countSetBitsa
    EXPECT_EQ(countSetBits(RayAttrMask::None), 0);
    EXPECT_EQ(countSetBits(RayAttrMask::PositionX), 1);
    EXPECT_EQ(countSetBits(RayAttrMask::Position), 3);
    EXPECT_EQ(countSetBits(RayAttrMask::All), static_cast<std::underlying_type_t<RayAttrMask>>(RayAttrMask::RayAttrMaskCount));

    // isFlag
    EXPECT_TRUE(isFlag(RayAttrMask::PositionX));
    EXPECT_FALSE(isFlag(RayAttrMask::Position));

    // to_string
    EXPECT_EQ(to_string(RayAttrMask::None), std::string(static_cast<int>(RayAttrMask::RayAttrMaskCount), '0'));
    EXPECT_EQ(to_string(RayAttrMask::All), std::string(static_cast<int>(RayAttrMask::RayAttrMaskCount), '1'));
}

TEST_F(TestSuite, traceWithRayAttrMask) {
    const auto rays = traceRml(beamlineFilename, RayAttrMask::Energy);
    EXPECT_EQ(rays.attrMask(), RayAttrMask::Energy) << to_string(rays.attrMask()) << " != " << to_string(RayAttrMask::Position);
}

TEST_F(TestSuite, testObjectRecordMask) {
    const auto beamline = loadBeamline(beamlineFilename);

    // test record no objects
    {
        const auto objectRecordMask = ObjectMask::none();
        const auto rays = tracer->trace(beamline, Sequential::No, objectRecordMask);

        auto objectIds = std::vector<int>(beamline.numObjects());
        std::iota(objectIds.begin(), objectIds.end(), 0);
        expectNotEvenOnce(rays.object_id, objectIds);
    }

    // test record all objects
    {
        const auto objectRecordMask = ObjectMask::all();
        const auto rays = tracer->trace(beamline, Sequential::No, objectRecordMask);

        auto objectIds = std::vector<int>(beamline.numObjects());
        std::iota(objectIds.begin(), objectIds.end(), 0);
        expectAtLeastOnce(rays.object_id, objectIds);
    }

    // test record sources only
    {
        const auto objectRecordMask = ObjectMask::allSources();
        const auto rays = tracer->trace(beamline, Sequential::No, objectRecordMask);

        auto sourceIds = std::vector<int>(beamline.numSources());
        std::iota(sourceIds.begin(), sourceIds.end(), 0);
        expectAtLeastOnce(rays.object_id, sourceIds);

        auto elementIds = std::vector<int>(beamline.numElements());
        std::iota(elementIds.begin(), elementIds.end(), beamline.numSources());
        expectNotEvenOnce(rays.object_id, elementIds);
    }

    // test record elements only
    {
        const auto objectRecordMask = ObjectMask::allElements();
        const auto rays = tracer->trace(beamline, Sequential::No, objectRecordMask);

        auto sourceIds = std::vector<int>(beamline.numSources());
        std::iota(sourceIds.begin(), sourceIds.end(), 0);
        expectNotEvenOnce(rays.object_id, sourceIds);

        auto elementIds = std::vector<int>(beamline.numElements());
        std::iota(elementIds.begin(), elementIds.end(), beamline.numSources());
        expectAtLeastOnce(rays.object_id, elementIds);
    }

    // test record objects determined by index
    {
        const auto numObjects = beamline.numObjects();
        auto objectIds = std::vector<int>();
        objectIds.push_back(std::rand() % numObjects);
        objectIds.push_back(std::rand() % numObjects);
        objectIds.push_back(std::rand() % numObjects);
        auto objectRecordMask = ObjectMask::byIndices(objectIds);
        const auto rays = tracer->trace(beamline, Sequential::No, objectRecordMask);

        expectAtLeastOnce(rays.object_id, objectIds);

        auto objectIdsNotRecorded = std::vector<int>();
        for (int i = 0; i < numObjects; ++i) {
            if (std::find(objectIds.begin(), objectIds.end(), i) == objectIds.end()) objectIdsNotRecorded.push_back(i);
        }
        expectNotEvenOnce(rays.object_id, objectIdsNotRecorded);
    }
}

#ifndef NO_H5
TEST_F(TestSuite, testH5) {
    const auto [beamline, raysOriginal] = loadBeamlineAndTrace(beamlineFilename);
    const auto objectNamesOriginal = beamline.getObjectNames();
    const auto h5Filepath = getBeamlineFilepath(beamlineFilename).replace_extension("testH5.h5");

    // full write and read
    {
        writeH5(h5Filepath, objectNamesOriginal, raysOriginal);
        const auto rays = readH5Rays(h5Filepath);
        CHECK_EQ(rays, raysOriginal);
        const auto objectNames = readH5ObjectNames(h5Filepath);
        EXPECT_EQ(objectNames, objectNamesOriginal);
    }

    // partial write and read
    {
        const auto attrMask = RayAttrMask::Position | RayAttrMask::ObjectId;  // just an example
        writeH5(h5Filepath, objectNamesOriginal, raysOriginal, attrMask);
        const auto rays = readH5Rays(h5Filepath, attrMask);
        const auto partialRaysOriginal = std::move(raysOriginal.copy().filterByAttrMask(attrMask));
        CHECK_EQ(rays, partialRaysOriginal);
    }
}
#endif

TEST_F(TestSuite, testCsv) {
    const auto raysOriginal = traceRml(beamlineFilename);
    const auto csvFilepath = getBeamlineFilepath(beamlineFilename).replace_extension("testCsv.csv");

    // full write and read
    {
        writeCsv(csvFilepath, raysOriginal);
        const auto rays = readCsv(csvFilepath);
        CHECK_EQ(rays, raysOriginal);
    }

    // partial write and read
    {
        const auto attrMask = RayAttrMask::Position | RayAttrMask::ObjectId;  // just an example
        const auto partialRaysOriginal = std::move(raysOriginal.copy().filterByAttrMask(attrMask));
        writeCsv(csvFilepath, partialRaysOriginal);
        const auto rays = readCsv(csvFilepath);
        CHECK_EQ(rays, partialRaysOriginal);
    }
}

TEST_F(TestSuite, testBeamlineBijectionBetweenObjectAndObjectId) {
    // this test loads a beamline where the objects are intentionally out of order in the file,
    // to test that the mapping between object IDs and objects is correct regardless of the order in
    // which they are defined in the file.

    const auto beamline = loadBeamline("ObjectsOutOfOrder");
    EXPECT_EQ(beamline.numObjects(), 3);
    EXPECT_EQ(beamline.findNodeByName("Matrix Source")->getObjectId(), 0);
    EXPECT_EQ(beamline.findNodeByName("Plane Mirror")->getObjectId(), 1);
    EXPECT_EQ(beamline.findNodeByName("Image Plane")->getObjectId(), 2);
    EXPECT_THAT(beamline.getObjectNames(), testing::ElementsAre("Matrix Source", "Plane Mirror", "Image Plane"));
}
