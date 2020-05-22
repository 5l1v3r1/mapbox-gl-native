#include <mbgl/storage/offline.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <gtest/gtest.h>

using namespace mbgl;


TEST(OfflineTilePyramidRegionDefinition, EncodeDecode) {
    const auto kBounds = LatLngBounds::hull({37.6609, -122.5744}, {37.8271, -122.3204});
    OfflineRegionDefinition region("mapbox://style", kBounds, 0, 20, 1.0, true);

    auto encoded = encodeOfflineRegionDefinition(region);
    auto decoded = decodeOfflineRegionDefinition(encoded);

    EXPECT_EQ(decoded.styleURL, region.styleURL);
    EXPECT_EQ(decoded.minZoom, region.minZoom);
    EXPECT_EQ(decoded.maxZoom, region.maxZoom);
    EXPECT_EQ(decoded.pixelRatio, region.pixelRatio);
    EXPECT_EQ(decoded.location, region.location);
    EXPECT_EQ(decoded.isGeometryDefined(), region.isGeometryDefined());
    ASSERT_FALSE(decoded.isGeometryDefined());
    EXPECT_EQ(kBounds, decoded.location.get<LatLngBounds>());
    EXPECT_EQ(decoded.includeIdeographs, region.includeIdeographs);
}

TEST(OfflineGeometryRegionDefinition, EncodeDecode) {
    const Geometry<double> kGeometry{Point<double>(-122.5744, 37.6609)};
    OfflineRegionDefinition region("mapbox://style", kGeometry, 0, 2, 1.0, false);

    auto encoded = encodeOfflineRegionDefinition(region);
    auto decoded = decodeOfflineRegionDefinition(encoded);

    EXPECT_EQ(decoded.styleURL, region.styleURL);
    EXPECT_EQ(decoded.minZoom, region.minZoom);
    EXPECT_EQ(decoded.maxZoom, region.maxZoom);
    EXPECT_EQ(decoded.pixelRatio, region.pixelRatio);
    EXPECT_EQ(decoded.location, region.location);
    EXPECT_EQ(decoded.isGeometryDefined(), region.isGeometryDefined());
    ASSERT_TRUE(decoded.isGeometryDefined());
    EXPECT_EQ(kGeometry, decoded.location.get<Geometry<double>>());
    EXPECT_EQ(decoded.includeIdeographs, region.includeIdeographs);
}
